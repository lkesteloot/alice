#include <iostream>
#include <fstream>

#include "VMain.h"
#include "VMain_Main.h"
#include "verilated.h"

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 480;
static const int PIXEL_COUNT = DISPLAY_WIDTH*DISPLAY_HEIGHT;

static uint8_t g_display[PIXEL_COUNT*3];
static int g_pixel = 0;

static void write_ppm() {
    if (g_pixel != PIXEL_COUNT) {
        printf("Warning: Pixel is %d instead of %d at end of frame.\n", g_pixel, PIXEL_COUNT);
    }

    std::cout << "Saving image." << std::endl;

    std::ofstream f;
    f.open("out.ppm", std::ios::out | std::ios::binary);
    f << "P6 " << DISPLAY_WIDTH << " " << DISPLAY_HEIGHT << " 255\n";
    f.write((char *) g_display, sizeof(g_display));
    f.close();
}

int main(int argc, char **argv, char **env) {

    Verilated::commandArgs(argc, argv);

    VMain* top = new VMain;

    while (!Verilated::gotFinish()) {
        if (top->clock_50) {
            if (top->Main->lcd_tick) {
                if (top->Main->next_frame) {
                    write_ppm();
                    g_pixel = 0;
                }
                if (top->Main->lcd_data_enable) {
                    if (g_pixel < PIXEL_COUNT) {
                        g_display[g_pixel*3 + 0] = 128;
                        g_display[g_pixel*3 + 1] = 128;
                        g_display[g_pixel*3 + 2] = 128;
                        g_pixel++;
                    } else {
                        printf("Pixel overran display.\n");
                    }
                }
            }

            // printf("%d %3d %3d\n", top->clock_50, top->Main->lcd_x, top->Main->lcd_y);
        }

        top->eval();

        // Toggle clock.
        top->clock_50 = top->clock_50 ^ 1;
    }

    delete top;

    exit(0);
}

