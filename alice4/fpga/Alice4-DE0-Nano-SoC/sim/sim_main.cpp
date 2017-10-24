#include <iostream>
#include <fstream>

#include "VMain.h"
#include "VMain_Main.h"
#include "VMain_Frame_buffer__pi1.h"
#include "verilated.h"

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 480;
static const int PIXEL_COUNT = DISPLAY_WIDTH*DISPLAY_HEIGHT;

static uint8_t g_lcd[PIXEL_COUNT*3];
static int g_lcd_index = 0;

// The three buffers. Everything is in 64-bit words.
static const uint32_t MEMORY_BASE = 0x7000000;
static uint64_t g_sdram[PIXEL_COUNT/2*3];
static const uint32_t MEMORY_SIZE = sizeof(g_sdram) / sizeof(g_sdram[0]);

static void write_ppm() {
    if (g_lcd_index != PIXEL_COUNT) {
        printf("Warning: Pixel is %d instead of %d at end of frame.\n", g_lcd_index, PIXEL_COUNT);
    }

    std::cout << "Saving image." << std::endl;

    std::ofstream f;
    f.open("out.ppm", std::ios::out | std::ios::binary);
    f << "P6 " << DISPLAY_WIDTH << " " << DISPLAY_HEIGHT << " 255\n";
    f.write((char *) g_lcd, sizeof(g_lcd));
    f.close();
}

int main(int argc, char **argv, char **env) {

    Verilated::commandArgs(argc, argv);

    VMain* top = new VMain;

    // Turn on debugging display.
    top->sw |= 0x8;

    // Fill memory with a pattern.
    {
        uint64_t address = 0;
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
                uint32_t color = (x % 256) << 8 | (y % 256);
                g_sdram[address] = color | ((uint64_t) color << 32);
                address++;
            }
        }
    }

    while (!Verilated::gotFinish()) {
        if (top->clock_50) {
            if (top->Main->lcd_tick_d1) {
                if (top->Main->next_frame) {
                    write_ppm();
                    g_lcd_index = 0;
                }
                if (top->Main->lcd_data_enable_delayed_d1) {
                    if (g_lcd_index < PIXEL_COUNT) {
                        g_lcd[g_lcd_index*3 + 0] = top->Main->lcd_red_d1;
                        g_lcd[g_lcd_index*3 + 1] = top->Main->lcd_green_d1;
                        g_lcd[g_lcd_index*3 + 2] = top->Main->lcd_blue_d1;
                        g_lcd_index++;
                    } else {
                        printf("Pixel overran display.\n");
                    }
                }
            }
        } else {
            // Simulate front buffer RAM.
            if (top->Main->sdram0_read) {
                uint32_t address = top->Main->sdram0_address;
                if (address < MEMORY_BASE || address >= MEMORY_BASE + MEMORY_SIZE) {
                    printf("WARNING: Reading out-of-bounds address %lx\n",
                            (unsigned long) address);
                    top->Main->sdram0_readdata = 0x000000ff0000000ffll;
                } else {
                    top->Main->sdram0_readdata = g_sdram[address - MEMORY_BASE];
                }
                top->Main->sdram0_waitrequest = 0;
                top->Main->sdram0_readdatavalid = 1;
            } else {
                top->Main->sdram0_waitrequest = 0;
                top->Main->sdram0_readdatavalid = 0;
            }
            if (0) {
                printf("%s address %lx --> %llx (%d, %llx, %d, %d, %d)\n",
                        top->Main->sdram0_read ? "Reading" : "Not reading",
                        (unsigned long) top->Main->sdram0_address,
                        (unsigned long long) top->Main->sdram0_readdata,
                        (int) top->Main->frame_buffer->fifo_usedw,
                        (unsigned long long) top->Main->frame_buffer->fifo_read_data,
                        (int) top->Main->frame_buffer->m2f_state,
                        (int) top->Main->frame_buffer->words_requested,
                        (int) top->Main->frame_buffer->words_read);
            }
        }

        top->eval();

        // Toggle clock.
        top->clock_50 = top->clock_50 ^ 1;
    }

    delete top;

    exit(0);
}

