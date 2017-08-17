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

static struct timespec before_time;
static struct timespec after_time;

static double diff_timespecs(struct timespec *t1, struct timespec *t2) {
    return (t1->tv_sec - t2->tv_sec) + (t1->tv_nsec - t2->tv_nsec)/1000000000.0;
}

void test_clear(int type)
{
    volatile uint64_t *p = awesome_get_command_buffer();

    int count = 20;
    awesome_init_frame();
    for (int i = 0; i < count; i++) {
	switch (type) {
	    case 0:
		awesome_clear(&p, 0, 0, 0);
		break;

	    case 1:
		awesome_zclear(&p, 0);
		break;

	    case 2:
		awesome_czclear(&p, 0, 0, 0, 0);
		break;
	}
    }
    awesome_end(&p);

    clock_gettime(CLOCK_MONOTONIC, &before_time);
    awesome_start_rasterizing();
    awesome_wait_for_end_of_processing();
    clock_gettime(CLOCK_MONOTONIC, &after_time);
    double elapsed = diff_timespecs(&after_time, &before_time);
    printf("%s: %.1f ms (%.0f per second)\n",
    	type == 0 ? "clear()" : type == 1 ? "zclear()" : "czclear()",
    	elapsed/count*1000, count/elapsed);
}

void test_fill_rate(int use_z)
{
    volatile uint64_t *p = awesome_get_command_buffer();

    int count = 50;
    int x1 = 5;
    int y1 = 5;
    int x2 = WIDTH - 5;
    int y2 = HEIGHT - 5;
    int pixels = (y2 - y1)*(x2 - x1);
    int drawn_pixels = pixels / 2;
    awesome_init_frame();
    awesome_draw(&p, AWESOME_DRAW_TRIANGLES, count, use_z, false);
    for (int i = 0; i < count; i++) {
	int z = i << 16;
	awesome_vertex(&p, x1, y1, z, 0, 0, 0);
	awesome_vertex(&p, x2, y1, z, 0, 0, 0);
	awesome_vertex(&p, x2, y2, z, 0, 0, 0);
    }
    awesome_end(&p);

    clock_gettime(CLOCK_MONOTONIC, &before_time);
    awesome_start_rasterizing();
    awesome_wait_for_end_of_processing();
    clock_gettime(CLOCK_MONOTONIC, &after_time);
    double elapsed = diff_timespecs(&after_time, &before_time);
    printf("Fill (%s): %.0f considered mpixels/s, %.0f drawn mpixels/s\n",
    	use_z ? "with Z" : "no Z",
	pixels*count/elapsed/1000000, drawn_pixels*count/elapsed/1000000);
}

void test_triangle_overhead()
{
    volatile uint64_t *p = awesome_get_command_buffer();

    int count = 10000;
    int x1 = 5;
    int y1 = 5;
    int x2 = 6;
    int y2 = 6;
    awesome_init_frame();
    awesome_draw(&p, AWESOME_DRAW_TRIANGLES, count, false, false);
    for (int i = 0; i < count; i++) {
	awesome_vertex(&p, x1, y1, 0, 0, 0, 0);
	awesome_vertex(&p, x2, y1, 0, 0, 0, 0);
	awesome_vertex(&p, x2, y2, 0, 0, 0, 0);
    }
    awesome_end(&p);

    clock_gettime(CLOCK_MONOTONIC, &before_time);
    awesome_start_rasterizing();
    awesome_wait_for_end_of_processing();
    clock_gettime(CLOCK_MONOTONIC, &after_time);
    double elapsed = diff_timespecs(&after_time, &before_time);
    printf("Triangles: %.1f million per second\n", count/elapsed/1000000);
}

int main()
{
    awesome_init();

    test_clear(0);
    test_clear(1);
    test_clear(2);
    test_fill_rate(0);
    test_fill_rate(1);
    test_triangle_overhead();

    exit(EXIT_SUCCESS);
}

