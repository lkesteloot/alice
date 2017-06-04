#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include "awesome.h"

#include <gl.h>
#include "rasterizer.h"
#include "connection.h"

#undef DEBUG_PRINT
#undef DEBUG_PRINT_VERBOSE
#undef SKIP_FPGA_WORK

static const int32_t DISPLAY_WIDTH = XMAXSCREEN + 1;
static const int32_t DISPLAY_HEIGHT = YMAXSCREEN + 1;

static volatile uint64_t *gpu_protocol_buffer; // The fixed buffer from which the GPU reads commands

// N.B.  This is a const variable meant to be compile-time-only.
// If this variable is changed to be settable at run-time, a
// synchronization point must be added to wait on GPU and copy between
// staging to gpu buffers as necessary.
static const int double_buffer_commands = 0;
static int must_wait_on_gpu = 0;

#define MAX_PROTOCOL_QUAD_COUNT (10 * 1024 * 1024 / sizeof(uint64_t))
static volatile uint64_t staging_protocol_buffer[MAX_PROTOCOL_QUAD_COUNT]; // Optional buffer for protocol double-buffering

static volatile uint64_t *protocol_buffer; // The base of the protocol buffer to be used, either GPU or staging
static volatile uint64_t *protocol_next; // The next location to fill in the protocol buffer

static int rasterizer_start_buffer;

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

static float min(float a, float b)
{
    return (a < b) ? a : b;
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

static int framestats_print = 0;
static float framestats_frame_duration_sum = 0.0; 
static float framestats_cpu_duration_sum = 0.0; 
static float framestats_copy_duration_sum = 0.0; 
static float framestats_gpu_duration_sum = 0.0; 
static float framestats_raster_duration_sum = 0.0; 
static float framestats_wait_duration_sum = 0.0; 
static int framestats_duration_count = 0; 
static time_t framestats_previous_print_time;
static struct timeval framestats_previous_frame_end;
static struct timeval framestats_rasterizer_start;

float diff_timevals(struct timeval *t1, struct timeval *t2)
{
    return (t1->tv_sec - t2->tv_sec) + (t1->tv_usec / 1000000.0 - t2->tv_usec / 1000000.0);
}

void rasterizer_swap()
{
    awesome_swap(&protocol_next);
    awesome_end(&protocol_next);

    if(framestats_print) {
	struct timeval now;
	float duration;

	gettimeofday(&now, NULL);
	framestats_cpu_duration_sum += diff_timevals(&now, &framestats_previous_frame_end);
    }

#ifdef DEBUG_PRINT
    printf("    Wrote %d words\n", protocol_next - protocol_buffer);
#ifdef DEBUG_PRINT_VERBOSE
    for (volatile uint64_t *t = protocol_buffer; t < protocol_next; t++) {
        printf("        0x%016llX\n", *t);
    }
#endif // DEBUG_PRINT_VERBOSE
#endif // DEBUG_PRINT

    if(double_buffer_commands) {
	struct timeval then, now;

	// Double buffer the command list.  If the GPU might have
	// been busy, make sure it has finished.
        if(must_wait_on_gpu) {

	    // Wait for GPU to finish rasterizing
	    if(framestats_print) {
		gettimeofday(&then, NULL);

		gpu_finish_rasterizing();

		gettimeofday(&now, NULL);
		float duration = diff_timevals(&now, &then);
		framestats_raster_duration_sum += duration;
	    }

	    // Now wait for VSYNC
	    if(framestats_print)
		gettimeofday(&then, NULL);

            gpu_wait();

	    if(framestats_print) {
		gettimeofday(&now, NULL);
		float duration = diff_timevals(&now, &then);
		framestats_wait_duration_sum += duration;
	    }

            must_wait_on_gpu = 0;
        }

        // Then copy the staing buffer to the GPU buffer
        ptrdiff_t byte_count = (char*)protocol_next - (char*)protocol_buffer;

	if(framestats_print)
	    gettimeofday(&then, NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
        memcpy(gpu_protocol_buffer, protocol_buffer, byte_count);
#pragma GCC diagnostic pop

	if(framestats_print) {
	    gettimeofday(&now, NULL);
	    framestats_copy_duration_sum += diff_timevals(&now, &then);
	}

	// Finally, kick off the GPU, and note that we will need
	// to wait for it to finish before copying the staging data
	// next time.
        gpu_start();

        must_wait_on_gpu = 1;

    } else {

	// Don't double buffer.  Just send off the commands to the
	// GPU and then wait to be done.
	struct timeval then, now;

	if(framestats_print)
	    gettimeofday(&then, NULL);

        gpu_start();

	// Wait for GPU to finish rasterizing if we want to print timing info
	if(framestats_print) {
	    gettimeofday(&then, NULL);

	    gpu_finish_rasterizing();

	    gettimeofday(&now, NULL);
	    float duration = diff_timevals(&now, &then);
	    framestats_raster_duration_sum += duration;
	}

        //
        gpu_wait();

	if(framestats_print) {
	    gettimeofday(&now, NULL);
	    framestats_gpu_duration_sum += diff_timevals(&now, &then);
	}
    }

    protocol_next = protocol_buffer;
    awesome_init_frame();

    if(framestats_print) {
	struct timeval now;
	float duration;

	gettimeofday(&now, NULL);

	framestats_frame_duration_sum += diff_timevals(&now, &framestats_previous_frame_end);

	framestats_duration_count ++;
	framestats_previous_frame_end = now;

	if((framestats_print == 2) || (now.tv_sec > framestats_previous_print_time)) {

	    framestats_previous_print_time = now.tv_sec;

	    static int printed_header = 0;

	    if(double_buffer_commands) {
		if(!printed_header)  {
		    printed_header = 1;
		    printf("total frame ms, cpu, copy, raster wait, est total raster, F2H_BUSY wait\n");
		}
		printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n",
		    framestats_frame_duration_sum / framestats_duration_count * 1000,
		    framestats_cpu_duration_sum / framestats_duration_count * 1000,
		    framestats_copy_duration_sum / framestats_duration_count * 1000,
		    framestats_raster_duration_sum / framestats_duration_count * 1000,
		    (framestats_cpu_duration_sum + framestats_copy_duration_sum + framestats_raster_duration_sum) / framestats_duration_count * 1000,
		    framestats_wait_duration_sum / framestats_duration_count * 1000);
	    } else {
		if(!printed_header)  {
		    printed_header = 1;
		    printf("total frame ms, cpu , raster wait, est total raster, F2H_BUSY wait\n");
		}
		printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n",
		    framestats_frame_duration_sum / framestats_duration_count * 1000,
		    framestats_cpu_duration_sum / framestats_duration_count * 1000,
		    framestats_raster_duration_sum / framestats_duration_count * 1000,
		    (framestats_cpu_duration_sum + framestats_copy_duration_sum + framestats_raster_duration_sum) / framestats_duration_count * 1000,
		    framestats_gpu_duration_sum / framestats_duration_count * 1000);
	    }

	    framestats_cpu_duration_sum = 0.0;
	    framestats_copy_duration_sum = 0.0;
	    framestats_wait_duration_sum = 0.0;
	    framestats_gpu_duration_sum = 0.0;
	    framestats_raster_duration_sum = 0.0;
	    framestats_frame_duration_sum = 0.0;
	    framestats_duration_count = 0;
	}
    }
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
	gettimeofday(&framestats_previous_frame_end, NULL);

	framestats_previous_print_time = framestats_previous_frame_end.tv_sec;
    }

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

void screen_vertex_offset_with_clamp(screen_vertex* v, float dx, float dy)
{
    v->x = clamp(v->x + dx * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE);
    v->y = clamp(v->y + dy * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE);
}

static void draw_screen_triangle(screen_vertex *s0, screen_vertex *s1, screen_vertex *s2)
{
    awesome_draw(&protocol_next, AWESOME_DRAW_TRIANGLES, 1, zbuffer_enabled, pattern_enabled);
    awesome_vertex(&protocol_next,
        s0->x / SCREEN_VERTEX_V2_SCALE,
	DISPLAY_HEIGHT - 1 - s0->y / SCREEN_VERTEX_V2_SCALE, s0->z,
        s0->r, s0->g, s0->b);
    awesome_vertex(&protocol_next,
        s1->x / SCREEN_VERTEX_V2_SCALE,
	DISPLAY_HEIGHT - 1 - s1->y / SCREEN_VERTEX_V2_SCALE, s1->z,
        s1->r, s1->g, s1->b);
    awesome_vertex(&protocol_next,
        s2->x / SCREEN_VERTEX_V2_SCALE,
	DISPLAY_HEIGHT - 1 - s2->y / SCREEN_VERTEX_V2_SCALE, s2->z,
        s2->r, s2->g, s2->b);
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
        case DRAW_LINE_STRIP:
            for(int i = 0; i < count - 1; i++) { 
                draw_line_as_triangles(&screenverts[i + 0], &screenverts[i + 1]);
            }
            break;
        case DRAW_LINE_LOOP:
            for(int i = 0; i < count - 1; i++) { 
                draw_line_as_triangles(&screenverts[i + 0], &screenverts[i + 1]);
            }
            draw_line_as_triangles(&screenverts[count - 1], &screenverts[0]);
            break;
        case DRAW_TRIANGLE_STRIP:
            for(int i = 0; i < count - 2; i++) {
                if(i % 2 == 0)
                    draw_screen_triangle(&screenverts[i], &screenverts[i + 1], &screenverts[i + 2]);
                else
                    draw_screen_triangle(&screenverts[i + 1], &screenverts[i], &screenverts[i + 2]);
            }
            break;
        case DRAW_TRIANGLES:
            for(int i = 0; i < count / 3; i++)
                draw_screen_triangle(&screenverts[i * 3], &screenverts[i * 3 + 1], &screenverts[i * 3 + 2]);
            break;
        case DRAW_TRIANGLE_FAN:
            for(int i = 0; i < count - 2; i++) {
                draw_screen_triangle(&screenverts[0], &screenverts[i + 1], &screenverts[i + 2]);
            }
            break;
    }
}

