// make all implemented functions have trace
// make all tracefunctions look like C-ish
// All i functions call f functions
// RECORD macro checks display list mode, make, store object...?
// fix all int usage
// make trace() function
// consistent function start
// Need display-listing:
//      reshapeviewport?
//      draw
//      swapbuffers?
//      pclos
//      mov
//      pmv
//      pmv2i
//      pmvi
//      pdr
//      pdr2i
//      pdri
//      linewidth
//      move
//      movei
//      rdr2i
//      rmv2i
//      rectf
//      recti
//      poly
//      zbuffer
//      zclear


// Alice 4 libgl implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gl.h>
#include <device.h>
#include "connection.h"
#include "basic_types.h"
#include "vector.h"
#include "driver.h"

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;
#define POLY_MAX 32

static int trace_functions = 0;
static int trace_network = 0;
static int indent = 0;

static vec3ub colormap[4096];
static vec4f current_color = {1.0f, 1.0f, 1.0f, 1.0f};
static uint16_t current_color_index = 0;
static uint16_t current_writemask = 0xffff;
static vec3f current_normal = {1.0f, 1.0f, 1.0f};
static vec4f current_position = {0.0f, 0.0f, 0.0f, 1.0};
static vec4f current_character_position = {0.0f, 0.0f, 0.0f, 1.0f};
static int current_font = 0;


#define INPUT_QUEUE_SIZE 128
static uint32_t input_queue_device[INPUT_QUEUE_SIZE];
static short input_queue_val[INPUT_QUEUE_SIZE];
// The next time that needs to be read:
static int input_queue_head = 0;
// The number of items in the queue (tail = (head + length) % len):
static int input_queue_length = 0;

//----------------------------------------------------------------------------
// GL state

#define MAX_MATERIALS 16
#define MAX_LIGHTS 16
#define MAX_LMODELS 16

typedef struct material {
    vec3f emission;
    vec3f ambient;
    vec3f specular;
    vec3f diffuse;
    float shininess;
    float alpha;
} material;

typedef struct light {
    vec4f position;
    vec3f ambient;
    vec3f color;
    vec3f spotdirection;
    float spotlight[2];
} light;

typedef struct lmodel {
    float local;
    vec3f ambient;
    float attenuation[2];
} lmodel;

void material_init(material *m)
{
    // Do these reasonable defaults like OpenGL?
    vec3f_set(m->ambient, 0.2, 0.2, 0.2);
    vec3f_set(m->specular, 0.0, 0.0, 0.0);
    vec3f_set(m->emission, 0.0, 0.0, 0.0);
    vec3f_set(m->diffuse, 0.8, 0.8, 0.8);
    m->shininess = 0.0;
    m->alpha = 1.0;
}

void light_init(light *l)
{
    // Are these reasonable defaults like OpenGL or just 0's?
    vec4f_set(l->position, 0.0, 0.0, 1.0, 0.0);
    vec3f_set(l->ambient, 0.0, 0.0, 0.0);
    vec3f_set(l->color, 1.0, 1.0, 1.0);
    vec3f_set(l->spotdirection, 0.0, 0.0, -1.0);
    l->spotlight[0] = 0;
    l->spotlight[1] = 180.0;
}

void lmodel_init(lmodel *l)
{
    vec3f_set(l->ambient, 0.2, 0.2, 0.2);
    l->local = 0.0;
    l->attenuation[0] = 1.0;
    l->attenuation[1] = 0.0;
}

static material materials[MAX_MATERIALS];
static light lights[MAX_LIGHTS];
static lmodel lmodels[MAX_LMODELS];

static short the_linewidth = 1;
static int lighting_enabled = 0;
static int normalize_enabled = 1;
static material *material_bound = NULL;
static light *lights_bound[MAX_LIGHTS];
static lmodel *lmodel_bound = NULL;

long matrix_mode = MSINGLE;
matrix4x4f_stack modelview_stack;
matrix4x4f_stack projection_stack;
matrix4x4f_stack *current_stack;

Screencoord the_viewport[6];

float unitclamp(float v)
{
    return v > 1.0f ? 1.0f : (v < 0.0f ? 0.0f : v);
}

float clamp(float v, float low, float high)
{
    return v > high ? high : (v < low ? low : v);
}

float min(float a, float b)
{
    return (a < b) ? a : b;
}


//----------------------------------------------------------------------------
// I/O operations

// Send a string as a length and the bytes. Max string length is 255.
void send_string(char *s) {
    send_uint8(strlen(s));
    while (*s != '\0') {
        send_uint8(*s);
        s++;
    }
}

// Little-endian.
void send_uint16(unsigned short x) {
    if (trace_network) {
        printf("Sending short %d\n", x);
    }
    send_uint8(x & 0xFF);
    send_uint8(x >> 8);
}

// Little-endian.
void send_uint32(uint32_t x) {
    if (trace_network) {
        printf("Sending long %u\n", x);
    }
    send_uint8(x & 0xFF);
    send_uint8((x >> 8) & 0xFF);
    send_uint8((x >> 16) & 0xFF);
    send_uint8((x >> 24) & 0xFF);
}

// Little-endian.
unsigned short receive_uint16() {
    unsigned char low = receive_uint8();
    unsigned char high = receive_uint8();

    return high*256 + low;
}

// Little-endian.
uint32_t receive_uint32() {
    long value = 0;

    value |= receive_uint8() << 0;
    value |= receive_uint8() << 8;
    value |= receive_uint8() << 16;
    value |= receive_uint8() << 24;

    return value;
}

//----------------------------------------------------------------------------
// Transformation, lighting, clipping

typedef struct world_vertex
{
    vec4f coord;
    vec3f normal;
    vec4f color;
} world_vertex;

typedef struct lit_vertex
{
    vec4f coord;
    vec4f color;
} lit_vertex;

void light_vertex(material *mtl, vec4f coord, vec3f normal, vec4f color_)
{
    vec3f color;
    vec3f_mult(mtl->ambient, lmodel_bound->ambient, color);

    for(int i = 0; i < MAX_LIGHTS; i++) {
        light *l = lights_bound[i];
        if(l == NULL)
            continue;

        // XXX point lights only, no ambient, no emission
        // XXX OpenGL ES 1.1: 2.12.1 Lighting
        vec4f vertex_to_light;
        if(l->position[3] == 0.0)  {
            vec4f_copy(vertex_to_light, l->position);
        }
        else
            vec4f_subtract(l->position, coord, vertex_to_light);
        vec4f_normalize(vertex_to_light, vertex_to_light);
        float diff_part = vec3f_dot(normal, vertex_to_light);
        if(diff_part >= 0)
        {
            /* diffuse calculation */
            vec3f t1;
            vec3f_mult(mtl->diffuse, l->color, t1);
            vec3f_scale(t1, diff_part, t1);
            vec3f_add(t1, color, color);
            vec4f h;
            h[0] = vertex_to_light[0];
            h[1] = vertex_to_light[1];
            h[2] = vertex_to_light[2] + 1;
            h[3] = 0;
            vec4f_normalize(h, h);
            float n_dot_h = vec3f_dot(normal, h);
            if(n_dot_h > 0.0) {
                float spec_part = powf(n_dot_h, mtl->shininess);
                vec3f_mult(mtl->specular, l->color, t1);
                vec3f_scale(t1, spec_part, t1);
                vec3f_add(t1, color, color);
            }
        }
    }
    vec4f_set(color_, color[0], color[1], color[2], mtl->alpha);
}

void transform_and_light_vertex(world_vertex *wv, lit_vertex *lv)
{
    vec4f tv;
    vec3f normal;

    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&modelview_stack), wv->coord, tv);
    // matrix4x4f_print("modelview", matrix4x4f_stack_top(&modelview_stack));
    if(lighting_enabled) {
        vec3f_mult_matrix4x4f(wv->normal, matrix4x4f_stack_get_inverse(&modelview_stack), normal);

        if(normalize_enabled)
            vec3f_normalize(normal, normal);

#if 0
        if(color_material_enabled) {
            // XXX color_material mode
            vec4f_copy(current_material.diffuse, lv->color);
            vec4f_copy(current_material.ambient, lv->color);
        }
#endif
    
        light_vertex(material_bound, tv, normal, lv->color);
    } else {
        vec4f_copy(lv->color, wv->color);
    }

    /// XXX could multiply mv and p together?
    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&projection_stack), tv, lv->coord);
    // matrix4x4f_print("projection", matrix4x4f_stack_top(&projection_stack));
    // vec4f_print("Object", wv->coord);
    // vec4f_print("World", tv);
    // vec4f_print("Lit", lv->coord);
}

void screen_vertex_set_color(screen_vertex *sv, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    sv->r = r;
    sv->g = g;
    sv->b = b;
    sv->a = a;
}

void project_vertex(lit_vertex *lv, screen_vertex *sv)
{
    // XXX could pre-compute
    int viewport_width = the_viewport[1] - the_viewport[0] + 1;
    int viewport_height = the_viewport[3] - the_viewport[2] + 1;

    float xndc, yndc, zndc;
    xndc = lv->coord[0] / lv->coord[3];
    yndc = lv->coord[1] / lv->coord[3];
    zndc = lv->coord[2] / lv->coord[3];
    // printf("ndc: %g %g %g\n", xndc, yndc, zndc);

    float xw, yw, zw;
    // XXX could pre-compute half width and height
    xw = viewport_width / 2.0 * xndc + (the_viewport[0] + viewport_width / 2.0);
    yw = viewport_height / 2.0 * yndc + (the_viewport[2] + viewport_height / 2.0);
    zw = (the_viewport[5] - the_viewport[4]) / 2.0 * zndc + (the_viewport[5] + the_viewport[4]) / 2.0;
    // printf("Viewport: %g %g\n", xw, yw);

    sv->x = clamp(xw, 0, DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE;
    sv->y = clamp(yw, 0, DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE;
    sv->z = zw * 0xffffffff;
    sv->r = unitclamp(lv->color[0]) * 255;
    sv->g = unitclamp(lv->color[1]) * 255;
    sv->b = unitclamp(lv->color[2]) * 255;
    sv->a = unitclamp(lv->color[3]) * 255;
}

void send_screen_vertex(screen_vertex *sv) {
    send_uint16(sv->x / SCREEN_VERTEX_V2_SCALE);
    send_uint16(sv->y / SCREEN_VERTEX_V2_SCALE);
    send_uint32(sv->z);
    send_uint8(sv->r);
    send_uint8(sv->g);
    send_uint8(sv->b);
    send_uint8(sv->a);
}

void process_triangle(screen_vertex *s0, screen_vertex *s1, screen_vertex *s2)
{
    send_uint8(COMMAND_TRIANGLE);

    send_screen_vertex(s0);
    send_screen_vertex(s1);
    send_screen_vertex(s2);
}

enum {
    CLIP_ALL_IN = 0x00,
    CLIP_NEG_X = 0x01,
    CLIP_POS_X = 0x02,
    CLIP_NEG_Y = 0x04,
    CLIP_POS_Y = 0x08,
    CLIP_NEG_Z = 0x10,
    CLIP_POS_Z = 0x20,
};

#define CLIP_EPSILON .001

void plane_to_clip_params(int plane, int *sign, int *index)
{
    switch(plane) {
        case CLIP_NEG_X: *sign = -1; *index = 0; break;
        case CLIP_POS_X: *sign = 1; *index = 0; break;
        case CLIP_NEG_Y: *sign = -1; *index = 1; break;
        case CLIP_POS_Y: *sign = 1; *index = 1; break;
        case CLIP_NEG_Z: *sign = -1; *index = 2; break;
        case CLIP_POS_Z: *sign = 1; *index = 2; break;
    }
}

long clip_line_against_plane(long plane, lit_vertex *input, lit_vertex *output)
{
    int sign;
    int index;

    plane_to_clip_params(plane, &sign, &index);

    float p0 = input[0].coord[index] * sign;
    float p1 = input[1].coord[index] * sign;
    float w0 = input[0].coord[3];
    float w1 = input[1].coord[3];

    if(p0 > w0 && p1 > w1)
        return 0;

    int n = 0;

    if(p0 < w0) {
        output[n++] = input[0];
    }

    if((p0 < w0 && p1 >= w1) || (p0 >= w0 && p1 < w1)) {
        float denom = -p0 + p1 + w0 - w1;
        if(fabs(denom) > CLIP_EPSILON) {
            float t = (-p0 + w0) / denom;
            vec4f_blend(input[0].coord, input[1].coord, t, output[n].coord);
            // Should other attributes be hyperbolically interpolated?
            vec4f_blend(input[0].color, input[1].color, t, output[n].color);
            n++;
        }
    }

    if(p1 < w1) {
        output[n++] = input[1];
    }

    return 1;
}

long clip_polygon_against_plane(long plane, long n, lit_vertex *input, lit_vertex *output)
{
    int sign;
    int index;

    plane_to_clip_params(plane, &sign, &index);

    int n2 = 0;
    lit_vertex* v0;
    lit_vertex* v1 = &input[n - 1];

    for(int i = 0; i < n; i++) {
        v0 = v1;
        v1 = &input[i];

        float p0 = v0->coord[index] * sign;
        float p1 = v1->coord[index] * sign;
        float w0 = v0->coord[3];
        float w1 = v1->coord[3];

        if(p0 < w0) {
            output[n2++] = *v0;
        }

        if((p0 < w0 && p1 >= w1) || (p0 >= w0 && p1 < w1)) {
            float denom = -p0 + p1 + w0 - w1;
            if(fabs(denom) > CLIP_EPSILON) {
                float t = (-p0 + w0) / denom;
                vec4f_blend(v0->coord, v1->coord, t, output[n2].coord);
                // Should other attributes be hyperbolically interpolated?
                vec4f_blend(v0->color, v1->color, t, output[n2].color);
                n2++;
            }
        }
    }
    return n2;
}

int classify_vertex(float *c)
{
    int code = 0;
    if(c[0] < -c[3]) code |= CLIP_NEG_X;
    if(c[0] > c[3]) code |= CLIP_POS_X;
    if(c[1] < -c[3]) code |= CLIP_NEG_Y;
    if(c[1] > c[3]) code |= CLIP_POS_Y;
    if(c[2] < -c[3]) code |= CLIP_NEG_Z;
    if(c[2] > c[3]) code |= CLIP_POS_Z;
    return code;
}

enum {
    CLIP_TRIVIAL_REJECT = 0,
    CLIP_TRIVIAL_ACCEPT = -1, // If clip_polygon() returns this, it stored nothing in "output".
};

long clip_line(lit_vertex *input, lit_vertex *output)
{
    static int code[2];
    static lit_vertex tmp[2];
    int all_neg[3] = {1, 1, 1};
    int all_pos[3] = {1, 1, 1};
    int all_inside = 1;
    int all_outside_one = 0xff;

    for(int i = 0; i < 2; i++) {
        code[i] = classify_vertex(input[i].coord);
        all_inside = all_inside && (code[i] == CLIP_ALL_IN);
        all_outside_one &= code[i];
    }

    if(all_inside)
        return CLIP_TRIVIAL_ACCEPT;

    if(all_outside_one)
        return CLIP_TRIVIAL_REJECT;

    int n;

    n = clip_line_against_plane(CLIP_NEG_X, input, tmp);
    if(n == 0) return 0;
    n = clip_line_against_plane(CLIP_POS_X, tmp, output);
    if(n == 0) return 0;
    n = clip_line_against_plane(CLIP_NEG_Y, output, tmp);
    if(n == 0) return 0;
    n = clip_line_against_plane(CLIP_POS_Y, tmp, output);
    if(n == 0) return 0;
    n = clip_line_against_plane(CLIP_NEG_Z, output, tmp);
    if(n == 0) return 0;
    n = clip_line_against_plane(CLIP_POS_Z, tmp, output);

    return n;
}

long clip_polygon(long n, lit_vertex *input, lit_vertex *output)
{
    static int code[POLY_MAX];
    static lit_vertex tmp[POLY_MAX];
    int all_neg[3] = {1, 1, 1};
    int all_pos[3] = {1, 1, 1};
    int all_inside = 1;
    int all_outside_one = 0xff;

    for(int i = 0; i < n; i++) {
        code[i] = classify_vertex(input[i].coord);
        all_inside = all_inside && (code[i] == CLIP_ALL_IN);
        all_outside_one &= code[i];
    }

    if(all_inside)
        return CLIP_TRIVIAL_ACCEPT;

    if(all_outside_one)
        return CLIP_TRIVIAL_REJECT;

    n = clip_polygon_against_plane(CLIP_NEG_X, n, input, tmp);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_POS_X, n, tmp, output);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_NEG_Y, n, output, tmp);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_POS_Y, n, tmp, output);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_NEG_Z, n, output, tmp);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_POS_Z, n, tmp, output);

    return n;
}

void screen_vertex_offset_with_clamp(screen_vertex* v, float dx, float dy)
{
    v->x = clamp(v->x + dx * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_WIDTH - 1) * SCREEN_VERTEX_V2_SCALE);
    v->y = clamp(v->y + dy * SCREEN_VERTEX_V2_SCALE, 0, (DISPLAY_HEIGHT - 1) * SCREEN_VERTEX_V2_SCALE);
}

void process_line(world_vertex *wv0, world_vertex *wv1)
{
    static lit_vertex litverts[2], *vp;
    static lit_vertex clipped[2];
    static screen_vertex screenverts[2];

    transform_and_light_vertex(wv0, &litverts[0]);
    transform_and_light_vertex(wv1, &litverts[1]);

    long r = clip_line(litverts, clipped);
    if(r == CLIP_TRIVIAL_REJECT)
        return;
    else if(r == CLIP_TRIVIAL_ACCEPT) {
        vp = litverts;
    } else {
        vp = clipped;
    }

    project_vertex(&vp[0], &screenverts[0]);
    project_vertex(&vp[1], &screenverts[1]);

// Fake this with screen quads until rasterizer implements lines
    float dx = (screenverts[1].x - screenverts[0].x);
    float dy = (screenverts[1].y - screenverts[0].y);
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
    q[0] = screenverts[0];
    q[1] = screenverts[0];
    q[2] = screenverts[1];
    q[3] = screenverts[1];

    screen_vertex_offset_with_clamp(&q[0],  dy * the_linewidth * .5, -dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[1], -dy * the_linewidth * .5,  dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[2], -dy * the_linewidth * .5,  dx * the_linewidth * .5);
    screen_vertex_offset_with_clamp(&q[3],  dy * the_linewidth * .5, -dx * the_linewidth * .5);

    process_triangle(&q[0], &q[1], &q[2]);
    process_triangle(&q[2], &q[3], &q[0]);
}

void process_polygon(long n, world_vertex *worldverts)
{
    static lit_vertex litverts[POLY_MAX];
    static lit_vertex clipped[POLY_MAX];
    static lit_vertex *vp;
    static screen_vertex screenverts[POLY_MAX];

    for(int i = 0; i < n; i++)
        transform_and_light_vertex(&worldverts[i], &litverts[i]);

    long r = clip_polygon(n, litverts, clipped);
    if(r == CLIP_TRIVIAL_REJECT)
        return;
    else if(r == CLIP_TRIVIAL_ACCEPT) {
        vp = litverts;
    } else {
        vp = clipped;
        n = r;
    }

    for(int i = 0; i < n; i++)
        project_vertex(&vp[i], &screenverts[i]);

    int i0, i1, i2;

    i1 = 0;
    i2 = n - 1;

    for(int i = 0; i < n - 2; i++) {
        i0 = i1;
        i1 = i2;
        // This next one means 3rd vertex alternates back and forth
        // across polygon, basically turning it into a triangle strip
        // A fan might be slightly clearer
        i2 = (i % 2 == 0) ? (1 + i / 2) : (n - 2 - i / 2);

        process_triangle(&screenverts[i0], &screenverts[i1], &screenverts[i2]);
    }
}

void draw_bitmap(screen_vertex *sv, uint8_t *bgcolor, uint8_t *bits, uint32_t width, uint32_t rowbytes, uint32_t height)
{
    // Should combine adjacent pixels into long runs of quads
    for(int j = 0; j < height; j++) {
        for(int i = 0; i < width; i++) {
            int bit = (bits[j * rowbytes + i / 8] >> (7 - i % 8)) & 1;

            if(bit || (bgcolor != NULL)) {
                screen_vertex pixel;
                pixel = *sv;
                pixel.x += SCREEN_VERTEX_V2_SCALE * i;
                pixel.y = pixel.y + (height - j - 1) * SCREEN_VERTEX_V2_SCALE;

                screen_vertex s[4];
                for(int k = 0; k < 4; k++) {
                    s[k] = pixel;
                    if(!bit && (bgcolor != NULL))
                        screen_vertex_set_color(&s[k], bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);
                }

                // depending on rasterizer, might get away here with a
                // triangle on top of the center of the pixel
                screen_vertex_offset_with_clamp(&s[0], 0, 0);
                screen_vertex_offset_with_clamp(&s[1], 0, 1);
                screen_vertex_offset_with_clamp(&s[2], 1, 1);
                screen_vertex_offset_with_clamp(&s[3], 1, 0);
                process_triangle(&s[0], &s[1], &s[2]);
                process_triangle(&s[2], &s[3], &s[0]);
            }
        }
    }
}


//----------------------------------------------------------------------------
// Display list ("Object" and "Tag") management

/*
implementing a display-listable function:
    add to enum in struct element
    if parameters, add new struct to union
    if needed, add free()/dtor in element_free()
    in callobj, add case entry with actual call
    in function, add preamble like one in rotate()
*/

typedef struct element
{
    enum object_type
    {
        TAG,
        CALLOBJ,
        COLOR,
        POLF,
        POLF2I,
        CLEAR,
        CIRCI,
        PUSHMATRIX,
        POPMATRIX,
        ROTATE,
        TRANSLATE,
        SCALE,
        MULTMATRIX,
        LOOKAT,
        LOADMATRIX,
        PERSPECTIVE,
        WINDOW,
        ORTHO2,
        VIEWPORT,
        RGBCOLOR,
        BGNPOLYGON,
        ENDPOLYGON,
        C3F,
        N3F,
        V3F,
        MMODE,
        LMBIND,
        MOVE2I,
        DRAW2I,
        CMOV2I,
        CHARSTR,
        WRITEMASK,
    } type;

    union
    {
        struct {
            Coord left, right, bottom, top;
        } ortho2;

        struct {
            char *str;
        } charstr;

        struct {
            Icoord x, y;
        } draw2i;

        struct {
            Icoord x, y;
        } move2i;

        struct {
            Icoord x, y;
        } cmov2i;

        struct {
            Icoord x, y, r;
        } circi;

        struct {
            short target;
            long index;
        } lmbind;

        struct {
            long mode;
        } mmode;

        struct {
            float v[3];
        } v3f;

        struct {
            float n[3];
        } n3f;

        struct {
            float c[3];
        } c3f;

        struct {
            long r, g, b;
        } rgbcolor;

        struct {
            Tag tag;
        } tag;

        struct {
            Object obj;
        } callobj;

        struct {
            Colorindex mask;
        } writemask;

        struct {
            Colorindex color;
        } color;

        struct {
            int n;
            Coord *parray;
        } polf;

        struct {
            int n;
            Icoord *parray;
        } polf2i;

        struct {
            Angle angle;
            unsigned char axis;
        } rotate;

        struct {
            Coord x, y, z;
        } scale;

        struct {
            Coord x, y, z;
        } translate;

        struct {
            Coord viewx;
            Coord viewy;
            Coord viewz;
            Coord pointx;
            Coord pointy;
            Coord pointz;
            Angle twist;
        } lookat;

        struct {
            Matrix m;
        } multmatrix;

        struct {
            Matrix m;
        } loadmatrix;

        struct {
            Angle fovy; 
            float aspect;
            Coord near, far;
        } perspective;

        struct {
            Coord left, right;
            Coord bottom, top;
            Coord near, far;
        } window;

        struct {
            Screencoord left, right;
            Screencoord bottom, top;
        } viewport;
    };

    struct element *next;
} element;

#define OBJ_MAX 1024
unsigned char object_allocation[OBJ_MAX];
element *objects[OBJ_MAX];
unsigned char tag_allocation[OBJ_MAX];
element **ptrs_to_tagptrs[OBJ_MAX];
element **cur_ptr_to_nextptr;
int replace_mode;

element *element_new()
{
    element *e = malloc(sizeof(element));
    return e;
}

void element_insert(element ***ptr_to_nextptr, element *n)
{
    n->next = **ptr_to_nextptr;
    **ptr_to_nextptr = n;
    *ptr_to_nextptr = &n->next;
}

void element_free(element *p)
{
    while(p != NULL) {
        switch(p->type) {
            case TAG:
                tag_allocation[p->tag.tag] = 0;
                ptrs_to_tagptrs[p->tag.tag] = NULL;
                break;
            case POLF:
                free(p->polf.parray);
                break;
            case POLF2I:
                free(p->polf2i.parray);
                break;
            case CHARSTR:
                free(p->charstr.str);
                break;
            default:
                break;
        }
        element *np = p->next;
        free(p);
        p = np;
    }
}

element *element_next_in_object(enum object_type t)
{
    element *e;

    if(!replace_mode) {
        e = element_new();
        element_insert(&cur_ptr_to_nextptr, e);
        // XXX Need to skip tags?
    } else {
        e = *cur_ptr_to_nextptr;
        cur_ptr_to_nextptr = &(*cur_ptr_to_nextptr)->next;
    }
    e->type = t;

    return e;
}


//----------------------------------------------------------------------------
// GL API calls

void callobj(Object obj) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(CALLOBJ);
        e->callobj.obj = obj;
        return;
    }

    if(trace_functions) printf("%*scallobj %ld\n", indent, "", obj);
    indent += 4;
    element *p = objects[obj];
    while(p) {
        switch(p->type) {
            case LMBIND:
                lmbind(p->lmbind.target, p->lmbind.index);
                break;
            case MMODE:
                mmode(p->mmode.mode);
                break;
            case V3F:
                v3f(p->v3f.v);
                break;
            case C3F:
                c3f(p->c3f.c);
                break;
            case N3F:
                n3f(p->n3f.n);
                break;
            case ENDPOLYGON:
                endpolygon();
                break;
            case BGNPOLYGON:
                bgnpolygon();
                break;
            case RGBCOLOR:
                RGBcolor(
                    p->rgbcolor.r,
                    p->rgbcolor.g,
                    p->rgbcolor.b
                );
                break;
            case VIEWPORT:
                viewport(
                    p->viewport.left,
                    p->viewport.right,
                    p->viewport.bottom,
                    p->viewport.top
                );
                break;
            case ORTHO2:
                ortho2(
                    p->ortho2.left,
                    p->ortho2.right,
                    p->ortho2.bottom,
                    p->ortho2.top);
                break;
            case WINDOW:
                window(
                    p->window.left,
                    p->window.right,
                    p->window.bottom,
                    p->window.top,
                    p->window.near,
                    p->window.far
                );
                break;
            case PERSPECTIVE:
                perspective(
                    p->perspective.fovy,
                    p->perspective.aspect,
                    p->perspective.near,
                    p->perspective.far
                );
                break;
            case POPMATRIX:
                popmatrix();
                break;
            case CIRCI:
                circi(p->circi.x, p->circi.y, p->circi.r);
                break;
            case CLEAR:
                clear();
                break;
            case PUSHMATRIX:
                pushmatrix();
                break;
            case LOADMATRIX:
                loadmatrix(p->loadmatrix.m);
                break;
            case LOOKAT:
                lookat(
                    p->lookat.viewx,
                    p->lookat.viewy,
                    p->lookat.viewz,
                    p->lookat.pointx,
                    p->lookat.pointy,
                    p->lookat.pointz,
                    p->lookat.twist
                );
                break;
            case MULTMATRIX:
                multmatrix(p->multmatrix.m);
                break;
            case TRANSLATE:
                translate(p->translate.x, p->translate.y, p->translate.z);
                break;
            case SCALE:
                scale(p->scale.x, p->scale.y, p->scale.z);
                break;
            case ROTATE:
                rotate(p->rotate.angle, p->rotate.axis);
                break;
            case POLF2I:
                polf2i(p->polf2i.n, p->polf2i.parray);
                break;
            case POLF:
                polf(p->polf.n, p->polf.parray);
                break;
            case COLOR:
                color(p->color.color);
                break;
            case CALLOBJ:
                callobj(p->callobj.obj);
                break;
            case CHARSTR:
                charstr(p->charstr.str);
                break;
            case WRITEMASK:
                writemask(p->writemask.mask);
                break;
            case CMOV2I:
                cmov2i(p->cmov2i.x, p->cmov2i.y);
                break;
            case DRAW2I:
                draw2i(p->draw2i.x, p->draw2i.y);
                break;
            case MOVE2I:
                move2i(p->move2i.x, p->move2i.y);
                break;
            case TAG:
                if(trace_functions) printf("%*stag %ld\n", indent, "", p->tag.tag);
                break;
            default:
                printf("encountered unknown Object type, %s:%d\n", __FILE__, __LINE__);
                abort();
        }
        p = p->next;
    }
    indent -= 4;
}

void backface() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void clear() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(CLEAR);
        return;
    }

    if(trace_functions) printf("%*sclear();\n", indent, "");
    send_uint8(COMMAND_CLEAR);
    for(int i = 0; i < 3; i++)
        send_uint8((int)(current_color[i] * 255.0));
}

void closeobj() { 
    if(trace_functions) printf("%*scloseobj\n", indent, "");
    cur_ptr_to_nextptr = NULL;
}

void color(Colorindex color) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(COLOR);
        e->color.color = color;
        return;
    }

    if(trace_functions) printf("%*scolor %u\n", indent, "", color);
    current_color_index = color;
    for(int i = 0; i < 3; i++)
        current_color[i] = colormap[color][i] / 255.0;
    // XXX alpha in color map?
}

void deflinestyle() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void defpattern(long index, short size, Pattern16 mask) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void doublebuffer() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void editobj(Object obj) { 
    if(trace_functions) printf("%*seditobj %ld\n", indent, "", obj);

    cur_ptr_to_nextptr = &(objects[obj]);
    while(*cur_ptr_to_nextptr != NULL)
        cur_ptr_to_nextptr = &(*cur_ptr_to_nextptr)->next;
    replace_mode = 0;
}

void frontbuffer() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void gconfig() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

Object genobj() { 
    for(int i = 0; i < OBJ_MAX; i++)
        if(!object_allocation[i]) {
            object_allocation[i] = 1;
            objects[i] = NULL;
            if(trace_functions) printf("%*sgenobj -> %d\n", indent, "", i);
            return i;
        }
    abort();
}

Tag gentag() { 
    for(int i = 0; i < OBJ_MAX; i++)
        if(!tag_allocation[i]) {
            tag_allocation[i] = 1;
            ptrs_to_tagptrs[i] = NULL;
            if(trace_functions) printf("%*sgentag -> %d\n", indent, "", i);
            return i;
        }
    abort();
}

Boolean getbutton() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
    return 0;
}

void getmcolor(Colorindex index, short *red, short *green, short *blue) { 
    if(trace_functions) printf("%*sgetmcolor(%d);\n", indent, "", index);
    *red = colormap[index][0];
    *green = colormap[index][1];
    *blue = colormap[index][2];
}

void getorigin(long *x, long *y) { 
    if(trace_functions) printf("%*sgetorigin();\n", indent, "");
    *x = 0;
    *y = 0;
}

long getplanes() { 
    if(trace_functions) printf("%*sgetplanes();\n", indent, "");
    return 24;
}

void getsize(long *width, long *height) { 
    if(trace_functions) printf("%*sgetsize();\n", indent, "");
    *width = DISPLAY_WIDTH;
    *height = DISPLAY_HEIGHT;
}

long getvaluator(long device) { 
    if(trace_functions) printf("%*sgetvaluator(%ld);\n", indent, "", device);
    send_uint8(COMMAND_GET_VALUATOR);
    send_uint32(device);
    return receive_uint32();
}

void setvaluator(Device device, short init, short min, short max) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void gflush() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void glcompat(long mode, long value) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void gl_sincos(Angle angle, float *s, float *c) {
    float a = angle / 1800.0 * M_PI;
    *s = sin(a);
    *c = cos(a);
}

void gexit() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void makeobj(Object obj) { 
    if(trace_functions) printf("%*smakeobj %ld\n", indent, "", obj);
    if(objects[obj] != NULL) {
        element_free(objects[obj]);
    }
    objects[obj] = NULL;
    cur_ptr_to_nextptr = &(objects[obj]);
    replace_mode = 0;
}

void maketag(Tag tag) { 
    if(trace_functions) printf("%*smaketag %ld\n", indent, "", tag);
    if(cur_ptr_to_nextptr == NULL) {
        printf("maketag : not editing\n");
        return;
    }
    element *e = element_new();
    e->type = TAG;
    e->tag.tag = tag;
    ptrs_to_tagptrs[tag] = cur_ptr_to_nextptr;
    element_insert(&cur_ptr_to_nextptr, e);
}

void mapcolor(Colorindex index, short red, short green, short blue) { 
    // XXX insect only provides numbers ranging 0..255
    if(trace_functions) printf("%*smapcolor %d %d %d %d\n", indent, "", index, red, green, blue);
    colormap[index][0] = red;
    colormap[index][1] = green;
    colormap[index][2] = blue;
}

void multmatrix(Matrix m) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(MULTMATRIX);
        memcpy(e->multmatrix.m, m, sizeof(Matrix));
        return;
    }

    if(trace_functions) {
        printf("%*smultmatrix\n", indent, "");
        for(int i = 0 ; i < 4; i++)
            printf("%*s    %f %f %f %f\n", indent, "",
                m[i][0], m[i][1], m[i][2], m[i][3]);
    }

    matrix4x4f_stack_mult(current_stack, (float *)m);
}

void objreplace(Tag tag) { 
    if(trace_functions) printf("%*sobjreplace %ld\n", indent, "", tag);
    cur_ptr_to_nextptr = &(*ptrs_to_tagptrs[tag])->next;
    replace_mode = 1;
}

void perspective(Angle fovy_, float aspect, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(PERSPECTIVE);
        e->perspective.fovy = fovy_;
        e->perspective.aspect = aspect;
        e->perspective.near = near;
        e->perspective.far = far;
        return;
    }

    float m[16];
    float fovy = fovy_ / 1800.0 * M_PI;

    float f = 1.0 / tan(fovy / 2.0);

    matrix4x4f_copy(m, identity_4x4f);
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0;
    m[14] = 2 * far * near / (near - far);
    m[15] = 0.0;
    if(trace_functions) printf("%*sperspective %d %f %f %f\n", indent, "", fovy_, aspect, near, far);
    if(matrix_mode == MSINGLE) {
        matrix4x4f_stack_load(&projection_stack, m);
        matrix4x4f_stack_load(&modelview_stack, identity_4x4f);
        matrix4x4f_stack_load(&projection_stack, m);
    } else 
        matrix4x4f_stack_load(current_stack, m);
}

// XXX display list
void ortho2(Coord left, Coord right, Coord bottom, Coord top) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(ORTHO2);
        e->window.left = left;
        e->window.right = right;
        e->window.bottom = bottom;
        e->window.top = top;
        return;
    }

    if(trace_functions) printf("%*sortho2(%f, %f, %f, %f);\n", indent, "", left, right, bottom, top);

    float m[16];

    matrix4x4f_copy(m, identity_4x4f);
    m[0] =  2.0f / (right-left);
    m[5] =  2.0f / (top-bottom);
    m[10] = -1.0f;
    m[12] = -(right+left) / (right-left);
    m[13] = -(top+bottom) / (top-bottom);
    m[15] =  1.0f;

    if(matrix_mode == MSINGLE) {
        matrix4x4f_stack_load(&projection_stack, m);
        matrix4x4f_stack_load(&modelview_stack, identity_4x4f);
        matrix4x4f_stack_load(&projection_stack, m);
    } else 
        matrix4x4f_stack_load(current_stack, m);
}

void polf(long n, Coord parray[ ][3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(POLF);
        e->polf.n = n;
        e->polf.parray = (Coord*) malloc(sizeof(Coord) * 3 * n);
        memcpy(e->polf.parray, parray, sizeof(Coord) * 3 * n);
        return;
    }

    if(trace_functions) printf("%*spolf %ld\n", indent, "", n);

    static world_vertex worldverts[POLY_MAX];
    vec4f color;
    vec4f_copy(color, current_color);

    for(int i = 0 ; i < n; i++) {
        vec4f_set(worldverts[i].coord,
            parray[i][0], parray[i][1], parray[i][2], 1.0);
        vec4f_copy(worldverts[i].color, color);
        vec3f_set(worldverts[i].normal, 1, 0, 0);
    }

    process_polygon(n, worldverts);
}

void polf2i(long n, Icoord parray[ ][2]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(POLF2I);
        e->polf2i.n = n;
        e->polf2i.parray = (Icoord*) malloc(sizeof(Icoord) * 2 * n);
        memcpy(e->polf.parray, parray, sizeof(Icoord) * 2 * n);
        return;
    }

    if(trace_functions) printf("%*spolf2i %ld\n", indent, "", n);

    static world_vertex worldverts[POLY_MAX];
    vec4f color;
    vec4f_copy(color, current_color);

    for(int i = 0 ; i < n; i++) {
        vec4f_set(worldverts[i].coord,
            parray[i][0], parray[i][1], 0, 1.0);
        vec4f_copy(worldverts[i].color, color);
        vec3f_set(worldverts[i].normal, 1, 0, 0);
    }

    process_polygon(n, worldverts);
}

void popmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(POPMATRIX);
        return;
    }

    if(trace_functions) printf("%*spopmatrix\n", indent, "");

    matrix4x4f_stack_pop(current_stack);
}

void pushmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(PUSHMATRIX);
        return;
    }

    if(trace_functions) printf("%*spushmatrix\n", indent, "");

    matrix4x4f_stack_push(current_stack);
}

static void enqueue_device(long device, short val) {
    if (input_queue_length == INPUT_QUEUE_SIZE) {
        printf("Input queue overflow.");
    } else {
        int tail = (input_queue_head + input_queue_length) % INPUT_QUEUE_SIZE;
        input_queue_device[tail] = device;
        input_queue_val[tail] = val;
        input_queue_length++;
    }
}

// We're interested in events from this device.
void qdevice(long device) { 
    switch (device) {
        case REDRAW:
            // Initial redraw.
            enqueue_device(REDRAW, 0);
            break;

        case INPUTCHANGE:
            // Tell app that this window has received input focus
            enqueue_device(INPUTCHANGE, 1);
            break;

        default:
            // Send the device to the server.
            send_uint8(COMMAND_QDEVICE);
            send_uint32(device);
            break;
    }
}

void unqdevice(Device device) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void fetch_event_queue(int blocking) {
    send_uint8(COMMAND_QREAD);
    send_uint8(blocking);

    // First is number of events.
    int count = receive_uint8();
    for (int i = 0; i < count; i++) {
        long device = receive_uint32();
        long value = receive_uint16();
        enqueue_device(device, value);
    }
}

// If the queue is empty, qread() blocks.
long qread(short *val) { 
    while (input_queue_length == 0) {
        // Blocking read.
        fetch_event_queue(TRUE);
    }

    *val = input_queue_val[input_queue_head];
    long device = input_queue_device[input_queue_head];
    input_queue_head = (input_queue_head + 1) % INPUT_QUEUE_SIZE;
    input_queue_length--;
    return device;
}

// Returns the device number of the first entry.
// Returns 0 if the event queue is empty.
// Doesn't change the queue.
long qtest() { 
    if (input_queue_length == 0) {
        // Non-blocking read.
        fetch_event_queue(FALSE);
    }

    if (input_queue_length == 0) {
        // Empty queue.
        return 0;
    } else {
        // Peek at the head.
        return input_queue_device[input_queue_head];
    }
}

void viewport(Screencoord left, Screencoord right, Screencoord bottom, Screencoord top)
{
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(VIEWPORT);
        e->viewport.left = left;
        e->viewport.right = right;
        e->viewport.bottom = bottom;
        e->viewport.top = top;
        return;
    }

    if(trace_functions) printf("%*sviewport %d %d %d %d\n", indent, "", left, right, bottom, top);

    the_viewport[0] = left;
    the_viewport[1] = right;
    the_viewport[2] = bottom;
    the_viewport[3] = top;
}
 
// XXX display-listable?
void reshapeviewport() { 
    long xsize, ysize;
    getsize(&xsize, &ysize);
    viewport(0, xsize-1, 0, ysize-1);
}

void rotate(Angle ang, unsigned char axis) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(ROTATE);
        e->rotate.angle = ang;
        e->rotate.axis = axis;
        return;
    }

    if(trace_functions) printf("%*srotate %d %c\n", indent, "", ang, axis);

    float m[16];

    matrix4x4f_copy(m, identity_4x4f);

    float s;
    float c;
    float t;
    float d;
    float x = 0.0, y = 0.0, z = 0.0;

    if(axis == 'x' || axis == 'X') {
        x = 1.0;
    } else if(axis == 'y' || axis == 'Y') {
        y = 1.0;
    } else {
        z = 1.0;
    }

    float angle = ang / 1800.0 * M_PI;
    d = sqrtf(x * x + y * y + z * z);
    x /= d;
    y /= d;
    z /= d;

    /*
     * Rotation around axis from Graphics Gems 1, p.466
     */
    s = sinf(angle);
    c = cosf(angle);
    t = 1 - cosf(angle);
    m[0] = t * x * x + c;
    m[5] = t * y * y + c;
    m[10] = t * z * z + c;
    m[1] = t * x * y + s * z;
    m[4] = t * x * y - s * z;
    m[2] = t * x * z - s * y;
    m[8] = t * x * z + s * y;
    m[6] = t * y * z + s * x;
    m[9] = t * y * z - s * x;

    matrix4x4f_stack_mult(current_stack, m);
}

void setpattern() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void shademodel(long mode) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

static int32_t font_width, font_rowbytes, font_height;
static uint8_t font_bits[];

void swapbuffers() {
    if(trace_functions) printf("%*sswapbuffers\n", indent, "");
    send_uint8(COMMAND_SWAPBUFFERS);
    flush();
}

void translate(Coord x, Coord y, Coord z) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(TRANSLATE);
        e->translate.x = x;
        e->translate.y = y;
        e->translate.z = z;
        return;
    }

    if(trace_functions) printf("%*stranslate %f %f %f\n", indent, "", x, y, z);

    float m[16];

    matrix4x4f_translate(x, y, z, m);
    matrix4x4f_stack_mult(current_stack, m);
}

void window(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(WINDOW);
        e->window.left = left;
        e->window.right = right;
        e->window.bottom = bottom;
        e->window.top = top;
        e->window.near = near;
        e->window.left = left;
        return;
    }

    if(trace_functions) printf("%*swindow(%f, %f, %f, %f, %f, %f);\n", indent, "", left, right, bottom, top, near, far);

    float m[16];

    matrix4x4f_copy(m, identity_4x4f);

    m[0] = 2 * near / (right - left);
    m[5] = 2 * near / (top - bottom);
    m[8] = (right + left) / (right - left);
    m[9] = (top + bottom) / (top - bottom);
    m[10] = - (far + near) / (far - near);
    m[14] = - 2 * far * near / (far - near);
    matrix4x4f_stack_load(&projection_stack, m);
}

long winopen(char *title) {
    if(trace_functions) printf("%*swinopen %s\n", indent, "", title);
    open_connection();
    send_uint8(COMMAND_WINOPEN);
    send_string(title);
    return 1;
}

void RGBcolor(long r, long g, long b) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(RGBCOLOR);
        e->rgbcolor.r = r;
        e->rgbcolor.g = g;
        e->rgbcolor.b = b;
        return;
    }

    if(trace_functions) printf("%*sRGBcolor %ld %ld %ld\n", indent, "", r, g, b);

    current_color[0] = r / 255.0;
    current_color[1] = g / 255.0;
    current_color[2] = b / 255.0;
    current_color[3] = 1.0;
}

void RGBmode() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void addtopup() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

static int polygon_vert_count = 0;
static world_vertex polygon_verts[POLY_MAX];

void bgnpolygon() {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(BGNPOLYGON);
        return;
    }

    if(trace_functions) printf("%*sbgnpolygon\n", indent, "");

    polygon_vert_count = 0;
}

void c3f(float c[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(C3F);
        vec3f_copy(e->c3f.c, c);
        return;
    }
    
    if(trace_functions) printf("%*sc3f({%f, %f, %f})\n", indent, "", c[0], c[1], c[2]);

    vec4f_set(current_color, c[0], c[1], c[2], 1.0f);
}

long defpup(char *menu) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

long dopup() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void endpolygon() {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(ENDPOLYGON);
        return;
    }

    if(trace_functions) printf("%*sendpolygon(); /* %d verts */\n", indent, "", polygon_vert_count);

    process_polygon(polygon_vert_count, polygon_verts);
}

void freepup() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void draw(Coord x, Coord y, Coord z) {
    world_vertex v0, v1;
    vec4f_copy(v0.coord, current_position);
    vec3f_copy(v0.color, current_color);

    current_position[0] = x;
    current_position[1] = y;
    current_position[2] = z;

    vec4f_copy(v1.coord, current_position);
    vec3f_copy(v1.color, current_color);

    int save_lighting = lighting_enabled;
    lighting_enabled = 0;
    process_line(&v0, &v1);
    lighting_enabled = save_lighting;
}

void drawi(Icoord x, Icoord y, Icoord z) {
    draw(x, y, z);
}

void draw2i(Icoord x, Icoord y) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(DRAW2I);
        e->draw2i.x = x;
        e->draw2i.y = y;
        return;
    }

    if(trace_functions) printf("%*sdraw2i(%ld, %ld);\n", indent, "", x, y);

    draw(x, y, 0);
}

void pclos() {
    endpolygon();
}

void pmv(Coord x, Coord y, Coord z) {
    bgnpolygon();
    pdr(x, y, z);
}

void pmv2i(Icoord x, Icoord y) {
    pmv(x, y, 0);
}

void pmvi(Icoord x, Icoord y, Icoord z) {
    pmv(x, y, z);
}

void pdr(Coord x, Coord y, Coord z) {
    vec3f p;

    p[0] = x;
    p[1] = y;
    p[2] = z;

    v3f(p);
}

void pdr2i(Icoord x, Icoord y) {
    pdr(x, y, 0);
}

void pdri(Icoord x, Icoord y, Icoord z) {
    pdr(x, y, z);
}

void pnt(Coord x, Coord y, Coord z) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void keepaspect() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void linewidth(short w) {
    if(trace_functions) printf("%*slinewidth(%d);\n", indent, "", w);
    the_linewidth = w;
}

void lmbind(short target, long index) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(LMBIND);
        e->lmbind.target = target;
        e->lmbind.index = index;
        return;
    }

    if(trace_functions) printf("%*slmbind(%d, %ld)\n", indent, "", target, index);

    if(target == MATERIAL) {
        material_bound = (index == 0) ? NULL : &materials[index - 1];
        lighting_enabled = (material_bound != NULL) && (lmodel_bound != NULL);
    } else if(target >= LIGHT0 && target <= LIGHT7) {
        lights_bound[target - LIGHT0] = (index == 0) ? NULL : &lights[index - 1];
    } else if(target == LMODEL) {
        lmodel_bound = (index == 0) ? NULL : &lmodels[index - 1];
        lighting_enabled = (material_bound != NULL) && (lmodel_bound != NULL);
    } else
        abort();
}

void lmdef(short deftype, long index, short numpoints, float properties[]) {
    if(index == 0)
        abort();
    index -=1;
    float *p = properties;
    long next = 0;
    if(deftype == DEFMATERIAL) {
        material *m = &materials[index];
        if(trace_functions) printf("%*slmdef(DEFMATERIAL, %ld, %d, {\n", indent, "", index + 1, numpoints);
        while(*p != LMNULL) {
            switch((int)*p) {
                case DIFFUSE:
                    if(trace_functions) printf("%*sDIFFUSE, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(m->diffuse, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case EMISSION:
                    if(trace_functions) printf("%*sEMISSION, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(m->emission, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case AMBIENT:
                    if(trace_functions) printf("%*sAMBIENT, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(m->ambient, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case SPECULAR:
                    if(trace_functions) printf("%*sSPECULAR, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(m->specular, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case SHININESS:
                    if(trace_functions) printf("%*sSHININESS, %f,\n", indent + 4, "", p[1]);
                    m->shininess = p[1];
                    p+= 2;
                    break;
                case ALPHA:
                    if(trace_functions) printf("%*sALPHA, %f,\n", indent + 4, "", p[1]);
                    m->alpha = p[1];
                    p+= 2;
                    break;
                default:
                    abort();
            }
        }
        if(trace_functions) printf("%*sLMNULL }\n", indent + 4, "");
    } else if(deftype == DEFLIGHT) {
        light *l = &lights[index];
        if(trace_functions) printf("%*slmdef(DEFLIGHT, %ld, %d, {\n", indent, "", index + 1, numpoints);

        while(*p != LMNULL) {
            switch((int)*p) {
                case SPOTDIRECTION: {
                    vec3f direction;
                    vec3f transformed;
                    if(trace_functions) printf("%*sSPOTDIRECTION, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    // XXX probably not right
                    vec3f_set(direction, p[0], p[1], p[2]);
                    vec3f_mult_matrix4x4f(direction, matrix4x4f_stack_get_inverse(&modelview_stack), transformed);
                    vec4f_set(l->spotdirection, transformed[0], transformed[1], transformed[2], 0.0f);
                    p+= 4;
                    break;
                }
                case AMBIENT:
                    if(trace_functions) printf("%*sAMBIENT, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(l->ambient, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case LCOLOR:
                    if(trace_functions) printf("%*sLCOLOR, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(l->color, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case POSITION:
                    if(trace_functions) printf("%*sPOSITION, %f, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3], p[4]);
                    if(p[4] == 0.0) {
                        vec3f direction;
                        vec3f transformed;
                        // XXX may not be right
                        vec3f_set(direction, p[0], p[1], p[2]);
                        vec3f_mult_matrix4x4f(direction, matrix4x4f_stack_get_inverse(&modelview_stack), transformed);
                        vec4f_set(l->position, transformed[0], transformed[1], transformed[2], 0.0f);
                    } else {
                        vec4f position;
                        vec4f_set(position, p[0], p[1], p[2], p[3]);
                        matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&modelview_stack), position, l->position);
                    }
                    p+= 5;
                    break;
                case SPOTLIGHT:
                    if(trace_functions) printf("%*sSPOTLIGHT, %f, %f,\n", indent + 4, "", p[1], p[2]);
                    l->spotlight[0] = p[1];
                    l->spotlight[1] = p[2];
                    p+= 3;
                    break;
                default:
                    abort();
            }
        }
        if(trace_functions) printf("%*sLMNULL }\n", indent + 4, "");
    } else if(deftype == DEFLMODEL) {
        lmodel *lm = &lmodels[index];
        if(trace_functions) printf("%*slmdef(DEFLMODEL, %ld, %d, {\n", indent, "", index + 1, numpoints);
        while(*p != LMNULL) {
            switch((int)*p) {
                case LOCALVIEWER:
                    if(trace_functions) printf("%*sLOCALVIEWER, %f,\n", indent + 4, "", p[1]);
                    lm->local = p[1];
                    p+= 2;
                    break;
                case AMBIENT:
                    if(trace_functions) printf("%*sAMBIENT, %f, %f, %f,\n", indent + 4, "", p[1], p[2], p[3]);
                    vec3f_set(lm->ambient, p[1], p[2], p[3]);
                    p+= 4;
                    break;
                case ATTENUATION:
                    if(trace_functions) printf("%*sATTENUATION, %f, %f,\n", indent + 4, "", p[1], p[2]);
                    lm->attenuation[0] = p[1];
                    lm->attenuation[1] = p[2];
                    p+= 3;
                    break;
                default:
                    abort();
            }
        }
        if(trace_functions) printf("%*sLMNULL }\n", indent + 4, "");
    } else {
        abort();
    }
}

void loadmatrix(Matrix m) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(LOADMATRIX);
        memcpy(e->loadmatrix.m, m, sizeof(Matrix));
        return;
    }

    if(trace_functions)
    {
        printf("%*sloadmatrix\n", indent, "");
        for(int i = 0 ; i < 4; i++)
            printf("%*s    %f %f %f %f\n", indent, "",
                m[i][0], m[i][1], m[i][2], m[i][3]);
    }

    matrix4x4f_stack_load(current_stack, (float *)m);
}

void mmode(long mode) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(MMODE);
        e->mmode.mode = mode;
        return;
    }

    if(trace_functions) printf("%*smmode(%ld)\n", indent, "", mode);

    matrix_mode = mode;
    switch(mode) {
        case MSINGLE: current_stack = &modelview_stack; break;
        case MVIEWING: current_stack = &modelview_stack; break;
        case MPROJECTION: current_stack = &projection_stack; break;
    }
}

void move(Coord x, Coord y, Coord z) {
    vec4f_set(current_position, x, y, z, 1.0);
}

void movei(Icoord x, Icoord y, Icoord z) {
    move(x, y, z);
}

void move2i(Icoord x, Icoord y) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(MOVE2I);
        e->move2i.x = x;
        e->move2i.y = y;
        return;
    }

    if(trace_functions) printf("%*smove2i(%ld, %ld);\n", indent, "", x, y);

    move(x, y, 0);
}

void n3f(float n[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(N3F);
        vec3f_copy(e->n3f.n, n);
        return;
    }

    if(trace_functions) printf("%*sn3f({%f, %f, %f})\n", indent, "", n[0], n[1], n[2]);

    vec3f_copy(current_normal, n);
}

long newpup() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void prefposition() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void qenter() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void rdr2i(Icoord dx, Icoord dy) {
    if(trace_functions) printf("%*srdr2i(%ld, %ld);\n", indent, "", dx, dy);

    world_vertex v0, v1;
    vec4f_copy(v0.coord, current_position);
    vec3f_copy(v0.color, current_color);

    current_position[0] += dx;
    current_position[1] += dy;

    vec4f_copy(v1.coord, current_position);
    vec3f_copy(v1.color, current_color);

    int save_lighting = lighting_enabled;
    lighting_enabled = 0;
    process_line(&v0, &v1);
    lighting_enabled = save_lighting;
}

void rmv2i(Icoord dx, Icoord dy) {
    if(trace_functions) printf("%*srmv2i(%ld, %ld);\n", indent, "", dx, dy);
    current_position[0] += dx;
    current_position[1] += dy;
}

void scale(float x, float y, float z) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(SCALE);
        e->scale.x = x;
        e->scale.y = y;
        e->scale.z = z;
        return;
    }

    if(trace_functions) printf("%*sscale(%f, %f, %f);\n", indent, "", x, y, z);

    float m[16];

    matrix4x4f_scale(x, y, z, m);
    matrix4x4f_stack_mult(current_stack, m);
}

void tie(long button, long val1, long val2) {
    send_uint8(COMMAND_TIE);
    send_uint32(button);
    send_uint32(val1);
    send_uint32(val2);
}

void v3f(float v[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(V3F);
        vec3f_copy(e->v3f.v, v);
        return;
    }

    if(trace_functions) printf("%*sv3f({%f, %f, %f})\n", indent, "", v[0], v[1], v[2]);

    if(polygon_vert_count > POLY_MAX - 2)
        abort();
    world_vertex *wv = polygon_verts + polygon_vert_count;
    vec4f_set(wv->coord, v[0], v[1], v[2], 1.0f);
    vec4f_copy(wv->color, current_color);
    vec3f_copy(wv->normal, current_normal);
    polygon_vert_count++;
}

long winattach() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void winconstraints() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

long winget() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
    return 1;
}

void winposition() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void getmatrix(Matrix m) {
    if(trace_functions)
        printf("%*sgetmatrix();\n", indent, "");
    if(matrix_mode == MSINGLE)
        matrix4x4f_copy((float*)m, matrix4x4f_stack_top(&modelview_stack));
    else 
        matrix4x4f_copy((float*)m, matrix4x4f_stack_top(current_stack));
}

void lookat(Coord viewx,Coord viewy, Coord viewz, Coord pointx, Coord pointy, Coord pointz, Angle twist) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(LOOKAT);
        e->lookat.viewx = viewx;
        e->lookat.viewy = viewy;
        e->lookat.viewz = viewz;
        e->lookat.pointx = pointx;
        e->lookat.pointy = pointy;
        e->lookat.pointz = pointz;
        e->lookat.twist = twist;
        return;
    }

    if(trace_functions)
        printf("%*slookat(%f, %f, %f, %f, %f, %f, %u)\n", indent, "",
            viewx, viewy, viewz,
            pointx, pointy, pointz,
            twist);

    vec3f f;
    vec3f up;
    f[0] = pointx - viewx;
    f[1] = pointy - viewy;
    f[2] = pointz - viewz;

    if(f[0] != 0.0 || f[2] != 0.0) {
        up[0] = 0.0; up[1] = 1.0; up[2] = 0.0;
    } else {
        up[0] = 0.0; up[1] = 0.0; up[2] = -1.0;
    }

    vec3f_normalize(f, f);
    vec3f_normalize(up, up);
    vec3f s, u;
    vec3f_cross(f, up, s);
    vec3f_cross(s, f, u);

    float m[16];

    m[0] =    s[0]; m[1]  =   u[0]; m[2]  =  -f[0]; m[3]  = 0;
    m[4] =    s[1]; m[5]  =   u[1]; m[6]  =  -f[1]; m[7]  = 0;
    m[8] =    s[2]; m[9]  =   u[2]; m[10] =  -f[2]; m[11] = 0;
    m[12] =      0; m[13] =      0; m[14] =      0; m[15] = 1;

    matrix4x4f_stack_mult(current_stack, (float *)m);

    matrix4x4f_translate(-viewx, -viewy, -viewz, m);
    matrix4x4f_stack_mult(current_stack, (float *)m);

    matrix4x4f_rotate(.1 * twist, f[0], f[1], f[2], m);
    matrix4x4f_stack_mult(current_stack, (float *)m);
}

void lsetdepth() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void charstr(char *str) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(CHARSTR);
        e->charstr.str = strdup(str);
        return;
    }

    if(trace_functions) printf("%*scharstr(\"%s\");\n", indent, "", str);

    static lit_vertex vert;
    static screen_vertex screenvert;

    vec4f_copy(vert.coord, current_character_position);
    vec4f_copy(vert.color, current_color);

    int code = classify_vertex(vert.coord);
    if(code != CLIP_ALL_IN)
        return;

    project_vertex(&vert, &screenvert);

    for(int i = 0; i < strlen(str); i++) {
        // hardcode 8x16 for now
        draw_bitmap(&screenvert, NULL, font_bits + str[i] * font_height * font_rowbytes, font_width, font_rowbytes, font_height);
        screenvert.x += font_width * SCREEN_VERTEX_V2_SCALE;
    }
}

#define CIRCLE_SEGMENTS 80
static float circle_verts[CIRCLE_SEGMENTS][2];

void circi(Icoord x, Icoord y, Icoord r) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(CIRCI);
        e->circi.x = x;
        e->circi.y = y;
        e->circi.r = r;
        return;
    }

    if(trace_functions) printf("%*scirci(%ld, %ld, %ld);\n", indent, "", x, y, r);
    world_vertex v0, v1;

    vec3f_copy(v0.color, current_color);
    vec3f_copy(v1.color, current_color);

    int save_lighting = lighting_enabled;
    lighting_enabled = 0;

    v0.coord[2] = 0.0;
    v0.coord[3] = 1.0;
    v1.coord[2] = 0.0;
    v1.coord[3] = 1.0;

    for(int i = 0; i < CIRCLE_SEGMENTS; i++) {  
        v0.coord[0] = x + r * circle_verts[i][0];
        v0.coord[1] = y + r * circle_verts[i][1];
        v1.coord[0] = x + r * circle_verts[(i + 1) % CIRCLE_SEGMENTS][0];
        v1.coord[1] = y + r * circle_verts[(i + 1) % CIRCLE_SEGMENTS][1];

        process_line(&v0, &v1);
    }

    lighting_enabled = save_lighting;
}

void cmov2i(Icoord x, Icoord y) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(CMOV2I);
        e->cmov2i.x = x;
        e->cmov2i.y = y;
        return;
    }

    if(trace_functions) printf("%*scmov2i(%ld, %ld);\n", indent, "", x, y);
        
    vec4f c, p;
    vec4f_set(c, x, y, 0.0, 1.0);
    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&modelview_stack), c, p);
    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&projection_stack), p, current_character_position);
}

void cursoff() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void curson() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void feedback() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

long endfeedback() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void writemask(Colorindex mask) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object(WRITEMASK);
        e->writemask.mask = mask;
        return;
    }

    if(trace_functions) printf("%*swritemask(%u);\n", indent, "", mask);

    current_writemask = mask;
}

void rectf(Coord x1, Coord y1, Coord x2, Coord y2) {
    pmv(x1, y1, 0);
    pdr(x1, y2, 0);
    pdr(x2, y2, 0);
    pdr(x2, y1, 0);
    pclos();
}

void rectfi(Icoord x1, Icoord y1, Icoord x2, Icoord y2) {
    rectf(x1, y1, x2, y2);
}

void poly(int n, Coord p[][3]) {
    for (int i = 0; i <= n; i++) {
        Coord *v = p[i % n];

        if (i == 0) {
            move(v[0], v[1], v[2]);
        } else {
            draw(v[0], v[1], v[2]);
        }
    }
}

// XXX display list
void zbuffer(int enable) {
    send_uint8(COMMAND_ZBUFFER);
    send_uint8(enable);
}

// XXX display list
void zclear() {
    send_uint8(COMMAND_ZCLEAR);
}

static void init_gl_state() __attribute__((constructor));
static void init_gl_state()
{
    if(getenv("TRACE_GL") != NULL)
        trace_functions = 1;

    matrix4x4f_stack_init(&modelview_stack);
    matrix4x4f_stack_init(&projection_stack);
    matrix4x4f_stack_load(&modelview_stack, identity_4x4f);
    matrix4x4f_stack_load(&projection_stack, identity_4x4f);
    current_stack = &modelview_stack;

    the_viewport[0] = 0.0;
    the_viewport[1] = DISPLAY_WIDTH - 1.0;
    the_viewport[2] = 0.0;
    the_viewport[3] = DISPLAY_HEIGHT - 1.0;
    the_viewport[4] = 0.0;
    the_viewport[5] = 1.0;

    for(int i = 0; i < MAX_MATERIALS; i++)
        material_init(&materials[i]);
    for(int i = 0; i < MAX_LIGHTS; i++)
        light_init(&lights[i]);
    for(int i = 0; i < MAX_LMODELS; i++)
        lmodel_init(&lmodels[i]);

    for(int i = 0; i < MAX_LIGHTS; i++)
        lights_bound[i] = NULL;

    for(int i = 0; i < CIRCLE_SEGMENTS; i++) {
        float a = i * M_PI * 2 / CIRCLE_SEGMENTS;
        circle_verts[i][0] = cos(a);
        circle_verts[i][1] = sin(a);
    }

    vec3ub_set(colormap[BLACK], 0, 0, 0);
    vec3ub_set(colormap[RED], 255, 0, 0);
    vec3ub_set(colormap[GREEN], 0, 255, 0);
    vec3ub_set(colormap[YELLOW], 255, 255, 0);
    vec3ub_set(colormap[BLUE], 0, 0, 255);
    vec3ub_set(colormap[MAGENTA], 255, 0, 255);
    vec3ub_set(colormap[CYAN], 0, 255, 255);
    vec3ub_set(colormap[WHITE], 255, 255, 255);
}

static int32_t font_width = 8, font_rowbytes = 1, font_height = 16;
static uint8_t font_bits[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x81, 0xa5, 0x81, 0x81, 0xbd, 0x99, 0x81, 
0x81, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xff, 0xdb, 0xff, 0xff, 
0xc3, 0xe7, 0xff, 0xff, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x6c, 0xfe, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0xe7, 0xe7, 0xe7, 0x18, 0x18, 
0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 
0x7e, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x99, 0xbd, 0xbd, 
0x99, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1e, 0x0e, 0x1a, 0x32, 
0x78, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 
0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0x33, 0x3f, 0x30, 0x30, 0x30, 0x30, 0x70, 0xf0, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x63, 0x67, 
0xe7, 0xe6, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0xdb, 0x3c, 
0xe7, 0x3c, 0xdb, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 
0xf0, 0xf8, 0xfe, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x02, 0x06, 0x0e, 0x1e, 0x3e, 0xfe, 0x3e, 0x1e, 0x0e, 0x06, 0x02, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 
0x66, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xdb, 0xdb, 
0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0xc6, 0x60, 0x38, 0x6c, 0xc6, 0xc6, 0x6c, 0x38, 0x0c, 0xc6, 0x7c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x7e, 
0x3c, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 
0xc0, 0xc0, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x10, 0x38, 0x38, 0x7c, 0x7c, 0xfe, 0xfe, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x7c, 0x7c, 0x38, 0x38, 0x10, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 
0x3c, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 
0x66, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 
0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7c, 0xc6, 0xc2, 0xc0, 0x7c, 0x06, 0x06, 
0x86, 0xc6, 0x7c, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xc6, 
0x0c, 0x18, 0x30, 0x60, 0xc6, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 
0x6c, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x30, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 
0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 
0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x02, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0xc6, 0xc6, 0x7c, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 
0x18, 0x30, 0x60, 0xc0, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0xc6, 0x06, 0x06, 0x3c, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x0c, 0x1e, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06, 0x06, 0x06, 
0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 
0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xc6, 
0x06, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x06, 0x06, 0x0c, 
0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 
0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x0c, 
0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x0c, 
0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0xc6, 0xc6, 0xc6, 0xde, 0xde, 0xde, 0xdc, 0xc0, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 
0x66, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 
0xc0, 0xc0, 0xc2, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x6c, 
0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x60, 0x60, 
0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xde, 
0xc6, 0xc6, 0x66, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 
0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0xcc, 0x78, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6, 0x66, 0x66, 0x6c, 0x78, 0x78, 0x6c, 
0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x60, 0x60, 0x60, 
0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 
0xe7, 0xff, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 
0x60, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 
0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x0c, 0x0e, 0x00, 0x00, 0x00, 
0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x60, 0x38, 0x0c, 0x06, 0xc6, 0xc6, 
0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xdb, 0x99, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 
0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xdb, 0xff, 0x66, 0x66, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0x66, 0x3c, 0x18, 0x18, 0x3c, 
0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0x66, 
0x3c, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
0xc3, 0x86, 0x0c, 0x18, 0x30, 0x60, 0xc1, 0xc3, 0xff, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 
0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 
0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
0x00, 0x00, 0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c, 0x7c, 
0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x60, 0x60, 
0x78, 0x6c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x1c, 0x0c, 0x0c, 0x3c, 0x6c, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 
0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x64, 0x60, 
0xf0, 0x60, 0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xcc, 0x78, 0x00, 
0x00, 0x00, 0xe0, 0x60, 0x60, 0x6c, 0x76, 0x66, 0x66, 0x66, 0x66, 0xe6, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x00, 0x0e, 0x06, 
0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0xe0, 0x60, 
0x60, 0x66, 0x6c, 0x78, 0x78, 0x6c, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xff, 0xdb, 0xdb, 0xdb, 0xdb, 
0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66, 
0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x7c, 
0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 
0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0xc6, 0x60, 0x38, 0x0c, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x30, 0x36, 0x1c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 
0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 
0xc3, 0xdb, 0xdb, 0xff, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 
0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xcc, 0x18, 0x30, 0x60, 0xc6, 
0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x18, 0x18, 0x18, 0x70, 0x18, 
0x18, 0x18, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 
0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x70, 0x18, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 
0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 
0xc0, 0xc0, 0xc2, 0x66, 0x3c, 0x0c, 0x06, 0x7c, 0x00, 0x00, 0x00, 0x00, 0xcc, 
0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0c, 0x18, 0x30, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 
0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x78, 0x0c, 
0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 
0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x38, 0x6c, 0x38, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x60, 0x60, 0x66, 0x3c, 0x0c, 
0x06, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0x00, 0x7c, 0xc6, 0xfe, 
0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 
0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 
0x30, 0x18, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x00, 0x38, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 
0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00, 
0x38, 0x6c, 0x38, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00, 
0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x00, 0xfe, 0x66, 0x60, 0x7c, 0x60, 0x60, 
0x66, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x3b, 
0x1b, 0x7e, 0xd8, 0xdc, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x6c, 
0xcc, 0xcc, 0xfe, 0xcc, 0xcc, 0xcc, 0xcc, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x10, 0x38, 0x6c, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x00, 0x7c, 0xc6, 0xc6, 
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x78, 0xcc, 0x00, 
0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 
0x30, 0x18, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7e, 
0x06, 0x0c, 0x78, 0x00, 0x00, 0xc6, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0xc6, 0xc6, 0xc6, 
0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 
0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x38, 0x6c, 0x64, 0x60, 0xf0, 0x60, 0x60, 0x60, 0x60, 0xe6, 0xfc, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0xff, 0x18, 0xff, 0x18, 
0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x66, 0x66, 0x7c, 0x62, 0x66, 
0x6f, 0x66, 0x66, 0x66, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x1b, 0x18, 
0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0xd8, 0x70, 0x00, 0x00, 0x00, 
0x18, 0x30, 0x60, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x00, 0x7c, 0xc6, 0xc6, 
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0x00, 
0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x76, 0xdc, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 
0x00, 0x76, 0xdc, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x6c, 0x6c, 0x3e, 0x00, 0x7e, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x00, 
0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
0x30, 0x00, 0x30, 0x30, 0x60, 0xc0, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x06, 0x06, 0x06, 
0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc2, 0xc6, 0xcc, 0x18, 
0x30, 0x60, 0xce, 0x9b, 0x06, 0x0c, 0x1f, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc2, 
0xc6, 0xcc, 0x18, 0x30, 0x66, 0xce, 0x96, 0x3e, 0x06, 0x06, 0x00, 0x00, 0x00, 
0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x3c, 0x18, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x6c, 0xd8, 0x6c, 0x36, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd8, 0x6c, 0x36, 
0x6c, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x44, 0x11, 0x44, 0x11, 
0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x55, 0xaa, 
0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 
0xaa, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 
0xdd, 0x77, 0xdd, 0x77, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xf6, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x18, 
0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x36, 0x36, 0x36, 0x36, 
0x36, 0xf6, 0x06, 0xf6, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x06, 0xf6, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xf6, 0x06, 0xfe, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xf8, 0x18, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x3f, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x30, 0x37, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0xf7, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xff, 0x00, 0xf7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36, 0x36, 0xf7, 
0x00, 0xf7, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x18, 0x18, 0x18, 
0x18, 0x18, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xff, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x1f, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0xff, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xf8, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x76, 0xdc, 0xd8, 0xd8, 0xd8, 0xdc, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x78, 0xcc, 0xcc, 0xcc, 0xd8, 0xcc, 0xc6, 0xc6, 0xc6, 0xcc, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xfe, 0xc6, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x6c, 0x6c, 0x6c, 
0x6c, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xc6, 
0x60, 0x30, 0x18, 0x30, 0x60, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7e, 0xd8, 0xd8, 0xd8, 0xd8, 0xd8, 0x70, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xdc, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x18, 0x3c, 
0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x6c, 0x6c, 0x6c, 0xee, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x30, 0x18, 0x0c, 0x3e, 0x66, 0x66, 0x66, 
0x66, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xdb, 
0xdb, 0xdb, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0x06, 0x7e, 0xdb, 0xdb, 0xf3, 0x7e, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1c, 0x30, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x30, 0x1c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0xfe, 
0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00, 0x7e, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x1b, 0x1b, 0x1b, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0xd8, 0xd8, 0xd8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x18, 0x18, 0x00, 0x7e, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xdc, 0x00, 0x76, 0xdc, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x0f, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xec, 0x6c, 0x6c, 0x3c, 0x1c, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xd8, 0x6c, 0x6c, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xd8, 0x30, 0x60, 0xc8, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 
0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, };
