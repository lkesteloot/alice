#include <iostream>
#include <fstream>
#include <vector>

#include "wavedrom.h"
#include "VMain.h"
#include "VMain_Main.h"
#include "VMain_Frame_buffer__A38000000_L40.h"
#include "verilated.h"

static const int DISPLAY_WIDTH = 4; // XYZ 800;
static const int DISPLAY_HEIGHT = 4; // XYZ 480;
static const int PIXEL_COUNT = DISPLAY_WIDTH*DISPLAY_HEIGHT;
static const int PIXELS_PER_WORD = 2;
static const int BUFFER_COUNT = 3;
static const bool DRAW_FRAME = true;

static uint8_t gLcd[PIXEL_COUNT*3];
static int gLcdIndex = 0;

// The three buffers. Everything is in 64-bit words.
static const uint32_t MEMORY_BASE = 0x7000000;
static uint64_t gSdram[PIXEL_COUNT/PIXELS_PER_WORD*BUFFER_COUNT];
static const uint32_t MEMORY_SIZE = sizeof(gSdram) / sizeof(gSdram[0]);

static void write_ppm() {
    if (gLcdIndex != PIXEL_COUNT) {
        printf("Warning: Pixel is %d instead of %d at end of frame.\n", gLcdIndex, PIXEL_COUNT);
    }

    std::cout << "Saving image." << std::endl;

    std::ofstream f;
    f.open("out.ppm", std::ios::out | std::ios::binary);
    f << "P6 " << DISPLAY_WIDTH << " " << DISPLAY_HEIGHT << " 255\n";
    f.write((char *) gLcd, sizeof(gLcd));
    f.close();
}

uint32_t getPixelColor(int x, int y) {
    return
        DRAW_FRAME && (x == 0 || y == 0 || x == DISPLAY_WIDTH - 1 || y == DISPLAY_HEIGHT - 1)
        ? 0xFFFFFF
        : (x % 256) << 8 | (y % 256);
}

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);
    Verilated::debug(1);

    VMain* top = new VMain;

    printf("------- address ???????? --> ???????????????? (fifo size, fifo read -> fifo q, state, words req, words read, XxY)\n");

    // Turn on debugging display.
    top->sw |= 0x8;

    // Fill memory with a pattern.
    {
        uint64_t address = 0;
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
                uint32_t color1 = getPixelColor(x, y);
                uint32_t color2 = getPixelColor(x + 1, y);
                gSdram[address] = color1 | ((uint64_t) color2 << 32);
                address++;
            }
        }
    }

    WaveDrom waveDrom;
    waveDrom.add(WaveDromSignal("clock"));
    waveDrom.add(WaveDromSignal("sdram_read"));
    waveDrom.add(WaveDromSignal("sdram_read_data_valid"));
    waveDrom.add(WaveDromSignal("fifo_size", true));
    waveDrom.add(WaveDromSignal("fifo_read"));
    waveDrom.add(WaveDromSignal("fifo_write"));
    waveDrom.add(WaveDromSignal("lcd_tick"));
    waveDrom.add(WaveDromSignal("lcd_data_enable"));

    bool previous_lcd_clock = 0;
    int count = 0;

    while (!Verilated::gotFinish() && count < 150) {
        // Toggle clock.
        top->clock_50 = top->clock_50 ^ 1;

        top->eval();

        printf("LCD is showing %02x%02x%02x\n",
                (int) top->Main->lcd_red_d1,
                (int) top->Main->lcd_green_d1,
                (int) top->Main->lcd_blue_d1);

        // Simulate LCD. Latch video on positive edge of LCD clock.
        bool lcd_clock = top->Main->lcd_tick_d1;
        if (lcd_clock && !previous_lcd_clock) {
            if (top->Main->next_frame && gLcdIndex != 0) {
                write_ppm();
                gLcdIndex = 0;
            }
            if (top->Main->lcd_data_enable_delayed_d1) {
                if (gLcdIndex < PIXEL_COUNT) {
                    if (1) {
                        printf("Writing %02x%02x%02x to index %d\n",
                                (int) top->Main->lcd_red_d1,
                                (int) top->Main->lcd_green_d1,
                                (int) top->Main->lcd_blue_d1,
                                (int) gLcdIndex);
                    }

                    gLcd[gLcdIndex*3 + 0] = top->Main->lcd_red_d1;
                    gLcd[gLcdIndex*3 + 1] = top->Main->lcd_green_d1;
                    gLcd[gLcdIndex*3 + 2] = top->Main->lcd_blue_d1;
                    gLcdIndex++;
                } else {
                    printf("Pixel overran display.\n");
                }
            }
        }
        previous_lcd_clock = lcd_clock;

        // RAM.
        if (top->clock_50) {
            // Posedge clock_50, post-eval work.

            // See if we just asked for a read on this clock. Answer immediately.
            if (top->hps_0_f2h_sdram0_data_read) {
                uint32_t address = top->hps_0_f2h_sdram0_data_address;
                if (address < MEMORY_BASE || address >= MEMORY_BASE + MEMORY_SIZE) {
                    printf("WARNING: Reading out-of-bounds address %lx\n",
                            (unsigned long) address);
                    top->hps_0_f2h_sdram0_data_readdata = 0x000000ff0000000ffll;
                } else {
                    top->hps_0_f2h_sdram0_data_readdata = gSdram[address - MEMORY_BASE];
                }
                top->hps_0_f2h_sdram0_data_waitrequest = 0;
                top->hps_0_f2h_sdram0_data_readdatavalid = 1;
            } else {
                top->hps_0_f2h_sdram0_data_waitrequest = 0;
                top->hps_0_f2h_sdram0_data_readdatavalid = 0;
            }
            if (1) {
                printf("%s address %08lx --> %016llx (size %d, %d -> %016llx, %d, %d, %d, %dx%d)\n",
                        top->hps_0_f2h_sdram0_data_read ? "Reading" : "Not    ",
                        (unsigned long) top->hps_0_f2h_sdram0_data_address,
                        (unsigned long long) top->hps_0_f2h_sdram0_data_readdata,
                        (int) top->Main->frame_buffer->fifo_usedw,
                        (int) top->Main->frame_buffer->fifo_read,
                        (unsigned long long) top->Main->frame_buffer->fifo_read_data,
                        (int) top->Main->frame_buffer->m2f_state,
                        (int) top->Main->frame_buffer->words_requested,
                        (int) top->Main->frame_buffer->words_read,
                        (int) top->Main->lcd_x,
                        (int) top->Main->lcd_y);
            }
        }

        top->eval();

        // Save signals before RAM, since those happen mid-clock.
        if (top->clock_50) {
            waveDrom["clock"].add(top->clock_50);
            waveDrom["sdram_read"].add(top->hps_0_f2h_sdram0_data_read);
            waveDrom["sdram_read_data_valid"].add(top->hps_0_f2h_sdram0_data_readdatavalid);
            waveDrom["fifo_size"].add(top->Main->frame_buffer->fifo_usedw);
            waveDrom["fifo_read"].add(top->Main->frame_buffer->fifo_read);
            waveDrom["fifo_write"].add(top->Main->frame_buffer->fifo_write);
            waveDrom["lcd_tick"].add(top->Main->lcd_tick);
            waveDrom["lcd_data_enable"].add(top->Main->lcd_data_enable);
        }

        count++;
    }

    waveDrom.write("out.json");

    delete top;

    exit(0);
}

