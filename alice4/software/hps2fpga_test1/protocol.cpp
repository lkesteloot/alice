#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#undef DEBUG_PRINT

// Number of visible pixels.
#define WIDTH 800
#define HEIGHT 480

// Number of pixels we clock out.
#define FULL_WIDTH 992
#define FULL_HEIGHT 500

// For lightweight communication with the FPGA:
#define FPGA_MANAGER_BASE 0xFF706000
#define FPGA_GPO_OFFSET 0x10
#define FPGA_GPI_OFFSET 0x14

// HPS-to-FPGA:
#define H2F_DATA_READY (1 << 0)
#define H2F_HOME_BUTTON (1 << 12)

// FPGA-to-HPS:
#define F2H_BUSY (1 << 0)
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

// Draw type:
#define DRAW_TRIANGLES 0
#define DRAW_LINES 1
#define DRAW_POINTS 2
#define DRAW_LINE_STRIP 3
#define DRAW_TRIANGLE_STRIP 5
#define DRAW_TRIANGLE_FAN 6

#define TEST_PATTERN 0
#define TEST_SPINNING_TRIANGLE 0
#define TEST_TWO_TRIANGLES 0
#define TEST_TWO_OVERLAPPING_TRIANGLES 1

static int home_button = 0;

// Returns time difference in seconds.
double diff_timespecs(struct timespec *t1, struct timespec *t2)
{
    return (t1->tv_sec - t2->tv_sec) + (t1->tv_nsec - t2->tv_nsec)/1000000000.0;
}

int get_home_button(volatile uint32_t *gpi, volatile uint32_t *gpo)
{
    int f2h_home_button = (*gpi & F2H_HOME_BUTTON) != 0;

    if (f2h_home_button != home_button) {
	home_button = f2h_home_button;
	if (home_button) {
	    *gpo |= H2F_HOME_BUTTON;
	} else {
	    *gpo &= ~H2F_HOME_BUTTON;
	}
    }

    return home_button;
}

void cmd_clear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ = CMD_CLEAR
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void cmd_zclear(volatile uint64_t **p, uint16_t z)
{
    *(*p)++ = CMD_ZCLEAR
	| ((uint64_t) z << 16);
}

void cmd_czclear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue, uint16_t z)
{
    *(*p)++ = CMD_CZCLEAR
	| ((uint64_t) z << 16)
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

void cmd_pattern(volatile uint64_t **p,
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

void cmd_draw(volatile uint64_t **p, int type, int count, int z_enable, int pattern_enable)
{
    *(*p)++ = CMD_DRAW
    	| ((uint64_t) type << 8)
	| ((uint64_t) count << 16)
	| ((uint64_t) z_enable << 32)
	| ((uint64_t) pattern_enable << 33);
}

void vertex(volatile uint64_t **p, int x, int y, int z,
    uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ =
    	  ((uint64_t) x << 2)
	| ((uint64_t) y << 15)
	| ((uint64_t) z << 24)
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
    struct timespec before_time;
    struct timespec after_time;

    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == -1) {
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

    int brightness = 1000;
    *gpo = (home_button ? H2F_HOME_BUTTON : 0) | (brightness << 2) | (1 << 1);
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
	    cmd_zclear(&p, 0xFFFF);
	} else if (0) {
	    cmd_clear(&p, 0, 0, 0);
	    cmd_zclear(&p, 0xFFFF);
	} else {
	    cmd_czclear(&p, 0, 0, 0, 0xFFFF);
	}
#if TEST_PATTERN
	cmd_pattern(&p,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL);
#endif
	int pattern_enable = (counter & 0x40) != 0 && TEST_PATTERN;
#if TEST_SPINNING_TRIANGLE
	cmd_draw(&p, DRAW_TRIANGLES, 1, false, pattern_enable);
	vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed + M_PI*2/3)),
	    HEIGHT/2 + (int) (200*cos(counter*speed + M_PI*2/3)),
	    0, 255, 0, 0);
	vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed)),
	    HEIGHT/2 + (int) (200*cos(counter*speed)),
	    0, 0, 255, 0);
	vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed + M_PI*4/3)),
	    HEIGHT/2 + (int) (200*cos(counter*speed + M_PI*4/3)),
	    0, 0, 0, 255);
#endif
#if TEST_TWO_TRIANGLES
	cmd_draw(&p, DRAW_TRIANGLES, 2, false, pattern_enable);

	// vertex(&p, 416, 346, 0, 255, 255, 255);
	// vertex(&p, 392, 346, 0, 255, 255, 255);
	// vertex(&p, 416, 321, 0, 255, 255, 255);

	int dy = ((counter / 50) % 20) - 10;

	// Bottom triangle.
	vertex(&p, 392, 346 + dy, 0, 255, 200, 255);
	vertex(&p, 416, 321 + dy, 0, 255, 200, 255);
	vertex(&p, 392, 321 + dy, 0, 255, 200, 255);

	// Top triangle.
	vertex(&p, 416, 321 + dy, 0, 255, 255, 200);
	vertex(&p, 392, 321 + dy, 0, 255, 255, 200);
	vertex(&p, 416, 296 + dy, 0, 255, 255, 200);

	// vertex(&p, 392, 321, 0, 200, 255, 255);
	// vertex(&p, 416, 296, 0, 200, 255, 255);
	// vertex(&p, 392, 297, 0, 200, 255, 255);
#endif
#if TEST_TWO_OVERLAPPING_TRIANGLES
	// Enable and disable Z buffering every 2 seconds.
	cmd_draw(&p, DRAW_TRIANGLES, 2, counter / 120 % 2, pattern_enable);

	float t = counter*speed;
	float dt = M_PI*2/6;
	int z_front = 10000;
	int z_back = 60000;
	// Switch order of triangles every second.
	for (int i = 0; i < 2; i++) {
	    if ((i == 0) ^ (counter / 60 % 2 == 0)) {
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t)),
		    HEIGHT/2 + (int) (200*cos(t)),
		    z_front, 255, 0, 0);
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 2*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 2*dt)),
		    z_front, 0, 255, 0);
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 4*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 4*dt)),
		    z_back, 0, 0, 255);
	    } else {
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + dt)),
		    HEIGHT/2 + (int) (200*cos(t + dt)),
		    z_back, 255, 255, 0);
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 3*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 3*dt)),
		    z_front, 0, 255, 255);
		vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 5*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 5*dt)),
		    z_front, 255, 0, 255);
	    }
	}
#endif
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
	clock_gettime(CLOCK_MONOTONIC, &before_time);
	while ((*gpi & F2H_BUSY) != 0) {
	    // Busy loop.
	}
	clock_gettime(CLOCK_MONOTONIC, &after_time);
	double elapsed = diff_timespecs(&after_time, &before_time);
	double frame_time = FULL_WIDTH*FULL_HEIGHT/25e6;
	int frames = (int) ceil(elapsed/frame_time);
	double fps = 1/(frames*frame_time);
	printf("Time spent rendering: %.1f ms (%d frames, %.1f FPS) (home button %s)\n",
	    elapsed*1000, frames, fps,
	    get_home_button(gpi, gpo) ? "pressed" : "not pressed");

	counter++;
    }

    exit(EXIT_SUCCESS);
}

