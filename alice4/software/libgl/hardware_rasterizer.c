#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "awesome.h"

#include <gl.h>
#include "rasterizer.h"
#include "connection.h"

#undef DEBUG_PRINT
#define COVERAGE_PRINT
#undef DUMP_ALL_COMMANDS
#undef SKIP_FPGA_WORK
#define WARN_SKIPPED_FRAME
#define BREAK_UP_TRIANGLES

static const int32_t DISPLAY_WIDTH = XMAXSCREEN + 1;
static const int32_t DISPLAY_HEIGHT = YMAXSCREEN + 1;

static volatile uint64_t *gpu_protocol_buffer; // The fixed buffer from which the GPU reads commands

// N.B.  This is a const variable meant to be compile-time-only.
// If this variable is changed to be settable at run-time, a
// synchronization point must be added to wait on GPU and copy between
// staging to gpu buffers as necessary.
static const int double_buffer_commands = 1;
static int must_wait_on_gpu = 0;

#define MAX_PROTOCOL_QUAD_COUNT (10 * 1024 * 1024 / sizeof(uint64_t))
static volatile uint64_t staging_protocol_buffer[MAX_PROTOCOL_QUAD_COUNT]; // Optional buffer for protocol double-buffering

static volatile uint64_t *protocol_buffer; // The base of the protocol buffer to be used, either GPU or staging
static volatile uint64_t *protocol_next; // The next location to fill in the protocol buffer

static int rasterizer_start_buffer;

#ifdef COVERAGE_PRINT
static int total_bbox_area = 0;
static int total_triangle_area = 0;
static int broken_count = 0;
#endif

#ifdef BREAK_UP_TRIANGLES
static const int subtriangle_count = 31; // Sweet spot. Must be odd.
static const int max_coverage = 25; // Sweet spot.
static const int min_bbox_area = 1600; // Sweet spot.
#endif

// Compute the point "t" of the way between s0 and s1.
static void interpolate_screen_vertex(screen_vertex *s0, screen_vertex *s1,
    screen_vertex *out, double t)
{
    out->x = (uint16_t) (s0->x + ((int32_t) s1->x - s0->x)*t);
    out->y = (uint16_t) (s0->y + ((int32_t) s1->y - s0->y)*t);
    out->z = (uint32_t) (s0->z + ((int64_t) s1->z - s0->z)*t);
    out->r = (uint8_t) (s0->r + ((int16_t) s1->r - s0->r)*t);
    out->g = (uint8_t) (s0->g + ((int16_t) s1->g - s0->g)*t);
    out->b = (uint8_t) (s0->b + ((int16_t) s1->b - s0->b)*t);
    out->a = (uint8_t) (s0->a + ((int16_t) s1->a - s0->a)*t);

    // To visualize the broken-up triangles.
    if (0) {
	out->r = rand() & 0xFF;
	out->g = rand() & 0xFF;
	out->b = rand() & 0xFF;
    }
}

void gpu_finish_rasterizing()
{
#ifndef SKIP_FPGA_WORK
    // Wait until it's done rasterizing.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to switch rasterization to the other buffer.\n");
#endif
    awesome_wait_for_end_of_rasterization();
#endif /* SKIP_FPGA_WORK */
}

void gpu_start()
{
#ifndef SKIP_FPGA_WORK
    awesome_start_rasterizing();
#endif /* SKIP_FPGA_WORK */
}

void gpu_wait()
{
#ifndef SKIP_FPGA_WORK
    // Wait until it's done rasterizing.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to finish vsync.\n");
#endif
    awesome_wait_for_end_of_processing();
#endif /* SKIP_FPGA_WORK */
}

static float the_linewidth;
static int pattern_enabled = 0;
static int zbuffer_enabled = 0;

static int min(int a, int b)
{
    return (a < b) ? a : b;
}

static int max(int a, int b)
{
    return (a > b) ? a : b;
}

static float clamp(float v, float low, float high)
{
    return v > high ? high : (v < low ? low : v);
}

void rasterizer_clear(uint8_t r, uint8_t g, uint8_t b)
{
    awesome_clear(&protocol_next, r, g, b);
}

void rasterizer_linewidth(float w)
{
    the_linewidth = w;
}

// Packs four uint16 into a single uint64;
static uint64_t pattern16to64(uint16_t pattern[4])
{
    return
	(((uint64_t) pattern[0]) << 0) |
	(((uint64_t) pattern[1]) << 16) |
	(((uint64_t) pattern[2]) << 32) |
	(((uint64_t) pattern[3]) << 48);
}

void rasterizer_setpattern(uint16_t pattern[16])
{
    awesome_pattern(&protocol_next,
    	pattern16to64(pattern + 0),
    	pattern16to64(pattern + 4),
    	pattern16to64(pattern + 8),
    	pattern16to64(pattern + 12));
}

void rasterizer_pattern(int enable)
{
    // Store now, pass in with DRAW command and primitives
    pattern_enabled = enable;
}

static int framestats_print = 1;
static float framestats_frame_duration_sum = 0.0; 
static float framestats_cpu_duration_sum = 0.0; 
static float framestats_copy_duration_sum = 0.0; 
static float framestats_gpu_duration_sum = 0.0; 
static float framestats_raster_duration_sum = 0.0; 
static float framestats_wait_duration_sum = 0.0; 
static int framestats_duration_count = 0; 
static time_t framestats_previous_print_time;
static struct timespec framestats_previous_frame_end;
static struct timespec framestats_rasterizer_start;
static int framestats_skipped_frames = 0;

double diff_timespecs(struct timespec *t1, struct timespec *t2)		
{		
    return (t1->tv_sec - t2->tv_sec) + (t1->tv_nsec - t2->tv_nsec)/1000000000.0;
}

void rasterizer_swap()
{
    awesome_swap(&protocol_next);
    awesome_end(&protocol_next);

#ifdef COVERAGE_PRINT
    if (total_bbox_area != 0) {
	printf("%d bbox / %d triangle = %d%% (%d broken)\n",
	    total_bbox_area, total_triangle_area,
	    total_triangle_area*100/total_bbox_area, broken_count);
    }

    total_bbox_area = 0;
    total_triangle_area = 0;
    broken_count = 0;
#endif

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    if (framestats_print) {
	framestats_cpu_duration_sum += diff_timespecs(&now, &framestats_previous_frame_end);
    }

#ifdef DEBUG_PRINT
    printf("    Wrote %d words\n", protocol_next - protocol_buffer);
#endif // DEBUG_PRINT
#ifdef DUMP_ALL_COMMANDS
    for (volatile uint64_t *t = protocol_buffer; t < protocol_next; t++) {
        printf("fpga cmd 0x%016llX\n", *t);
    }
#endif // DUMP_ALL_COMMANDS

    if(double_buffer_commands) {
	struct timespec then, now;

	// Double buffer the command list.  If the GPU might have
	// been busy, make sure it has finished.
        if(must_wait_on_gpu) {

	    // Wait for GPU to finish rasterizing
	    if(framestats_print) {
		clock_gettime(CLOCK_MONOTONIC, &then);

		gpu_finish_rasterizing();

		clock_gettime(CLOCK_MONOTONIC, &now);
		framestats_raster_duration_sum += diff_timespecs(&now, &then);
	    }

	    // Now wait for VSYNC
	    if(framestats_print)
		clock_gettime(CLOCK_MONOTONIC, &then);

            gpu_wait();

	    if(framestats_print) {
		clock_gettime(CLOCK_MONOTONIC, &now);
		framestats_wait_duration_sum += diff_timespecs(&now, &then);
	    }

            must_wait_on_gpu = 0;
        }

        // Then copy the staing buffer to the GPU buffer
        ptrdiff_t byte_count = (char*)protocol_next - (char*)protocol_buffer;

	if(framestats_print)
	    clock_gettime(CLOCK_MONOTONIC, &then);

	awesome_init_frame();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
        memcpy(gpu_protocol_buffer, protocol_buffer, byte_count);
#pragma GCC diagnostic pop

	if(framestats_print) {
	    clock_gettime(CLOCK_MONOTONIC, &now);
	    framestats_copy_duration_sum += diff_timespecs(&now, &then);
	}

	// Finally, kick off the GPU, and note that we will need
	// to wait for it to finish before copying the staging data
	// next time.
        gpu_start();

        must_wait_on_gpu = 1;

    } else {

	// Don't double buffer.  Just send off the commands to the
	// GPU and then wait to be done.
	struct timespec then, now;

	if(framestats_print)
	    clock_gettime(CLOCK_MONOTONIC, &then);

        gpu_start();

	// Wait for GPU to finish rasterizing if we want to print timing info
	if(framestats_print) {
	    clock_gettime(CLOCK_MONOTONIC, &then);

	    gpu_finish_rasterizing();

	    clock_gettime(CLOCK_MONOTONIC, &now);
	    framestats_raster_duration_sum += diff_timespecs(&now, &then);
	}

        //
        gpu_wait();

	if(framestats_print) {
	    clock_gettime(CLOCK_MONOTONIC, &now);
	    framestats_gpu_duration_sum += diff_timespecs(&now, &then);
	}

	awesome_init_frame();
    }

    protocol_next = protocol_buffer;

    clock_gettime(CLOCK_MONOTONIC, &now);
    float frame_duration = diff_timespecs(&now, &framestats_previous_frame_end);

    if(framestats_print) {
	framestats_frame_duration_sum += frame_duration;

	framestats_duration_count ++;

	if((framestats_print == 2) || (now.tv_sec > framestats_previous_print_time)) {

	    framestats_previous_print_time = now.tv_sec;

	    static int printed_header = 0;

	    if(double_buffer_commands) {
		if(!printed_header)  {
		    printed_header = 1;
		    printf("total frame ms, cpu, copy, raster wait, est total raster, swap wait, skipped\n");
		}
		printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %d\n",
		    framestats_frame_duration_sum / framestats_duration_count * 1000,
		    framestats_cpu_duration_sum / framestats_duration_count * 1000,
		    framestats_copy_duration_sum / framestats_duration_count * 1000,
		    framestats_raster_duration_sum / framestats_duration_count * 1000,
		    (framestats_cpu_duration_sum + framestats_copy_duration_sum + framestats_raster_duration_sum) / framestats_duration_count * 1000,
		    framestats_wait_duration_sum / framestats_duration_count * 1000,
		    framestats_skipped_frames);
	    } else {
		if(!printed_header)  {
		    printed_header = 1;
		    printf("total frame ms, cpu , raster wait, est total raster, swap wait, skipped\n");
		}
		printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %d\n",
		    framestats_frame_duration_sum / framestats_duration_count * 1000,
		    framestats_cpu_duration_sum / framestats_duration_count * 1000,
		    framestats_raster_duration_sum / framestats_duration_count * 1000,
		    (framestats_cpu_duration_sum + framestats_copy_duration_sum + framestats_raster_duration_sum) / framestats_duration_count * 1000,
		    framestats_gpu_duration_sum / framestats_duration_count * 1000,
		    framestats_skipped_frames);
	    }

	    framestats_cpu_duration_sum = 0.0;
	    framestats_copy_duration_sum = 0.0;
	    framestats_wait_duration_sum = 0.0;
	    framestats_gpu_duration_sum = 0.0;
	    framestats_raster_duration_sum = 0.0;
	    framestats_frame_duration_sum = 0.0;
	    framestats_duration_count = 0;
	    framestats_skipped_frames = 0;
	}
    }

    // (800+24+72+96)*(480+3+10+7)/25e6 = 0.01984
    // It'll be a multiple of that. Add 10% for margin. Don't warn
    // if it was too long, that's probably just the app that stopped
    // drawing (like buttonfly).
    if (frame_duration >= .01984*1.1 && frame_duration < .01984*10) {
	framestats_skipped_frames++;
#ifdef WARN_SKIPPED_FRAME
	puts("WARNING: Skipped frame");
#endif
    }

    framestats_previous_frame_end = now;
}

int32_t rasterizer_winopen(char *title)
{
    if(getenv("PRINT_FRAME_STATS") != NULL) {
	framestats_print = atoi(getenv("PRINT_FRAME_STATS"));
	if(framestats_print == 1)
	    printf("will print durations for some portions of frame every second or so\n");
	else if(framestats_print == 2)
	    printf("will print durations for some portions of frame continuously\n");
	else {
	    fprintf(stderr, "%d value for \"PRINT_FRAME_STATS\" is not defined\n", framestats_print);
	    exit(EXIT_FAILURE);
	}

	framestats_previous_print_time = framestats_previous_frame_end.tv_sec;
    }

    clock_gettime(CLOCK_MONOTONIC, &framestats_previous_frame_end);

    // Initialize the interface to the FPGA.
    awesome_init();
    gpu_protocol_buffer = awesome_get_command_buffer();

    if(double_buffer_commands)
        protocol_buffer = staging_protocol_buffer;
    else
        protocol_buffer = gpu_protocol_buffer;

    protocol_next = protocol_buffer;
    awesome_init_frame();
}

void rasterizer_zbuffer(int enable)
{
    // Store now, pass in with DRAW command and primitives
    zbuffer_enabled = enable;
}

void rasterizer_zclear(uint32_t z)
{
    awesome_zclear(&protocol_next, z);
}

void rasterizer_czclear(uint8_t r, uint8_t g, uint8_t b, uint32_t z)
{
    awesome_czclear(&protocol_next, r, g, b, z);
}

void screen_vertex_offset_with_clamp(screen_vertex* v, float dx, float dy)
{
    v->x = clamp(v->x + dx * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE);
    v->y = clamp(v->y + dy * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE);
}

static void draw_screen_triangle(screen_vertex *s0, screen_vertex *s1, screen_vertex *s2)
{
    // Compute the coordinates of the vertices.
    int x0 = s0->x / SCREEN_VERTEX_V2_SCALE;
    int y0 = DISPLAY_HEIGHT - 1 - s0->y / SCREEN_VERTEX_V2_SCALE;
    int x1 = s1->x / SCREEN_VERTEX_V2_SCALE;
    int y1 = DISPLAY_HEIGHT - 1 - s1->y / SCREEN_VERTEX_V2_SCALE;
    int x2 = s2->x / SCREEN_VERTEX_V2_SCALE;
    int y2 = DISPLAY_HEIGHT - 1 - s2->y / SCREEN_VERTEX_V2_SCALE;

    // Compute bbox in pixels.
    int min_x = min(x0, min(x1, x2));
    int min_y = min(y0, min(y1, y2));
    int max_x = max(x0, max(x1, x2));
    int max_y = max(y0, max(y1, y2));
    int bbox_area = (max_x - min_x + 1)*(max_y - min_y + 1);

    // Compute triangle size in pixels.
    int x01 = x1 - x0;
    int y01 = y1 - y0;
    int x12 = x2 - x1;
    int y12 = y2 - y1;
    int x20 = x0 - x2;
    int y20 = y0 - y2;
    int triangle_area = (x01*y20 - y01*x20)/2;
    if (triangle_area < 0) {
	triangle_area = -triangle_area;
    }

#ifdef BREAK_UP_TRIANGLES
    // Compute coverage (what fraction of the bounding box does
    // the triangle cover) in percent.
    int coverage = triangle_area*100/bbox_area;

    // See if we need to break up the triangle. Thin diagonal
    // triangles have low coverage.
    if (coverage < max_coverage && bbox_area > min_bbox_area) {
	// Find shortest edge.
	int len01 = x01*x01 + y01*y01;
	int len12 = x12*x12 + y12*y12;
	int len20 = x20*x20 + y20*y20;
	if (len01 <= len12 && len01 <= len20) {
	    // len01 shortest, rotate.
	    screen_vertex *tmp = s0;
	    s0 = s2;
	    s2 = s1;
	    s1 = tmp;
	} else if (len12 <= len01 && len12 <= len20) {
	    // len12 shortest, nothing to do.
	} else {
	    // len20 shortest, rotate.
	    screen_vertex *tmp = s0;
	    s0 = s1;
	    s1 = s2;
	    s2 = tmp;
	}

	// Tesselate the triangle along its long dimension.
	double t = 1.0/subtriangle_count;

	screen_vertex s[subtriangle_count + 2];
	s[0] = *s0;
	s[subtriangle_count] = *s1;
	s[subtriangle_count + 1] = *s2;
	for (int i = 1; i < subtriangle_count; i++) {
	    interpolate_screen_vertex(s0, i % 2 == 0 ? s2 : s1, &s[i], i*t);
	}

	for (int i = 0; i < subtriangle_count; i++) {
	    if (i % 2 == 0) {
		draw_screen_triangle(&s[i], &s[i + 1], &s[i + 2]);
	    } else {
		// Reverse the triangle so it's consistent
		// with the triangle we were given.
		draw_screen_triangle(&s[i], &s[i + 2], &s[i + 1]);
	    }
	}

#ifdef COVERAGE_PRINT
	broken_count++;
#endif

	return;
    }
#endif // BREAK_UP_TRIANGLES

#ifdef COVERAGE_PRINT
    total_bbox_area += bbox_area;
    total_triangle_area += triangle_area;
#endif

    // Send the triangle to the FPGA.
    awesome_draw(&protocol_next, AWESOME_DRAW_TRIANGLES, 1,
	zbuffer_enabled, pattern_enabled);
    awesome_vertex(&protocol_next, x0, y0, s0->z, s0->r, s0->g, s0->b);
    awesome_vertex(&protocol_next, x1, y1, s1->z, s1->r, s1->g, s1->b);
    awesome_vertex(&protocol_next, x2, y2, s2->z, s2->r, s2->g, s2->b);
}


// In hardware may have to repack to do FPGA's rowbytes
void rasterizer_bitmap(uint32_t width, uint32_t rowbytes, uint32_t height, screen_vertex *sv, uint8_t *bits)
{
    screen_vertex s[4];

    for(int j = 0; j < height; j++) {
        int prevbit = 0;
        int count;
        for(int i = 0; i < width; i++) {
            int bit = (bits[j * rowbytes + i / 8] >> (7 - i % 8)) & 1;

            if(bit) {

                if(!prevbit && bit) {
                    // Previous bit was 0 and this bit is 1, so start a
                    // run
                    for(int k = 0; k < 4; k++) {
                        s[k] = *sv; // Copy color
                        s[k].x = sv->x + SCREEN_VERTEX_V2_SCALE * i;
                        s[k].y = sv->y + (height - j - 1) * SCREEN_VERTEX_V2_SCALE;
                        s[k].z = sv->z;
                    }

                    screen_vertex_offset_with_clamp(&s[0], 0, 0);
                    screen_vertex_offset_with_clamp(&s[1], 0, 1);
                    count = 0;
                }

                // Add this bit to current run
                count++;

            } else if(prevbit) {

                // The previous bit was 1 and this bit is 0, so
                // finish the run
                screen_vertex_offset_with_clamp(&s[2], count, 1);
                screen_vertex_offset_with_clamp(&s[3], count, 0);
// XXX rasterizer_draw()
                draw_screen_triangle(&s[0], &s[1], &s[2]);
                draw_screen_triangle(&s[2], &s[3], &s[0]);
            }

            prevbit = bit;
        }

        if(prevbit) {
            // the end of the row was a 1 bit, so finish run
            screen_vertex_offset_with_clamp(&s[2], count, 1);
            screen_vertex_offset_with_clamp(&s[3], count, 0);
// XXX rasterizer_draw()
            draw_screen_triangle(&s[0], &s[1], &s[2]);
            draw_screen_triangle(&s[2], &s[3], &s[0]);
        }
    }
}

void draw_line_as_triangles(screen_vertex *v0, screen_vertex *v1)
{
// XXX rasterizer_draw() LINE 
// Fake this with screen quads until rasterizer implements lines
    float dx = (v1->x - v0->x);
    float dy = (v1->y - v0->y);
    float d = sqrt(dx * dx + dy * dy);

    if(d == 0.0) {
        // XXX should draw point if the line is too short
        return;
    }

    screen_vertex q[4];
    q[0] = *v0;
    q[1] = *v0;
    q[2] = *v1;
    q[3] = *v1;

    if(fabs(dx) > fabs(dy)) {
        screen_vertex_offset_with_clamp(&q[0], 0, -the_linewidth * .5);
        screen_vertex_offset_with_clamp(&q[1], 0,  the_linewidth * .5);
        screen_vertex_offset_with_clamp(&q[2], 0,  the_linewidth * .5);
        screen_vertex_offset_with_clamp(&q[3], 0, -the_linewidth * .5);
    } else {
        screen_vertex_offset_with_clamp(&q[0], -the_linewidth * .5, 0);
        screen_vertex_offset_with_clamp(&q[1],  the_linewidth * .5, 0);
        screen_vertex_offset_with_clamp(&q[2],  the_linewidth * .5, 0);
        screen_vertex_offset_with_clamp(&q[3], -the_linewidth * .5, 0);
    }

    draw_screen_triangle(&q[0], &q[1], &q[2]);
    draw_screen_triangle(&q[2], &q[3], &q[0]);
}

void rasterizer_draw(uint32_t type, uint32_t count, screen_vertex *screenverts)
{
    switch(type) {
        case DRAW_POINTS:
            fprintf(stderr, "POINTS not implemented\n");
            break;
        case DRAW_LINES:
            for(int i = 0; i < count / 2; i++) { 
                draw_line_as_triangles(&screenverts[i * 2 + 0], &screenverts[i * 2 + 1]);
            }
            break;
        case DRAW_TRIANGLES:
            for(int i = 0; i < count / 3; i++)
                draw_screen_triangle(&screenverts[i * 3], &screenverts[i * 3 + 1], &screenverts[i * 3 + 2]);
            break;
            break;
    }
}

