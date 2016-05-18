
// Alice 4 libgl implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gl.h>
#include <device.h>
#include "connection.h"
#include "basic_types.h"
#include "driver.h"

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;
#define POLY_MAX 32

static int trace_functions = 0;
static int trace_network = 0;
static int indent = 0;

static vec3ub colormap[4096];
static vec4f current_color = {1.0f, 1.0f, 1.0f, 1.0f};
static vec3f current_normal = {1.0f, 1.0f, 1.0f};
static vec4f current_position = {0.0f, 0.0f, 0.0f, 1.0};

#define INPUT_QUEUE_SIZE 128
static uint32_t input_queue_device[INPUT_QUEUE_SIZE];
static short input_queue_val[INPUT_QUEUE_SIZE];
// The next time that needs to be read:
static int input_queue_head = 0;
// The number of items in the queue (tail = (head + length) % len):
static int input_queue_length = 0;

//----------------------------------------------------------------------------
// Vector math; vector operations, matrix operations, matrix stack operations

const matrix4x4f identity_4x4f = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
};

void vec3f_set(vec3f v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void vec4f_set(vec4f v, float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

void vec3f_copy(vec3f d, const vec3f s)
{
    memcpy(d, s, sizeof(vec3f));
}

void vec4f_copy(vec4f d, const vec4f s)
{
    memcpy(d, s, sizeof(vec4f));
}

void vec4f_subtract(vec4f i1, vec4f i2, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] - i2[i];
}

void vec3f_scale(vec3f i1, float v, vec3f out)
{
    for(int i = 0; i < 3; i++)
        out[i] = i1[i] * v;
}

void vec4f_scale(vec4f i1, float v, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * v;
}

void vec3f_mult(vec3f i1, vec3f i2, vec3f out)
{
    for(int i = 0; i < 3; i++)
        out[i] = i1[i] * i2[i];
}

void vec4f_mult(vec4f i1, vec4f i2, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * i2[i];
}

void vec3f_blend(vec3f i1, vec3f i2, float t, vec3f out)
{
    for(int i = 0; i < 3; i++)
        out[i] = i1[i] * (1 - t) + i2[i] * t;
}

void vec4f_blend(vec4f i1, vec4f i2, float t, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * (1 - t) + i2[i] * t;
}

void vec4f_add(vec4f i1, vec4f i2, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] + i2[i];
}

void vec3f_add(vec3f i1, vec3f i2, vec3f out)
{
    for(int i = 0; i < 3; i++)
        out[i] = i1[i] + i2[i];
}

float vec3f_dot(vec3f i1, vec3f i2)
{
    float dot = 0;
    for(int i = 0; i < 3; i++)
        dot += i1[i] * i2[i];
    return dot;
}

float vec4f_dot(vec4f i1, vec4f i2)
{
    float dot = 0;
    for(int i = 0; i < 4; i++)
        dot += i1[i] * i2[i];
    return dot;
}

void vec3f_normalize(vec3f i1, vec3f out)
{
    float d = 1.0f/sqrtf(i1[0] * i1[0] + i1[1] * i1[1] + i1[2] * i1[2]);
    for(int i = 0; i < 3; i++)
        out[i] = i1[i] * d;
}

void vec4f_normalize(vec4f i1, vec4f out)
{
    float d = 1.0f/sqrtf(i1[0] * i1[0] + i1[1] * i1[1] + i1[2] * i1[2] + i1[3] * i1[3]);
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * d;
}

void vec3f_cross(vec3f v0, vec3f v1, vec3f result)
{
    result[0] = v0[1] * v1[2] - v0[2] * v1[1];
    result[1] = v0[2] * v1[0] - v0[0] * v1[2];
    result[2] = v0[0] * v1[1] - v0[1] * v1[0];
}

void matrix4x4f_copy(matrix4x4f d, const matrix4x4f s)
{
    memcpy(d, s, sizeof(matrix4x4f));
}

static void matrix4x4f_translate(float x, float y, float z, float matrix[16])
{
    matrix4x4f_copy(matrix, identity_4x4f);
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

static void matrix4x4f_scale(float x, float y, float z, float matrix[16])
{
    matrix4x4f_copy(matrix, identity_4x4f);
    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;
}

static void matrix4x4f_rotate(float a, float x, float y, float z, float matrix[16])
{
    float c, s, t;

    c = (float)cos(a);
    s = (float)sin(a);
    t = 1.0f - c;

    matrix[0] = t * x * x + c;
    matrix[1] = t * x * y + s * z;
    matrix[2] = t * x * z - s * y;
    matrix[3] = 0;

    matrix[4] = t * x * y - s * z;
    matrix[5] = t * y * y + c;
    matrix[6] = t * y * z + s * x;
    matrix[7] = 0;

    matrix[8] = t * x * z + s * y;
    matrix[9] = t * y * z - s * x;
    matrix[10] = t * z * z + c;
    matrix[11] = 0;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

void matrix4x4f_print(char *label, matrix4x4f m)
{
    int i, j;

    printf("%s:\n", label);
    for(j = 0; j < 4; j++) {
        for(i = 0; i < 4; i++) {
            printf("%6.2f ", m[j*4 + i]);
        }
        printf("\n");
    }
    printf("\n");
}

static void matrix4x4f_mult_matrix4x4f(const matrix4x4f m1, const matrix4x4f m2, matrix4x4f r)
{
    matrix4x4f t;
    int i, j;

    for(j = 0; j < 4; j++)
        for(i = 0; i < 4; i++)
           t[i * 4 + j] = m1[i * 4 + 0] * m2[0 * 4 + j] +
               m1[i * 4 + 1] * m2[1 * 4 + j] +
               m1[i * 4 + 2] * m2[2 * 4 + j] +
               m1[i * 4 + 3] * m2[3 * 4 + j];

    matrix4x4f_copy(r, t);
}

static float matrix4x4f_determinant(const matrix4x4f mat)
{
    return (mat[0] * mat[5] - mat[1] * mat[4]) *
        (mat[10] * mat[15] - mat[11] * mat[14]) + 
        (mat[2] * mat[4] - mat[0] * mat[6]) *
	(mat[9] * mat[15] - mat[11] * mat[13]) + 
        (mat[0] * mat[7] - mat[3] * mat[4]) *
	(mat[9] * mat[14] - mat[10] * mat[13]) + 
        (mat[1] * mat[6] - mat[2] * mat[5]) *
	(mat[8] * mat[15] - mat[11] * mat[12]) + 
        (mat[3] * mat[5] - mat[1] * mat[7]) *
	(mat[8] * mat[14] - mat[10] * mat[12]) + 
        (mat[2] * mat[7] - mat[3] * mat[6]) *
	(mat[8] * mat[13] - mat[9] * mat[12]);
}

#define EPSILON .0000001

static int matrix4x4f_invert(const matrix4x4f mat, matrix4x4f inv)
{
    int		i, rswap;
    float	det, div, swap;
    matrix4x4f	hold;

    matrix4x4f_copy(hold, mat);
    matrix4x4f_copy(inv, identity_4x4f);
    det = matrix4x4f_determinant(hold);
    if(fabs(det) < EPSILON) /* singular? */
	return -1;

    rswap = 0;
    /* this loop isn't entered unless [0 + 0] > EPSILON and det > EPSILON,
	 so rswap wouldn't be 0, but I initialize so as not to get warned */
    if(fabs(hold[0]) < EPSILON)
    {
        if(fabs(hold[1]) > EPSILON)
            rswap = 1;
        else if(fabs(hold[2]) > EPSILON)
	    rswap = 2;
        else if(fabs(hold[3]) > EPSILON)
	    rswap = 3;

        for(i = 0; i < 4; i++)
	{
            swap = hold[i * 4 + 0];
            hold[i * 4 + 0] = hold[i * 4 + rswap];
            hold[i * 4 + rswap] = swap;

            swap = inv[i * 4 + 0];
            inv[i * 4 + 0] = inv[i * 4 + rswap];
            inv[i * 4 + rswap] = swap;
        }
    }
        
    div = hold[0];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 0] /= div;
        inv[i * 4 + 0] /= div;
    }

    div = hold[1];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 1] -= div * hold[i * 4 + 0];
        inv[i * 4 + 1] -= div * inv[i * 4 + 0];
    }
    div = hold[2];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 2] -= div * hold[i * 4 + 0];
        inv[i * 4 + 2] -= div * inv[i * 4 + 0];
    }
    div = hold[3];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 3] -= div * hold[i * 4 + 0];
        inv[i * 4 + 3] -= div * inv[i * 4 + 0];
    }

    if(fabs(hold[5]) < EPSILON){
        if(fabs(hold[6]) > EPSILON)
	    rswap = 2;
        else if(fabs(hold[7]) > EPSILON)
	    rswap = 3;

        for(i = 0; i < 4; i++)
	{
            swap = hold[i * 4 + 1];
            hold[i * 4 + 1] = hold[i * 4 + rswap];
            hold[i * 4 + rswap] = swap;

            swap = inv[i * 4 + 1];
            inv[i * 4 + 1] = inv[i * 4 + rswap];
            inv[i * 4 + rswap] = swap;
        }
    }

    div = hold[5];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 1] /= div;
        inv[i * 4 + 1] /= div;
    }

    div = hold[4];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 0] -= div * hold[i * 4 + 1];
        inv[i * 4 + 0] -= div * inv[i * 4 + 1];
    }
    div = hold[6];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 2] -= div * hold[i * 4 + 1];
        inv[i * 4 + 2] -= div * inv[i * 4 + 1];
    }
    div = hold[7];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 3] -= div * hold[i * 4 + 1];
        inv[i * 4 + 3] -= div * inv[i * 4 + 1];
    }

    if(fabs(hold[10]) < EPSILON){
        for(i = 0; i < 4; i++)
	{
            swap = hold[i * 4 + 2];
            hold[i * 4 + 2] = hold[i * 4 + 3];
            hold[i * 4 + 3] = swap;

            swap = inv[i * 4 + 2];
            inv[i * 4 + 2] = inv[i * 4 + 3];
            inv[i * 4 + 3] = swap;
        }
    }

    div = hold[10];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 2] /= div;
        inv[i * 4 + 2] /= div;
    }

    div = hold[8];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 0] -= div * hold[i * 4 + 2];
        inv[i * 4 + 0] -= div * inv[i * 4 + 2];
    }
    div = hold[9];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 1] -= div * hold[i * 4 + 2];
        inv[i * 4 + 1] -= div * inv[i * 4 + 2];
    }
    div = hold[11];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 3] -= div * hold[i * 4 + 2];
        inv[i * 4 + 3] -= div * inv[i * 4 + 2];
    }

    div = hold[15];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 3] /= div;
        inv[i * 4 + 3] /= div;
    }

    div = hold[12];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 0] -= div * hold[i * 4 + 3];
        inv[i * 4 + 0] -= div * inv[i * 4 + 3];
    }
    div = hold[13];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 1] -= div * hold[i * 4 + 3];
        inv[i * 4 + 1] -= div * inv[i * 4 + 3];
    }
    div = hold[14];
    for(i = 0; i < 4; i++)
    {
        hold[i * 4 + 2] -= div * hold[i * 4 + 3];
        inv[i * 4 + 2] -= div * inv[i * 4 + 3];
    }
    
    return 0;
}

static void matrix4x4f_mult_vec4f(const matrix4x4f m, const vec4f in, vec4f out)
{
    int i;
    vec4f t;

    for(i = 0; i < 4; i++)
        t[i] =
            m[0 + i] * in[0] +
            m[4 + i] * in[1] +
            m[8 + i] * in[2] +
            m[12 + i] * in[3];
    vec4f_copy(out, t);
}

static void vec3f_mult_matrix4x4f(const vec3f in, const matrix4x4f m, vec3f out)
{
    int i;
    vec3f t;

    for(i = 0; i < 3; i++)
        t[i] =
            in[0] * m[0 + i * 4] +
            in[1] * m[1 + i * 4] +
            in[2] * m[2 + i * 4];
    vec3f_copy(out, t);
}

static void vec4f_mult_matrix4x4f(const vec4f in, const matrix4x4f m, vec4f out)
{
    int i;
    vec4f t;

    for(i = 0; i < 4; i++)
        t[i] =
            in[0] * m[0 + i * 4] +
            in[1] * m[1 + i * 4] +
            in[2] * m[2 + i * 4] +
            in[3] * m[3 + i * 4];
    vec4f_copy(out, t);
}

static void vec4f_print(char *label, const vec4f v)
{
    printf("%s: ", label);
    for (int i = 0; i < 4; i++) {
        printf("%6.2f ", v[i]);
    }
    printf("\n");
}

#define STACK_MAX 8

typedef struct matrix4x4f_stack
{
    matrix4x4f s[STACK_MAX];

    int top;

    int inverse_needs_calculation;
    matrix4x4f inverse;
} matrix4x4f_stack;

float *matrix4x4f_stack_top(matrix4x4f_stack *m)
{
    return m->s[m->top];
}

void matrix4x4f_stack_init(matrix4x4f_stack *m)
{
    m->top = 0;
    m->inverse_needs_calculation = 1;
}

float *matrix4x4f_stack_get_inverse(matrix4x4f_stack *stack)
{
    if(stack->inverse_needs_calculation) {
        matrix4x4f_invert(stack->s[stack->top], stack->inverse);
        stack->inverse_needs_calculation = 0;
    }
    return stack->inverse;
}

int matrix4x4f_stack_push(matrix4x4f_stack *stack)
{
    if(stack->top == STACK_MAX - 1) {
        return 0;
    } else {
        matrix4x4f_copy(stack->s[stack->top + 1], stack->s[stack->top]);
        stack->inverse_needs_calculation = 1;
        stack->top++;
        return 1;
    }
}

int matrix4x4f_stack_pop(matrix4x4f_stack *stack)
{
    if(stack->top == 0) {
        return 0;
    } else {
        stack->top--;
        stack->inverse_needs_calculation = 1;
        return 1;
    }
}

void matrix4x4f_stack_load(matrix4x4f_stack *stack, const matrix4x4f m)
{
    matrix4x4f_copy(stack->s[stack->top], m);
    stack->inverse_needs_calculation = 1;
#if 0
    printf("matrix stack loaded\n");
    for(int i = 0 ; i < 4; i++)
        printf("    %f %f %f %f\n", 
            stack->s[stack->top][i * 4 + 0],
            stack->s[stack->top][i * 4 + 1],
            stack->s[stack->top][i * 4 + 2],
            stack->s[stack->top][i * 4 + 3]);
#endif
}

void matrix4x4f_stack_mult(matrix4x4f_stack *stack, const matrix4x4f m)
{
    matrix4x4f_mult_matrix4x4f(m, stack->s[stack->top], stack->s[stack->top]);
    stack->inverse_needs_calculation = 1;
#if 0
    printf("matrix stack multiplied\n");
    for(int i = 0 ; i < 4; i++)
        printf("    %f %f %f %f\n", 
            stack->s[stack->top][i * 4 + 0],
            stack->s[stack->top][i * 4 + 1],
            stack->s[stack->top][i * 4 + 2],
            stack->s[stack->top][i * 4 + 3]);
#endif
}

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

float clamp(float v)
{
    return v > 1.0f ? 1.0f : (v < 0.0f ? 0.0f : v);
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

typedef struct screen_vertex_hd
{
    uint32_t x, y;
    uint32_t z;
    uint8_t r, g, b, a;
} screen_vertex_hd;


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

void project_vertex_hd(lit_vertex *lv, screen_vertex_hd *sv)
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

    sv->x = xw * 65536;
    sv->y = yw * 65536;
    sv->z = zw * 0xffffffff;
    sv->r = clamp(lv->color[0]) * 255;
    sv->g = clamp(lv->color[1]) * 255;
    sv->b = clamp(lv->color[2]) * 255;
    sv->a = clamp(lv->color[3]) * 255;
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

    sv->x = xw;
    sv->y = yw;
    sv->z = zw * 0xffffffff;
    sv->r = clamp(lv->color[0]) * 255;
    sv->g = clamp(lv->color[1]) * 255;
    sv->b = clamp(lv->color[2]) * 255;
    sv->a = clamp(lv->color[3]) * 255;
}

void send_screen_vertex(screen_vertex *sv) {
    send_uint16(sv->x);
    send_uint16(sv->y);
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

enum {
    CLIP_TRIVIAL_REJECT = 0,
    CLIP_TRIVIAL_ACCEPT = -1, // If clip_polygon_against_plane() returns this, it stored nothing in "output".
};

#define CLIP_EPSILON .001

long clip_polygon_against_plane(long plane, long n, lit_vertex *input, lit_vertex *output)
{
    int sign;
    int index;

    switch(plane) {
        case CLIP_NEG_X: sign = -1; index = 0; break;
        case CLIP_POS_X: sign = 1; index = 0; break;
        case CLIP_NEG_Y: sign = -1; index = 1; break;
        case CLIP_POS_Y: sign = 1; index = 1; break;
        case CLIP_NEG_Z: sign = -1; index = 2; break;
        case CLIP_POS_Z: sign = 1; index = 2; break;
    }

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

        if((p0 < w0 && p1 > w1) || (p0 > w0 && p1 < w1)) {
            float denom = -p0 + p1 + w0 - w1;
            if(fabs(denom) > CLIP_EPSILON) {
                float t = (-p0 + w0) / denom;
                if(t > 0.001 && t < .999) {
                    vec4f_blend(v0->coord, v1->coord, t, output[n2].coord);
                    vec4f_blend(v0->color, v1->color, t, output[n2].color);
                    n2++;
                }
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
    n = clip_polygon_against_plane(CLIP_POS_Z, n, tmp, output);
    if(n == 0) return 0;
    n = clip_polygon_against_plane(CLIP_NEG_Z, n, output, tmp);

    return n;
}

// Fake this until rasterizer implements lines
void process_line(world_vertex *wv0, world_vertex *wv1)
{
    static lit_vertex litverts[2], *vp;
    static lit_vertex clipped[2];
    static screen_vertex_hd screenverts[2];

    transform_and_light_vertex(wv0, &litverts[0]);
    transform_and_light_vertex(wv1, &litverts[1]);

#if 0
    long r = clip_line(litverts, clipped);
    if(r == CLIP_TRIVIAL_REJECT)
        return;
    else if(r == CLIP_TRIVIAL_ACCEPT) {
        vp = litverts;
    } else {
        vp = clipped;
    }
#else
    vp = litverts;
#endif

    project_vertex_hd(&vp[0], &screenverts[0]);
    project_vertex_hd(&vp[1], &screenverts[1]);

    float dx = screenverts[1].x - screenverts[0].x;
    float dy = screenverts[1].y - screenverts[0].y;
    float d = sqrt(dx * dx + dy * dy);

    dx = dx * 65536 / d;
    dy = dy * 65536 / d;

    screen_vertex_hd linequad_hd[4];
    linequad_hd[0] = screenverts[0];
    linequad_hd[1] = screenverts[0];
    linequad_hd[2] = screenverts[1];
    linequad_hd[3] = screenverts[1];

    linequad_hd[0].x += -dx * .5 + dy * the_linewidth * .5;
    linequad_hd[0].y += -dy * .5 + -dx * the_linewidth * .5;
    linequad_hd[1].x += -dx * .5 + -dy * the_linewidth * .5;
    linequad_hd[1].y += -dy * .5 + dx * the_linewidth * .5;
    linequad_hd[2].x += -dx * .5 + -dy * the_linewidth * .5;
    linequad_hd[2].y += -dy * .5 + dx * the_linewidth * .5;
    linequad_hd[3].x += -dx * .5 + dy * the_linewidth * .5;
    linequad_hd[3].y += -dy * .5 + -dx * the_linewidth * .5;

    screen_vertex linequad[4];
    for(int i = 0; i < 4; i++) {
        linequad[i].x = linequad_hd[i].x / 65536;
        linequad[i].y = linequad_hd[i].y / 65536;
        linequad[i].r = linequad_hd[i].r;
        linequad[i].g = linequad_hd[i].g;
        linequad[i].b = linequad_hd[i].b;
        linequad[i].a = linequad_hd[i].a;
    }

    process_triangle(&linequad[0], &linequad[1], &linequad[2]);
    process_triangle(&linequad[2], &linequad[3], &linequad[0]);
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
    enum
    {
        TAG,
        CALLOBJ,
        COLOR,
        POLF,
        POLF2I,
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
        VIEWPORT,
        RGBCOLOR,
        BGNPOLYGON,
        ENDPOLYGON,
        C3F,
        N3F,
        V3F,
        MMODE,
        LMBIND,
    } type;

    union
    {
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
            default:
                break;
        }
        element *np = p->next;
        free(p);
        p = np;
    }
}

element *element_next_in_object()
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

    return e;
}


//----------------------------------------------------------------------------
// GL API calls

void callobj(Object obj) { 
    if(cur_ptr_to_nextptr != NULL) {

        element *e = element_next_in_object();
        e->type = CALLOBJ;
        e->callobj.obj = obj;

    } else {

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
                case TAG:
                    if(trace_functions) printf("%*stag %ld\n", indent, "", p->tag.tag);
                    break;
            }
            p = p->next;
        }
        indent -= 4;
    }
}

void backface() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

// XXX display list
void clear() { 
    if(trace_functions) printf("%*sclear\n", indent, "");
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
        element *e = element_next_in_object();
        e->type = COLOR;
        e->color.color = color;
    } else {
        if(trace_functions) printf("%*scolor %u\n", indent, "", color);
        for(int i = 0; i < 3; i++)
            current_color[i] = colormap[color][i] / 255.0;
            // XXX alpha in color map?
    }
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
    if(trace_functions) printf("%*sgetmcolor %d\n", indent, "", index);
    *red = colormap[index][0];
    *green = colormap[index][1];
    *blue = colormap[index][2];
}

void getorigin(long *x, long *y) { 
    *x = 0;
    *y = 0;
}

long getplanes() { 
    return 24;
}

void getsize(long *width, long *height) { 
    *width = DISPLAY_WIDTH;
    *height = DISPLAY_HEIGHT;
}

long getvaluator(long device) { 
    send_uint8(COMMAND_GET_VALUATOR);
    send_uint32(device);
    return receive_uint32();
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
        element *e = element_next_in_object();
        e->type = MULTMATRIX;
        memcpy(e->multmatrix.m, m, sizeof(Matrix));
    } else { 
        if(trace_functions)
        {
        printf("%*smultmatrix\n", indent, "");
        for(int i = 0 ; i < 4; i++)
            printf("%*s    %f %f %f %f\n", indent, "",
                m[i][0], m[i][1], m[i][2], m[i][3]);
        }
        matrix4x4f_stack_mult(current_stack, (float *)m);
    }
}

void objreplace(Tag tag) { 
    if(trace_functions) printf("%*sobjreplace %ld\n", indent, "", tag);
    cur_ptr_to_nextptr = &(*ptrs_to_tagptrs[tag])->next;
    replace_mode = 1;
}

void perspective(Angle fovy_, float aspect, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = PERSPECTIVE;
        e->perspective.fovy = fovy_;
        e->perspective.aspect = aspect;
        e->perspective.near = near;
        e->perspective.far = far;
    } else {
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
}

void polf(long n, Coord parray[ ][3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = POLF;
        e->polf.n = n;
        e->polf.parray = (Coord*) malloc(sizeof(Coord) * 3 * n);
        memcpy(e->polf.parray, parray, sizeof(Coord) * 3 * n);
    } else {
        static world_vertex worldverts[POLY_MAX];

        if(trace_functions) printf("%*spolf %ld\n", indent, "", n);

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
}

void polf2i(long n, Icoord parray[ ][2]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = POLF2I;
        e->polf2i.n = n;
        e->polf2i.parray = (Icoord*) malloc(sizeof(Icoord) * 2 * n);
        memcpy(e->polf.parray, parray, sizeof(Icoord) * 2 * n);
    } else {
        static world_vertex worldverts[POLY_MAX];

        if(trace_functions) printf("%*spolf2i %ld\n", indent, "", n);

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
}

void popmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = POPMATRIX;
    } else {
        if(trace_functions) printf("%*spopmatrix\n", indent, "");
        matrix4x4f_stack_pop(current_stack);
    }
}

void pushmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = PUSHMATRIX;
    } else {
        if(trace_functions) printf("%*spushmatrix\n", indent, "");
        matrix4x4f_stack_push(current_stack);
    }
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
        element *e = element_next_in_object();
        e->type = VIEWPORT;
        e->viewport.left = left;
        e->viewport.right = right;
        e->viewport.bottom = bottom;
        e->viewport.top = top;
    } else {
        if(trace_functions) printf("%*sviewport %d %d %d %d\n", indent, "", left, right, bottom, top);
        the_viewport[0] = left;
        the_viewport[1] = right;
        the_viewport[2] = bottom;
        the_viewport[3] = top;
    }
}

void reshapeviewport() { 
    long xsize, ysize;
    getsize(&xsize, &ysize);
    viewport(0, xsize-1, 0, ysize-1);
}

void rotate(Angle ang, unsigned char axis) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = ROTATE;
        e->rotate.angle = ang;
        e->rotate.axis = axis;
    } else {
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

        if(trace_functions) printf("%*srotate %d %c\n", indent, "", ang, axis);
        matrix4x4f_stack_mult(current_stack, m);
    }
}

void setpattern() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void shademodel(long mode) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void swapbuffers() { 
    if(trace_functions) printf("%*sswapbuffers\n", indent, "");
    send_uint8(COMMAND_SWAPBUFFERS);
    flush();
}

void translate(Coord x, Coord y, Coord z) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = TRANSLATE;
        e->translate.x = x;
        e->translate.y = y;
        e->translate.z = z;
    } else {
        float m[16];

        matrix4x4f_translate(x, y, z, m);

        if(trace_functions) printf("%*stranslate %f %f %f\n", indent, "", x, y, z);
        matrix4x4f_stack_mult(current_stack, m);
    }
}

void window(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = WINDOW;
        e->window.left = left;
        e->window.right = right;
        e->window.bottom = bottom;
        e->window.top = top;
        e->window.near = near;
        e->window.left = left;
    } else {
        float m[16];

        matrix4x4f_copy(m, identity_4x4f);

        m[0] = 2 / (right - left);
        m[5] = 2 / (top - bottom);
        m[10] = -2 / (near - far);
        m[3] = (right + left) / (right - left);
        m[7] = (top + bottom) / (top - bottom);
        m[11] = (far + near) / (far - near);
        if(trace_functions) printf("%*swindow %f %f %f %f %f %f\n", indent, "", left, right, bottom, top, near, far);
        matrix4x4f_stack_load(&projection_stack, m);

    }
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
        element *e = element_next_in_object();
        e->type = RGBCOLOR;
        e->rgbcolor.r = r;
        e->rgbcolor.g = g;
        e->rgbcolor.b = b;
    } else {
        if(trace_functions) printf("%*sRGBcolor %ld %ld %ld\n", indent, "", r, g, b);
        current_color[0] = r;
        current_color[1] = g;
        current_color[2] = b;
        current_color[3] = 255;
    }
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
        element *e = element_next_in_object();
        e->type = BGNPOLYGON;
    } else {
        if(trace_functions) printf("%*sbgnpolygon\n", indent, "");
        polygon_vert_count = 0;
    }
}

void c3f(float c[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = C3F;
        vec3f_copy(e->c3f.c, c);
    } else {
        if(trace_functions) printf("%*sc3f({%f, %f, %f})\n", indent, "", c[0], c[1], c[2]);
        vec4f_set(current_color, c[0], c[1], c[2], 1.0f);
    }
}

long defpup(char *menu) {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

long dopup() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void endpolygon() {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = ENDPOLYGON;
    } else {
        if(trace_functions) printf("%*sendpolygon(); /* %d verts */\n", indent, "", polygon_vert_count);
        process_polygon(polygon_vert_count, polygon_verts);
    }
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

void pclos() {
    endpolygon();
}

void pmv(Coord x, Coord y, Coord z) {
    bgnpolygon();
    pdr(x, y, z);
}

void pdr(Coord x, Coord y, Coord z) {
    vec3f p;

    p[0] = x;
    p[1] = y;
    p[2] = z;

    v3f(p);
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
        element *e = element_next_in_object();
        e->type = LMBIND;
        e->lmbind.target = target;
        e->lmbind.index = index;
    } else { 
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
                    if(p[3] == 0.0) {
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
        element *e = element_next_in_object();
        e->type = LOADMATRIX;
        memcpy(e->loadmatrix.m, m, sizeof(Matrix));
    } else { 
        if(trace_functions)
        {
        printf("%*sloadmatrix\n", indent, "");
        for(int i = 0 ; i < 4; i++)
            printf("%*s    %f %f %f %f\n", indent, "",
                m[i][0], m[i][1], m[i][2], m[i][3]);
        }
        matrix4x4f_stack_load(current_stack, (float *)m);
    }
}

void mmode(long mode) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = MMODE;
        e->mmode.mode = mode;
    } else {
        if(trace_functions) printf("%*smmode(%ld)\n", indent, "", mode);
        matrix_mode = mode;
        switch(mode) {
            case MSINGLE: current_stack = &modelview_stack; break;
            case MVIEWING: current_stack = &modelview_stack; break;
            case MPROJECTION: current_stack = &projection_stack; break;
        }
    }
}

void move(Coord x, Coord y, Coord z) {
    vec4f_set(current_position, x, y, z, 1.0);
}

void move2i(Icoord x, Icoord y) {
    if(trace_functions) printf("%*smove2i(%ld, %ld);\n", indent, "", x, y);
    vec4f_set(current_position, x, y, 0.0, 1.0);
}

void n3f(float n[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = N3F;
        vec3f_copy(e->n3f.n, n);
    } else {
        if(trace_functions) printf("%*sn3f({%f, %f, %f})\n", indent, "", n[0], n[1], n[2]);
        vec3f_copy(current_normal, n);
    }
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
        element *e = element_next_in_object();
        e->type = SCALE;
        e->scale.x = x;
        e->scale.y = y;
        e->scale.z = z;
    } else {
        float m[16];

        if(trace_functions) printf("%*sscale(%f, %f, %f);\n", indent, "", x, y, z);
        matrix4x4f_scale(x, y, z, m);
        matrix4x4f_stack_mult(current_stack, m);
    }
}

void tie(long button, long val1, long val2) {
    send_uint8(COMMAND_TIE);
    send_uint32(button);
    send_uint32(val1);
    send_uint32(val2);
}

void v3f(float v[3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = V3F;
        vec3f_copy(e->v3f.v, v);
    } else {
        if(trace_functions) printf("%*sv3f({%f, %f, %f})\n", indent, "", v[0], v[1], v[2]);
        if(polygon_vert_count > POLY_MAX - 2)
            abort();
        world_vertex *wv = polygon_verts + polygon_vert_count;
        vec4f_set(wv->coord, v[0], v[1], v[2], 1.0f);
        vec4f_copy(wv->color, current_color);
        vec4f_set(wv->color, 1, 0, 0, 1);
        vec3f_copy(wv->normal, current_normal);
        polygon_vert_count++;
    }
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

void getmatrix() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void lookat(Coord viewx,Coord viewy, Coord viewz, Coord pointx, Coord pointy, Coord pointz, Angle twist) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = LOOKAT;
        e->lookat.viewx = viewx;
        e->lookat.viewy = viewy;
        e->lookat.viewz = viewz;
        e->lookat.pointx = pointx;
        e->lookat.pointy = pointy;
        e->lookat.pointz = pointz;
        e->lookat.twist = twist;
    } else { 
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
}

void lsetdepth() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
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
}
