#include <stdio.h>
#include <gl.h>
#include <math.h>
#include "rasterizer.h"
#include "connection.h"

static const int32_t DISPLAY_WIDTH = XMAXSCREEN + 1;
static const int32_t DISPLAY_HEIGHT = YMAXSCREEN + 1;

static int save_file_frame_number = 0;
static int save_file_last_frame = 1 << 30;
static char *save_filename = NULL;
static FILE *save_file = NULL;

static float the_linewidth;

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
    send_and_capture_uint8(COMMAND_CLEAR);
    send_and_capture_uint8(r);
    send_and_capture_uint8(g);
    send_and_capture_uint8(b);
    if(save_file) {
        fprintf(save_file, "clear %u %u %u\n", r, g, b);
    }
}

void rasterizer_linewidth(float w)
{
    the_linewidth = w;
    if(save_file)
        fprintf(save_file, "linewidth %f\n", w);
}

void rasterizer_setpattern(uint16_t pattern[16])
{
    send_and_capture_uint8(COMMAND_SET_PATTERN);
    for (int i = 0; i < 16; i++)
        send_and_capture_uint16(pattern[i]);
    if(save_file) {
        fprintf(save_file, "setpattern");
        for (int i = 0; i < 16; i++)
            fprintf(save_file, " %04X", pattern[i]);
        fprintf(save_file, "\n");
    }
}

void rasterizer_pattern(int enable)
{
    if(enable)
        send_and_capture_uint8(COMMAND_ENABLE_PATTERN);
    else
        send_and_capture_uint8(COMMAND_DISABLE_PATTERN);
    if(save_file) {
        fprintf(save_file, "pattern %d\n", enable);
    }
}

void rasterizer_swap()
{
    if(save_file) {
        fprintf(save_file, "swap\n");
        if(save_file_frame_number++ == save_file_last_frame) {
            fflush(save_file);
            exit(EXIT_SUCCESS);
        }
        fprintf(save_file, "# frame %d\n", save_file_frame_number);
    }
    send_and_capture_uint8(COMMAND_SWAPBUFFERS);

    flush();

    // XXX force a round trip to fpgasim to throttle
    send_uint8(COMMAND_GET_VALUATOR);
    send_uint32(266); // MOUSEX
    uint32_t val = receive_uint32();
}

int32_t rasterizer_winopen(char *title)
{
    save_filename = getenv("SAVE_DRAWING_FILENAME");
    if(save_filename != NULL) {
        save_file = fopen(save_filename, "w");
        if(save_file == NULL) {
            fprintf(stderr, "Couldn't open \"%s\" for writing.\n", save_filename);
            exit(EXIT_FAILURE);
        } else {
            printf("Saving drawing commands to \"%s\".\n", save_filename);
            fprintf(save_file, "winopen %s\n", title);
            fprintf(save_file, "# frame 0\n");
        }
    }

    char *s = getenv("SAVE_DRAWING_LAST_FRAME");
    if(s != NULL) {
        save_file_last_frame = atoi(s);
        printf("Saving only %d frames\n", save_file_last_frame);
    }

    return network_winopen(title);
}

void rasterizer_zbuffer(int enable)
{
    send_and_capture_uint8(COMMAND_ZBUFFER);
    send_and_capture_uint8(enable);
    if(save_file) {
        fprintf(save_file, "zbuffer %d\n", enable);
    }
}

void rasterizer_zclear(uint32_t z)
{
    send_and_capture_uint8(COMMAND_ZCLEAR);
    if(save_file) {
        fprintf(save_file, "zclear %u\n", z);
    }
}

static void send_screen_vertex(screen_vertex *sv)
{
    send_and_capture_uint16(sv->x / SCREEN_VERTEX_V2_SCALE);
    send_and_capture_uint16(sv->y / SCREEN_VERTEX_V2_SCALE);
    send_and_capture_uint32(sv->z & 0xFFFF0000);
    send_and_capture_uint8(sv->r);
    send_and_capture_uint8(sv->g);
    send_and_capture_uint8(sv->b);
    send_and_capture_uint8(sv->a);
}

void screen_vertex_offset_with_clamp(screen_vertex* v, float dx, float dy)
{
    v->x = clamp(v->x + dx * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE);
    v->y = clamp(v->y + dy * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE);
}

static void send_screen_triangle(screen_vertex *s0, screen_vertex *s1, screen_vertex *s2)
{
    send_and_capture_uint8(COMMAND_TRIANGLE);

    send_screen_vertex(s0);
    send_screen_vertex(s1);
    send_screen_vertex(s2);
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
                send_screen_triangle(&s[0], &s[1], &s[2]);
                send_screen_triangle(&s[2], &s[3], &s[0]);
            }

            prevbit = bit;
        }

        if(prevbit) {
            // the end of the row was a 1 bit, so finish run
            screen_vertex_offset_with_clamp(&s[2], count, 1);
            screen_vertex_offset_with_clamp(&s[3], count, 0);
// XXX rasterizer_draw()
            send_screen_triangle(&s[0], &s[1], &s[2]);
            send_screen_triangle(&s[2], &s[3], &s[0]);
        }
    }
    if(save_file) {
        fprintf(save_file, "bitmap\n");
        fprintf(save_file, " %u %u %u\n", width, rowbytes, height);
        fprintf(save_file, " %u %u %u %u %u %u\n", sv->x, sv->y, sv->z, sv->r, sv->g, sv->b);
        for(int j = 0; j < height; j++)
            for(int i = 0; i < rowbytes; i++)
                fprintf(save_file, " %20X", bits[j * rowbytes + i]);
        fprintf(save_file, "\n");
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

    send_screen_triangle(&q[0], &q[1], &q[2]);
    send_screen_triangle(&q[2], &q[3], &q[0]);
}


void rasterizer_draw(uint32_t type, uint32_t count, screen_vertex *screenverts)
{
    switch(type) {
        default:
            fprintf(stderr, "Unimplemented draw type %d.\n", type);
            return;
            break;

        case DRAW_POINTS:
            fprintf(stderr, "POINTS not implemented\n");
            return;
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
                    send_screen_triangle(&screenverts[i], &screenverts[i + 1], &screenverts[i + 2]);
                else
                    send_screen_triangle(&screenverts[i + 1], &screenverts[i], &screenverts[i + 2]);
            }
            break;

        case DRAW_TRIANGLES:
            for(int i = 0; i < count / 3; i++)
                send_screen_triangle(&screenverts[i * 3], &screenverts[i * 3 + 1], &screenverts[i * 3 + 2]);
            break;

        case DRAW_TRIANGLE_FAN:
            for(int i = 0; i < count - 2; i++) {
                send_screen_triangle(&screenverts[0], &screenverts[i + 1], &screenverts[i + 2]);
            }
            break;
    }

    if(save_file) {
        fprintf(save_file, "draw %d %d\n", type, count);
        for(int i = 0; i < count; i++)
            fprintf(save_file, " %u %u %u %u %u %u\n", screenverts[i].x, screenverts[i].y, screenverts[i].z, screenverts[i].r, screenverts[i].g, screenverts[i].b);
    }
}

