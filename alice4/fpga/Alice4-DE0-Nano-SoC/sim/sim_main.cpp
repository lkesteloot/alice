#include <iostream>
#include <fstream>
#include <vector>

#include "wavedrom.h"
#include "memory.h"
#include "VMain.h"
#include "VMain_Main.h"
#include "VMain_Frame_buffer__pi1.h"
#include "verilated.h"

// Protocol command number:
#define CMD_CLEAR 1
#define CMD_ZCLEAR 2
#define CMD_PATTERN 3
#define CMD_DRAW 4
#define CMD_BITMAP 5
#define CMD_SWAP 6
#define CMD_END 7
#define CMD_CZCLEAR 8

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 480;
static const int PIXEL_COUNT = DISPLAY_WIDTH*DISPLAY_HEIGHT;
static const int PIXELS_PER_WORD = 2;
static const int BUFFER_COUNT = 3;
static const bool DRAW_FRAME = false;
static const bool GENERATE_JSON = false;

static uint8_t gLcd[PIXEL_COUNT*3];
static int gLcdIndex = 0;

// In words.
static const uint32_t MEMORY_BASE = 0x7000000;
static const uint32_t FRAME_BUFFER_SIZE = PIXEL_COUNT/PIXELS_PER_WORD;
static const uint32_t COMMAND_BUFFER_SIZE = 16*1024*1024;
static const uint32_t MEMORY_SIZE = FRAME_BUFFER_SIZE*BUFFER_COUNT + COMMAND_BUFFER_SIZE;

static vluint64_t gMainTime = 0;

/**
 * Return 0 or 1 depending on whether bit "bit" of "value" is on.
 */
static int get_bit(uint64_t value, int bit) {
    return (value >> bit) & 1;
}

/**
 * Return a blank integer with only "to" bit set to the value of "from" bit in "value".
 */
static uint64_t move_bit(uint64_t value, int from, int to) {
    return (uint64_t) get_bit(value, from) << to;
}

/**
 * Put together a byte from the eight bit positions in "value".
 */
static uint8_t assemble_byte(uint64_t value, int b0, int b1, int b2, int b3, int b4, int b5, int b6, int b7) {
    return (uint8_t) (
            move_bit(value, b0, 0) |
            move_bit(value, b1, 1) |
            move_bit(value, b2, 2) |
            move_bit(value, b3, 3) |
            move_bit(value, b4, 4) |
            move_bit(value, b5, 5) |
            move_bit(value, b6, 6) |
            move_bit(value, b7, 7));
}

static void write_ppm(std::string pathname) {
    if (gLcdIndex != PIXEL_COUNT) {
        printf("Warning: Pixel is %d instead of %d at end of frame.\n", gLcdIndex, PIXEL_COUNT);
    }

    std::cout << "Saving image to \"" << pathname << "\"." << std::endl;

    std::ofstream f;
    f.open(pathname, std::ios::out | std::ios::binary);
    f << "P6 " << DISPLAY_WIDTH << " " << DISPLAY_HEIGHT << " 255\n";
    f.write((char *) gLcd, sizeof(gLcd));
    f.close();
}

// Returns 0BGR color
uint32_t getPixelColor(int x, int y) {
    return
        DRAW_FRAME && (x == 0 || y == 0 || x == DISPLAY_WIDTH - 1 || y == DISPLAY_HEIGHT - 1)
        ? 0xFFFFFF
        : (x % 256) << 8 | (y % 256);
}

// Needed by Verilator: https://www.veripool.org/projects/verilator/wiki/Manual-verilator#CONNECTING-TO-C
double sc_time_stamp() {
    return gMainTime;
}

static uint64_t make_vertex(int x, int y, int z, int red, int green, int blue) {
    return ((uint64_t) x << 2)
        | ((uint64_t) y << 15)
        | ((uint64_t) ((uint16_t) (z >> 16)) << 24)
        | ((uint64_t) red << 56)
        | ((uint64_t) green << 48)
        | ((uint64_t) blue << 40);
}

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);
    Verilated::debug(1);

    std::string cmd_pathname;
    if (argc > 1) {
        cmd_pathname = argv[1];
    }

    Memory memory(MEMORY_BASE, MEMORY_SIZE);
    VMain* top = new VMain;

    // Turn on debugging display.
    /// top->sw |= 0x8;

    // Fill front buffer with a pattern.
    {
        uint32_t address = MEMORY_BASE;
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x += 2) {
                uint32_t color1 = getPixelColor(x, y);
                uint32_t color2 = getPixelColor(x + 1, y);
                memory[address] = color1 | ((uint64_t) color2 << 32);
                address++;
            }
        }
    }

    // Fill command buffer.
    uint32_t address = MEMORY_BASE + 3*FRAME_BUFFER_SIZE;
    if (cmd_pathname.empty()) {
        int triangle_count = 1;
        memory[address++] = CMD_CLEAR
            | ((uint64_t) 0 << 56)
            | ((uint64_t) 0 << 48)
            | ((uint64_t) 0 << 40);
        memory[address++] = CMD_DRAW
            | ((uint64_t) 0 << 8) // Triangle type.
            | ((uint64_t) triangle_count << 16) // Triangle count.
            | ((uint64_t) 0 << 32) // Z enable.
            | ((uint64_t) 0 << 33); // Pattern enable.
        for (int i = 0; i < triangle_count; i++) {
            memory[address++] = make_vertex(100 + i*5, 100, 0, 255, 0, 0);
            memory[address++] = make_vertex(300 + i*5, 150, 0, 0, 255, 0);
            memory[address++] = make_vertex(200 + i*5, 200, 0, 0, 0, 255);
        }
        memory[address++] = CMD_SWAP;
        memory[address++] = CMD_END;
    } else {
        // Load binary file cmd_pathname and stick it into memory.
        std::cout << "Loading commands from \"" << cmd_pathname << "\"." << std::endl;

        std::ifstream f(cmd_pathname, std::ios::binary | std::ios::ate);
        int size = f.tellg();
        if (size == -1) {
            std::cout << "Command file not found." << std::endl;
            return 1;
        }
        f.seekg(0);
        unsigned char *buf = new unsigned char[size];
        f.read((char *) buf, size);
        f.close();

        for (int i = 0; i < size/8; i++) {
            memory[address++] =
                ((uint64_t) buf[i*8 + 0] << 0) |
                ((uint64_t) buf[i*8 + 1] << 8) |
                ((uint64_t) buf[i*8 + 2] << 16) |
                ((uint64_t) buf[i*8 + 3] << 24) |
                ((uint64_t) buf[i*8 + 4] << 32) |
                ((uint64_t) buf[i*8 + 5] << 40) |
                ((uint64_t) buf[i*8 + 6] << 48) |
                ((uint64_t) buf[i*8 + 7] << 56);
        }

        delete[] buf;
    }

    /*
    for (uint32_t a = MEMORY_BASE + 3*FRAME_BUFFER_SIZE; a < address; a++) {
        printf(">> 0x%016llx\n", memory[a]);
    }
    */

    WaveDrom waveDrom;
    if (GENERATE_JSON) {
        waveDrom.add(WaveDromSignal("clock"));
        waveDrom.add(WaveDromSignal("sdram_read"));
        waveDrom.add(WaveDromSignal("sdram_read_data_valid"));
        waveDrom.add(WaveDromSignal("fifo_size", true));
        waveDrom.add(WaveDromSignal("fifo_read"));
        waveDrom.add(WaveDromSignal("fifo_write"));
        waveDrom.add(WaveDromSignal("lcd_tick"));
        waveDrom.add(WaveDromSignal("lcd_data_enable"));
    }

    bool previous_lcd_clock = 0;
    int next_frame = 0; // Next frame delay.
    bool data_is_ready = false;
    int frame_number = 0;
    int clock_number = 0;

    while (!Verilated::gotFinish() && frame_number < 105) {
        if (gMainTime == 50) {
            data_is_ready = true;
        }
        top->sim_h2f_value = data_is_ready ? 0x1 : 0x0; // Rasterizer data is ready.
        if (get_bit(top->sim_f2h_value, 0)) {
            data_is_ready = false;
        }

        // Toggle clock.
        top->clock_50 = top->clock_50 ^ 1;

        if (top->clock_50) {
            clock_number++;
        }

        top->eval();

        // Simulate LCD. Latch video on positive edge of LCD clock.
        bool lcd_clock = get_bit(top->gpio_0, 17); // lcd_tick_d1
        if (lcd_clock && !previous_lcd_clock) {
            next_frame = (next_frame << 1) | top->Main->next_frame;
            if ((next_frame & 0x04) && gLcdIndex != 0) {
                std::stringstream ss;
                ss << "out-";
                ss.width(4);
                ss.fill('0');
                ss << frame_number << ".ppm";
                write_ppm(ss.str());
                frame_number++;
                gLcdIndex = 0;

                int ms = clock_number*1000/50000000;
                printf("Frame took %d clocks (%d ms, %d FPS).\n", clock_number, ms, 1000/ms);
                clock_number = 0;
            }
            if (get_bit(top->gpio_0, 25)) { // lcd_data_enable_delayed_d1
                if (gLcdIndex < PIXEL_COUNT) {
                    // See assignments to gpio_0 in Main.v:
                    uint8_t red = assemble_byte(top->gpio_0, 0, 2, 4, 6, 8, 10, 12, 14);
                    uint8_t green = assemble_byte(top->gpio_0, 1, 3, 5, 7, 9, 11, 13, 15);
                    uint8_t blue = assemble_byte(top->gpio_0, 16, 18, 20, 22, 24, 26, 28, 30);

                    if (0) {
                        printf("Writing %02x%02x%02x to index %d\n",
                                (int) red, (int) green, (int) blue, (int) gLcdIndex);
                    }

                    gLcd[gLcdIndex*3 + 0] = red;
                    gLcd[gLcdIndex*3 + 1] = green;
                    gLcd[gLcdIndex*3 + 2] = blue;
                    gLcdIndex++;
                } else {
                    printf("Pixel overran display.\n");
                }
            }
        }
        previous_lcd_clock = lcd_clock;

        // RAM.
        if (top->clock_50) {
            memory.evalRead(
                    top->hps_0_f2h_sdram0_data_burstcount,
                    top->hps_0_f2h_sdram0_data_read,
                    top->hps_0_f2h_sdram0_data_address,
                    top->hps_0_f2h_sdram0_data_waitrequest,
                    top->hps_0_f2h_sdram0_data_readdatavalid,
                    top->hps_0_f2h_sdram0_data_readdata);
            memory.evalRead(
                    top->hps_0_f2h_sdram1_data_burstcount,
                    top->hps_0_f2h_sdram1_data_read,
                    top->hps_0_f2h_sdram1_data_address,
                    top->hps_0_f2h_sdram1_data_waitrequest,
                    top->hps_0_f2h_sdram1_data_readdatavalid,
                    top->hps_0_f2h_sdram1_data_readdata);
            memory.evalRead(
                    top->hps_0_f2h_sdram2_data_burstcount,
                    top->hps_0_f2h_sdram2_data_read,
                    top->hps_0_f2h_sdram2_data_address,
                    top->hps_0_f2h_sdram2_data_waitrequest,
                    top->hps_0_f2h_sdram2_data_readdatavalid,
                    top->hps_0_f2h_sdram2_data_readdata);
            memory.evalWrite(
                    top->hps_0_f2h_sdram3_data_burstcount,
                    top->hps_0_f2h_sdram3_data_write,
                    top->hps_0_f2h_sdram3_data_address,
                    top->hps_0_f2h_sdram3_data_waitrequest,
                    top->hps_0_f2h_sdram3_data_byteenable,
                    top->hps_0_f2h_sdram3_data_writedata);
            memory.evalWrite(
                    top->hps_0_f2h_sdram4_data_burstcount,
                    top->hps_0_f2h_sdram4_data_write,
                    top->hps_0_f2h_sdram4_data_address,
                    top->hps_0_f2h_sdram4_data_waitrequest,
                    top->hps_0_f2h_sdram4_data_byteenable,
                    top->hps_0_f2h_sdram4_data_writedata);

            // Eval again immediately to update dependant wires.
            top->eval();
        }

        if (GENERATE_JSON && top->clock_50) {
            waveDrom["clock"].add(top->clock_50);
            waveDrom["sdram_read"].add(top->hps_0_f2h_sdram0_data_read);
            waveDrom["sdram_read_data_valid"].add(top->hps_0_f2h_sdram0_data_readdatavalid);
            waveDrom["fifo_size"].add(top->Main->frame_buffer->fifo_usedw);
            waveDrom["fifo_read"].add(top->Main->frame_buffer->fifo_read);
            waveDrom["fifo_write"].add(top->Main->frame_buffer->fifo_write);
            waveDrom["lcd_tick"].add(top->Main->lcd_tick);
            waveDrom["lcd_data_enable"].add(top->Main->lcd_data_enable);
        }

        gMainTime++;
    }

    if (GENERATE_JSON) {
        waveDrom.write("out.json");
    }

    top->final();
    delete top;

    exit(0);
}

