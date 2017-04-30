#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <math.h>

#undef DEBUG_PRINT

// For lightweight communication with the FPGA:
#define FPGA_MANAGER_BASE 0xFF706000
#define FPGA_GPO_OFFSET 0x10
#define FPGA_GPI_OFFSET 0x14

// HPS-to-FPGA:
#define H2F_DATA_READY (1 << 0)

// FPGA-to-HPS:
#define F2H_BUSY (1 << 0)

// Shared memory addresses:
#define BASE 0x38000000
#define RAM_SIZE 0x40000000
#define COLOR_BUFFER_1_OFFSET 0
#define BUFFER_SIZE (800*480*4)
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

// Draw type:
#define DRAW_TRIANGLES 0
#define DRAW_LINES 1
#define DRAW_POINTS 2
#define DRAW_LINE_STRIP 3
#define DRAW_TRIANGLE_STRIP 5
#define DRAW_TRIANGLE_FAN 6

void cmd_clear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ = CMD_CLEAR
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void cmd_draw(volatile uint64_t **p, int type, int count)
{
    *(*p)++ = CMD_DRAW
    	| ((uint64_t) type << 8)
	| ((uint64_t) count << 16);
}

void vertex(volatile uint64_t **p, int x, int y, int z,
    uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ =
    	  ((uint64_t) x << 2)
	| ((uint64_t) y << 15)
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void cmd_swap(volatile uint64_t **p)
{
    *(*p)++ = CMD_SWAP;
}

void cmd_end(volatile uint64_t **p)
{
    *(*p)++ = CMD_END;
}

int main()
{
    float speed = 0.01;

    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Get access to lightweight FPGA communication.
    uint8_t *fpga_manager_base = (uint8_t *) mmap(0, 64,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, FPGA_MANAGER_BASE);
    if(fpga_manager_base == MAP_FAILED) {
        perror("mmap for fpga manager base");
        exit(EXIT_FAILURE);
    }
    volatile uint32_t *gpo = (uint32_t*)(fpga_manager_base + FPGA_GPO_OFFSET);
    volatile uint32_t *gpi = (uint32_t*)(fpga_manager_base + FPGA_GPI_OFFSET);

    // Get access to the various RAM buffers.
    uint8_t *buffers_base = (uint8_t *) mmap(0, RAM_SIZE - BASE,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, BASE);
    if(buffers_base == MAP_FAILED) {
        perror("mmap for buffers");
        exit(EXIT_FAILURE);
    }
    volatile uint64_t *protocol_buffer =
	(uint64_t *) (buffers_base + PROTOCOL_BUFFER_OFFSET);

    *gpo = 0;
    int counter = 0;
    while (1) {
	// Start of frame.
	if ((*gpi & F2H_BUSY) != 0) {
	    printf("Warning: FPGA is busy at top of loop.\n");
	}

	// Write protocol buffer.
#ifdef DEBUG_PRINT
	printf("Writing protocol buffer.\n");
#endif
	volatile uint64_t *p = protocol_buffer;
	if (0) {
	    cmd_clear(&p,
		(counter & 0x04) ? 255 : 0,
		(counter & 0x02) ? 255 : 0,
		(counter & 0x01) ? 255 : 0);
	} else {
	    cmd_clear(&p, 0, 0, 0);
	}
	cmd_draw(&p, DRAW_TRIANGLES, 1);
	vertex(&p,
	    800/2 + (int) (200*sin(counter*speed)),
	    450/2 + (int) (200*cos(counter*speed)),
	    0, 50, 100, 150);
	vertex(&p,
	    800/2 + (int) (200*sin(counter*speed + M_PI*2/3)),
	    450/2 + (int) (200*cos(counter*speed + M_PI*2/3)),
	    0, 50, 100, 150);
	vertex(&p,
	    800/2 + (int) (200*sin(counter*speed + M_PI*4/3)),
	    450/2 + (int) (200*cos(counter*speed + M_PI*4/3)),
	    0, 50, 100, 150);
	cmd_swap(&p);
	cmd_end(&p);
#ifdef DEBUG_PRINT
	printf("    Wrote %d words:\n", p - protocol_buffer);
	for (volatile uint64_t *t = protocol_buffer; t < p; t++) {
	    printf("        0x%016llX\n", *t);
	}
#endif

	// Tell FPGA that our data is ready.
#ifdef DEBUG_PRINT
	printf("Telling FPGA that the data is ready.\n");
#endif
	*gpo |= H2F_DATA_READY;

	// Wait until we find out that it has heard us.
#ifdef DEBUG_PRINT
	printf("Waiting for FPGA to get busy.\n");
#endif
	while ((*gpi & F2H_BUSY) == 0) {
	    // Busy loop.
	}

	// Let FPGA know that we know that it's busy.
#ifdef DEBUG_PRINT
	printf("Telling FPGA that we know it's busy.\n");
#endif
	*gpo &= ~H2F_DATA_READY;

	// Wait until it's done rasterizing.
#ifdef DEBUG_PRINT
	printf("Waiting for FPGA to finish rasterizing.\n");
#endif
	while ((*gpi & F2H_BUSY) != 0) {
	    // Busy loop.
	}

	// usleep(1000*100);
	counter++;
    }

    exit(EXIT_SUCCESS);
}

