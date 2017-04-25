#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include <gl.h>
#include "rasterizer.h"
#include "connection.h"

static const int32_t DISPLAY_WIDTH = XMAXSCREEN + 1;
static const int32_t DISPLAY_HEIGHT = YMAXSCREEN + 1;

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

static uint8_t *fpga_manager_base;
static uint8_t *gpu_buffers_base;
static volatile uint32_t *fpga_gpo;
static volatile uint32_t *fpga_gpi;
static volatile uint64_t *gpu_protocol_buffer;
static volatile uint64_t *gpu_protocol_next;

static void cmd_clear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ = CMD_CLEAR
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

static void cmd_draw(volatile uint64_t **p, int type, int count)
{
    *(*p)++ = CMD_DRAW
    	| ((uint64_t) type << 8)
	| ((uint64_t) count << 16);
}

static void vertex(volatile uint64_t **p, int x, int y, int z,
    uint8_t red, uint8_t green, uint8_t blue)
{
    *(*p)++ =
    	  ((uint64_t) x << 2)
	| ((uint64_t) y << 15)
	| ((uint64_t) red << 56)
	| ((uint64_t) green << 48)
	| ((uint64_t) blue << 40);
}

static void cmd_swap(volatile uint64_t **p)
{
    *(*p)++ = CMD_SWAP;
}

static void cmd_end(volatile uint64_t **p)
{
    *(*p)++ = CMD_END;
}

static void gpu_frame_start()
{
    *fpga_gpo = 0;
    gpu_protocol_next = gpu_protocol_buffer;
}

static float the_linewidth;
static uint16_t the_pattern[16]; // XXX
static int pattern_enabled = 0;
static int zbuffer_enabled;


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
    cmd_clear(&gpu_protocol_next, r, g, b);
    // HW command
}

void rasterizer_linewidth(float w)
{
    the_linewidth = w;
}

void rasterizer_setpattern(uint16_t pattern[16])
{
    // XXX
    // HW command
    for (int i = 0; i < 16; i++) {
        the_pattern[i] = pattern[i];
    }
}

void rasterizer_pattern(int enable)
{
    // XXX
    // HW command
    pattern_enabled = enable;
}

void rasterizer_swap()
{
    cmd_swap(&gpu_protocol_next);
    cmd_end(&gpu_protocol_next);

#ifdef DEBUG_PRINT
    printf("    Wrote %d words:\n", gpu_protocol_next - gpu_protocol_buffer);
    for (volatile uint64_t *t = gpu_protocol_buffer; t < gpu_protocol_next; t++) {
        printf("        0x%016llX\n", *t);
    }
#endif

    // Tell FPGA that our data is ready.
#ifdef DEBUG_PRINT
    printf("Telling FPGA that the data is ready.\n");
#endif
    *fpga_gpo |= H2F_DATA_READY;

    // Wait until we find out that it has heard us.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to get busy.\n");
#endif
    while ((*fpga_gpi & F2H_BUSY) == 0) {
        // Busy loop.
    }

    // Let FPGA know that we know that it's busy.
#ifdef DEBUG_PRINT
    printf("Telling FPGA that we know it's busy.\n");
#endif
    *fpga_gpo &= ~H2F_DATA_READY;

    // Wait until it's done rasterizing.
#ifdef DEBUG_PRINT
    printf("Waiting for FPGA to finish rasterizing.\n");
#endif
    while ((*fpga_gpi & F2H_BUSY) != 0) {
        // Busy loop.
    }
}

int32_t rasterizer_winopen(char *title)
{
    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Get access to lightweight FPGA communication.
    fpga_manager_base = (uint8_t *) mmap(0, 64,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, FPGA_MANAGER_BASE);
    if(fpga_manager_base == 0) {
        perror("mmap for gpu manager base");
        exit(EXIT_FAILURE);
    }
    fpga_gpo = (uint32_t*)(fpga_manager_base + FPGA_GPO_OFFSET);
    fpga_gpi = (uint32_t*)(fpga_manager_base + FPGA_GPI_OFFSET);

    // Get access to the various RAM buffers.
    gpu_buffers_base = (uint8_t *) mmap(0, RAM_SIZE - BASE,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, BASE);
    if(gpu_buffers_base == 0) {
        perror("mmap for buffers");
        exit(EXIT_FAILURE);
    }
    gpu_protocol_next = gpu_protocol_buffer =
	(uint64_t *) (gpu_buffers_base + PROTOCOL_BUFFER_OFFSET);

    gpu_frame_start();
}

void rasterizer_zbuffer(int enable)
{
    // Store now, pass in with DRAW command and primitives
    zbuffer_enabled = enable;
}

void rasterizer_zclear(uint32_t z)
{
    // XXX
}

void screen_vertex_offset_with_clamp(screen_vertex* v, float dx, float dy)
{
    v->x = clamp(v->x + dx * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE);
    v->y = clamp(v->y + dy * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE);
}

static void draw_screen_triangle(screen_vertex *s0, screen_vertex *s1, screen_vertex *s2)
{
    cmd_draw(&gpu_protocol_next, DRAW_TRIANGLES, 1);
    vertex(&gpu_protocol_next,
        s0->x / SCREEN_VERTEX_V2_SCALE, s0->y / SCREEN_VERTEX_V2_SCALE, s0->z,
        s0->r, s0->g, s0->b);
    vertex(&gpu_protocol_next,
        s1->x / SCREEN_VERTEX_V2_SCALE, s1->y / SCREEN_VERTEX_V2_SCALE, s1->z,
        s1->r, s1->g, s1->b);
    vertex(&gpu_protocol_next,
        s2->x / SCREEN_VERTEX_V2_SCALE, s2->y / SCREEN_VERTEX_V2_SCALE, s2->z,
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
// XXX network_rasterizer 
// Fake this with screen quads until rasterizer implements lines
    float dx = (v1->x - v0->x);
    float dy = (v1->y - v0->y);
    float d = sqrt(dx * dx + dy * dy);

    if(d == 0.0) {
        // XXX should draw point if the line is too short
        return;
    }

    dx = dx / d;
    dy = dy / d;

    float anglescale;
    if(fabs(dx) < 0.0001) {
        anglescale = 1.0 / fabs(dy);
    } else if(fabs(dy) < 0.0001) {
        anglescale = 1.0 / fabs(dx);
    } else {
        anglescale = min(1.0 / fabs(dx), 1.0 / fabs(dy));
    }

    dx = dx * anglescale;
    dy = dy * anglescale;

    screen_vertex q[4];
    q[0] = *v0;
    q[1] = *v0;
    q[2] = *v1;
    q[3] = *v1;

    screen_vertex_offset_with_clamp(&q[0],  dy * the_linewidth * .5, -dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[1], -dy * the_linewidth * .5,  dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[2], -dy * the_linewidth * .5,  dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[3],  dy * the_linewidth * .5, -dx * the_linewidth * .5);

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

