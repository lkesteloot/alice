#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "awesome.h"

#undef DEBUG_PRINT

// Number of visible pixels.
#define WIDTH 800
#define HEIGHT 480

#define TEST_PATTERN 0
#define TEST_SPINNING_TRIANGLE 0
#define TEST_TWO_TRIANGLES 0
#define TEST_TWO_OVERLAPPING_TRIANGLES 1

int main()
{
    float speed = 0.01;
    Awesome awesome;

    awesome_init(&awesome);

    int counter = 0;
    while (1) {
	awesome_init_frame(&awesome);
	volatile uint64_t *p = awesome_get_command_buffer(&awesome);

	if (0) {
	    awesome_clear(&p,
		(counter & 0x04) ? 255 : 0,
		(counter & 0x02) ? 255 : 0,
		(counter & 0x01) ? 255 : 0);
	    awesome_zclear(&p, 0xFFFF);
	} else if (0) {
	    awesome_clear(&p, 0, 0, 0);
	    awesome_zclear(&p, 0xFFFF);
	} else {
	    awesome_czclear(&p, 0, 0, 0, 0xFFFF);
	}
#if TEST_PATTERN
	awesome_pattern(&p,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL,
		0x5555aaaa5555aaaaLL);
#endif
	int pattern_enable = (counter & 0x40) != 0 && TEST_PATTERN;
#if TEST_SPINNING_TRIANGLE
	awesome_draw(&p, AWESOME_DRAW_TRIANGLES, 1, false, pattern_enable);
	awesome_vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed + M_PI*2/3)),
	    HEIGHT/2 + (int) (200*cos(counter*speed + M_PI*2/3)),
	    0, 255, 0, 0);
	awesome_vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed)),
	    HEIGHT/2 + (int) (200*cos(counter*speed)),
	    0, 0, 255, 0);
	awesome_vertex(&p,
	    WIDTH/2 + (int) (200*sin(counter*speed + M_PI*4/3)),
	    HEIGHT/2 + (int) (200*cos(counter*speed + M_PI*4/3)),
	    0, 0, 0, 255);
#endif
#if TEST_TWO_TRIANGLES
	awesome_draw(&p, AWESOME_DRAW_TRIANGLES, 2, false, pattern_enable);

	// awesome_vertex(&p, 416, 346, 0, 255, 255, 255);
	// awesome_vertex(&p, 392, 346, 0, 255, 255, 255);
	// awesome_vertex(&p, 416, 321, 0, 255, 255, 255);

	int dy = ((counter / 50) % 20) - 10;

	// Bottom triangle.
	awesome_vertex(&p, 392, 346 + dy, 0, 255, 200, 255);
	awesome_vertex(&p, 416, 321 + dy, 0, 255, 200, 255);
	awesome_vertex(&p, 392, 321 + dy, 0, 255, 200, 255);

	// Top triangle.
	awesome_vertex(&p, 416, 321 + dy, 0, 255, 255, 200);
	awesome_vertex(&p, 392, 321 + dy, 0, 255, 255, 200);
	awesome_vertex(&p, 416, 296 + dy, 0, 255, 255, 200);

	// awesome_vertex(&p, 392, 321, 0, 200, 255, 255);
	// awesome_vertex(&p, 416, 296, 0, 200, 255, 255);
	// awesome_vertex(&p, 392, 297, 0, 200, 255, 255);
#endif
#if TEST_TWO_OVERLAPPING_TRIANGLES
	// Enable and disable Z buffering every 2 seconds.
	awesome_draw(&p, AWESOME_DRAW_TRIANGLES, 2, counter / 120 % 2, pattern_enable);

	float t = counter*speed;
	float dt = M_PI*2/6;
	int z_front = 10000;
	int z_back = 1000000000;
	// Switch order of triangles every second.
	for (int i = 0; i < 2; i++) {
	    if ((i == 0) ^ (counter / 60 % 2 == 0)) {
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t)),
		    HEIGHT/2 + (int) (200*cos(t)),
		    z_front, 255, 0, 0);
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 2*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 2*dt)),
		    z_front, 0, 255, 0);
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 4*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 4*dt)),
		    z_back, 0, 0, 255);
	    } else {
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + dt)),
		    HEIGHT/2 + (int) (200*cos(t + dt)),
		    z_back, 255, 255, 0);
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 3*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 3*dt)),
		    z_front, 0, 255, 255);
		awesome_vertex(&p,
		    WIDTH/2 + (int) (200*sin(t + 5*dt)),
		    HEIGHT/2 + (int) (200*cos(t + 5*dt)),
		    z_front, 255, 0, 255);
	    }
	}
#endif
	awesome_swap(&p);
	awesome_end(&p);
	awesome_start_rasterizing(&awesome);
	awesome_wait_for_end_of_processing(&awesome);
	counter++;
    }

    exit(EXIT_SUCCESS);
}

