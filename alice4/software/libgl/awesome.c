#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>
#include "awesome.h"

#undef DEBUG_PRINT

// Number of visible pixels.
#define WIDTH 800
#define HEIGHT 480

// Number of pixels we clock out.
#define FULL_WIDTH 992
#define FULL_HEIGHT 500

// 10 MB of buffer.
#define MAX_COMMAND_QUAD_COUNT (10 * 1024 * 1024 / sizeof(uint64_t))

// For lightweight communication with the FPGA:
#define FPGA_MANAGER_BASE 0xFF706000
#define FPGA_GPO_OFFSET 0x10
#define FPGA_GPI_OFFSET 0x14

// HPS-to-FPGA:
#define H2F_DATA_READY (1 << 0)
#define H2F_BRIGHTNESS_ENABLED (1 << 1)
#define H2F_BRIGHTNESS_SHIFT 2
#define H2F_BRIGHTNESS_MASK (0x3FF << H2F_BRIGHTNESS_SHIFT)
#define H2F_HOME_BUTTON (1 << 12)

// FPGA-to-HPS:
#define F2H_BUSY (1 << 0)
#define F2H_FRAME_BUFFER_FRONTBUFFER (1 << 1)
#define F2H_RASTERIZER_FRONTBUFFER (1 << 2)
#define F2H_HOME_BUTTON (1 << 3)

// Shared memory addresses:
#define BASE 0x38000000
#define RAM_SIZE 0x40000000
#define COLOR_BUFFER_1_OFFSET 0
#define BUFFER_SIZE (WIDTH*HEIGHT*4)
#define COLOR_BUFFER_2_OFFSET (COLOR_BUFFER_1_OFFSET + BUFFER_SIZE)
#define Z_BUFFER_OFFSET (COLOR_BUFFER_2_OFFSET + BUFFER_SIZE)
#define PROTOCOL_BUFFER_OFFSET (Z_BUFFER_OFFSET + BUFFER_SIZE)

// Protocol command number:
#define CMD_CLEAR 1
#define CMD_ZCLEAR 2
#define CMD_PATTERN 3
#define CMD_DRAW 4
#define CMD_BITMAP 5
#define CMD_SWAP 6
#define CMD_END 7
#define CMD_CZCLEAR 8

static int get_rasterizer_buffer_number(Awesome *awesome)
{
    return (*awesome->gpi & F2H_RASTERIZER_FRONTBUFFER) != 0;
}

void awesome_init(Awesome *awesome)
{
    int dev_mem = open("/dev/mem", O_RDWR);
    if (dev_mem == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Get access to lightweight FPGA communication.
    uint8_t *fpga_manager_base = (uint8_t *) mmap(0, 64,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, FPGA_MANAGER_BASE);
    if (fpga_manager_base == MAP_FAILED) {
        perror("mmap for fpga manager base");
        exit(EXIT_FAILURE);
    }
    awesome->gpo = (uint32_t*)(fpga_manager_base + FPGA_GPO_OFFSET);
    awesome->gpi = (uint32_t*)(fpga_manager_base + FPGA_GPI_OFFSET);

    // Get access to the various RAM buffers.
    uint8_t *buffers_base = (uint8_t *) mmap(0, RAM_SIZE - BASE,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, BASE);
    if (buffers_base == MAP_FAILED) {
        perror("mmap for buffers");
        exit(EXIT_FAILURE);
    }
    awesome->command_buffer =
	(uint64_t *) (buffers_base + PROTOCOL_BUFFER_OFFSET);

    awesome->home_button = 0;
    awesome->rasterizer_buffer_number = 0;
}

volatile uint64_t *awesome_get_command_buffer(Awesome *awesome)
{
    return awesome->command_buffer;
}

int awesome_get_home_button(Awesome *awesome)
{
    int f2h_home_button = (*awesome->gpi & F2H_HOME_BUTTON) != 0;

    if (f2h_home_button != awesome->home_button) {
	awesome->home_button = f2h_home_button;
	if (awesome->home_button) {
	    *awesome->gpo |= H2F_HOME_BUTTON;
	} else {
	    *awesome->gpo &= ~H2F_HOME_BUTTON;
	}
    }

    return awesome->home_button;
}

void awesome_clear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ = CMD_CLEAR
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void awesome_zclear(volatile uint64_t **p, uint16_t z)
{
    *(*p)++ = CMD_ZCLEAR
	| ((uint64_t) z << 16);
}

void awesome_czclear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue, uint16_t z)
{
    *(*p)++ = CMD_CZCLEAR
	| ((uint64_t) z << 16)
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void awesome_pattern(volatile uint64_t **p,
	uint64_t pattern0,
	uint64_t pattern1,
	uint64_t pattern2,
	uint64_t pattern3)
{
    *(*p)++ = CMD_PATTERN;
    *(*p)++ = pattern0;
    *(*p)++ = pattern1;
    *(*p)++ = pattern2;
    *(*p)++ = pattern3;
}

void awesome_draw(volatile uint64_t **p, int type, int count, int z_enable, int pattern_enable)
{
    *(*p)++ = CMD_DRAW
    	| ((uint64_t) type << 8)
	| ((uint64_t) count << 16)
	| ((uint64_t) z_enable << 32)
	| ((uint64_t) pattern_enable << 33);
}

void awesome_vertex(volatile uint64_t **p, int x, int y, int z,
    uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ =
    	  ((uint64_t) x << 2)
	| ((uint64_t) y << 15)
	| ((uint64_t) ((uint16_t) (z >> 16)) << 24)
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void awesome_swap(volatile uint64_t **p)
{
    *(*p)++ = CMD_SWAP;
}

void awesome_end(volatile uint64_t **p)
{
    *(*p)++ = CMD_END;
}

void awesome_set_brightness(Awesome *awesome, int brightness)
{
    *awesome->gpo = (*awesome->gpo & ~H2F_BRIGHTNESS_MASK)
	| (brightness << H2F_BRIGHTNESS_SHIFT)
	| H2F_BRIGHTNESS_ENABLED;
}

void awesome_disable_brightness(Awesome *awesome)
{
    *awesome->gpo &= ~H2F_BRIGHTNESS_ENABLED;
}

void awesome_init_frame(Awesome *awesome)
{
    // Start of frame.
    if ((*awesome->gpi & F2H_BUSY) != 0) {
	printf("Warning: FPGA is busy at top of loop.\n");
    }
}

void awesome_start_rasterizing(Awesome *awesome)
{
    // Tell FPGA that our data is ready.
#ifdef DEBUG_PRINT
    printf("Telling FPGA that the data is ready.\n");
#endif
    *awesome->gpo |= H2F_DATA_READY;

    // Wait until we find out that it has heard us.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to get busy.\n");
#endif
    while ((*awesome->gpi & F2H_BUSY) == 0) {
	// Busy loop.
    }

    // Record which buffer the rasterizer is drawing into.
    awesome->rasterizer_buffer_number = get_rasterizer_buffer_number(awesome);

    // Let FPGA know that we know that it's busy.
#ifdef DEBUG_PRINT
    printf("Telling FPGA that we know it's busy.\n");
#endif
    *awesome->gpo &= ~H2F_DATA_READY;
}

void awesome_wait_for_end_of_rasterization(Awesome *awesome)
{
    // Wait until it's done rasterizing.
    while (get_rasterizer_buffer_number(awesome) ==
	awesome->rasterizer_buffer_number) {

        // Busy loop.
    }
}

void awesome_wait_for_end_of_processing(Awesome *awesome)
{
    // Wait until all commands have been processed.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to finish rasterizing.\n");
#endif
    while ((*awesome->gpi & F2H_BUSY) != 0) {
	// Busy loop.
    }
}
