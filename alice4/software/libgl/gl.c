
// Alice 4 libgl implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gl.h>
#include <device.h>
#include "connection.h"
#include "basic_types.h"

static int trace_functions = 0;
static int trace_network = 0;

static vec3ub colormap[4096];
static vec4ub current_color = {255, 255, 255, 255};

#define INPUT_QUEUE_SIZE 128
static long input_queue_device[INPUT_QUEUE_SIZE];
static short input_queue_val[INPUT_QUEUE_SIZE];
// The next time that needs to be read:
static int input_queue_head = 0;
// The number of items in the queue (tail = (head + length) % len):
static int input_queue_length = 0;

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

void vec4f_scale(vec4f i1, float v, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * v;
}

void vec4f_mult(vec4f i1, vec4f i2, vec4f out)
{
    for(int i = 0; i < 4; i++)
        out[i] = i1[i] * i2[i];
}

void vec4f_add(vec4f i1, vec4f i2, vec4f out)
{
    for(int i = 0; i < 4; i++)
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

void matrix4x4f_copy(matrix4x4f d, const matrix4x4f s)
{
    memcpy(d, s, sizeof(matrix4x4f));
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

matrix4x4f_stack modelview_stack;
matrix4x4f_stack projection_stack;

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;

Screencoord the_viewport[6];

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

float clamp(float v)
{
	return v > 1.0f ? 1.0f : (v < 0.0f ? 0.0f : v);
}

// Send a string as a length and the bytes. Max string length is 255.
void send_string(char *s) {
    send_byte(strlen(s));
    while (*s != '\0') {
        send_byte(*s);
        s++;
    }
}

// Little-endian.
void send_ushort(unsigned short x) {
    if (trace_network) {
        printf("Sending short %d\n", x);
    }
    send_byte(x & 0xFF);
    send_byte(x >> 8);
}

// Little-endian.
void send_ulong(unsigned long x) {
    if (trace_network) {
        printf("Sending long %lu\n", x);
    }
    send_byte(x & 0xFF);
    send_byte((x >> 8) & 0xFF);
    send_byte((x >> 16) & 0xFF);
    send_byte((x >> 24) & 0xFF);
}

// Little-endian.
unsigned short receive_ushort() {
    unsigned char low = receive_byte();
    unsigned char high = receive_byte();

    return high*256 + low;
}

// Little-endian.
unsigned long receive_ulong() {
    long value = 0;

    value |= receive_byte() << 0;
    value |= receive_byte() << 8;
    value |= receive_byte() << 16;
    value |= receive_byte() << 24;

    return value;
}

void transform_and_light_vertex(world_vertex *wv, lit_vertex *lv)
{
    vec4f tv;
    vec3f normal;

    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&modelview_stack), wv->coord, tv);
    // matrix4x4f_print("modelview", matrix4x4f_stack_top(&modelview_stack));
#if 0
    vec3f_mult_matrix4x4f(wv->normal, modelview_stack.get_inverse(), normal);

    if(normalize_enabled)
        vec3f_normalize(normal, normal);

    if(color_material_enabled) {
        // XXX color_material mode
        vec4f_copy(current_material.diffuse, lv->color);
        vec4f_copy(current_material.ambient, lv->color);
    }
    
    if(lighting_enabled) {
        light_vertex(&current_material, tv, normal, lv->color);
    } else {
        vec4f_copy(lv->color, wv->color);
    }
#else
    lv->color[0] = current_color[0]/255.0;
    lv->color[1] = current_color[1]/255.0;
    lv->color[2] = current_color[2]/255.0;
    lv->color[3] = current_color[3]/255.0;
#endif

    /// XXX could multiply mv and p together?
    matrix4x4f_mult_vec4f(matrix4x4f_stack_top(&projection_stack), tv, lv->coord);
    // matrix4x4f_print("projection", matrix4x4f_stack_top(&projection_stack));
    // vec4f_print("Object", wv->coord);
    // vec4f_print("World", tv);
    // vec4f_print("Screen", lv->coord);
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
    send_ushort(sv->x);
    send_ushort(sv->y);
    send_byte(sv->r);
    send_byte(sv->g);
    send_byte(sv->b);
    send_byte(sv->a);
}

int indent = 0;

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
        MULTMATRIX,
        PERSPECTIVE,
        WINDOW,
        VIEWPORT,
    } type;

    union
    {
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
        } translate;

        struct {
            Matrix m;
        } multmatrix;

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

void backface() {
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
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
                case MULTMATRIX:
                    multmatrix(p->multmatrix.m);
                    break;
                case TRANSLATE:
                    translate(p->translate.x, p->translate.y, p->translate.z);
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


// XXX display list
void clear() { 
    if(trace_functions) printf("%*sclear\n", indent, "");
    send_byte(1);
    for(int i = 0; i < 3; i++)
        send_byte(current_color[i]);
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
            current_color[i] = colormap[color][i];
            // XXX alpha in color map?
    }
}

void deflinestyle() { 
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }
}

void	defpattern(long index, short size, short *mask) {
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
    send_byte(4);
    send_ulong(device);
    return receive_ulong();
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
        matrix4x4f_stack_mult(&modelview_stack, (float *)m);
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
#if 0
        printf("projection matrix:\n");
        for(int i = 0 ; i < 4; i++)
            printf("    %f %f %f %f\n",
                m[i * 4 + 0],
                m[i * 4 + 1],
                m[i * 4 + 2],
                m[i * 4 + 3]);
#endif
        matrix4x4f_stack_load(&projection_stack, m);
        matrix4x4f_stack_load(&modelview_stack, identity_4x4f);
        if(trace_functions) printf("%*sperspective %d %f %f %f\n", indent, "", fovy_, aspect, near, far);
        matrix4x4f_stack_load(&projection_stack, m);
    }
}

#define POLY_MAX 16

void process_triangle(world_vertex *w0, world_vertex *w1, world_vertex *w2)
{
    lit_vertex l0, l1, l2;
    screen_vertex s0, s1, s2;

    transform_and_light_vertex(w0, &l0);
    transform_and_light_vertex(w1, &l1);
    transform_and_light_vertex(w2, &l2);

    project_vertex(&l0, &s0);
    project_vertex(&l1, &s1);
    project_vertex(&l2, &s2);

    send_byte(3);
    send_screen_vertex(&s0);
    send_screen_vertex(&s1);
    send_screen_vertex(&s2);
}

void process_polygon(long n, world_vertex *worldverts)
{
    // Clip polygon to view volume
    int i0, i1, i2;

    i1 = 0;
    i2 = n - 1;

    // XXX IrisGL polys can be concave; not handled.  see concave()
    for(int i = 0; i < n - 2; i++) {
        i0 = i1;
        i1 = i2;
        // This next one means 3rd vertex alternates back and forth
        // across polygon, basically turning it into a triangle strip
        // A fan might be slightly clearer
        i2 = (i % 2 == 0) ? (1 + i / 2) : (n - 2 - i / 2);

        process_triangle(&worldverts[i0], &worldverts[i1], &worldverts[i2]);
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
        vec4f_set(color, current_color[0] / 255.0, current_color[1] / 255.0, current_color[2] / 255.0, current_color[3] / 255.0);

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
        vec4f_set(color, current_color[0] / 255.0, current_color[1] / 255.0, current_color[2] / 255.0, current_color[3] / 255.0);

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
        matrix4x4f_stack_pop(&modelview_stack);
    }
}

void pushmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e = element_next_in_object();
        e->type = PUSHMATRIX;
    } else {
        if(trace_functions) printf("%*spushmatrix\n", indent, "");
        matrix4x4f_stack_push(&modelview_stack);
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
    static int warned = 0; if(!warned) { printf("%s unimplemented\n", __FUNCTION__); warned = 1; }

    switch (device) {
        case REDRAW:
            // Initial redraw.
            enqueue_device(REDRAW, 0);
            break;

        case INPUTCHANGE:
            // Tell app that an input device is ready.
            enqueue_device(INPUTCHANGE, 1);
            break;

        default:
            // Nothing to do.
            break;
    }
}

// If the queue is empty, qread() blocks.
long qread(short *val) { 
    if (input_queue_length == 0) {
        // We don't support blocking.
        printf("qread() blocking unimplemented.");
        exit(1);
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
        matrix4x4f_stack_mult(&modelview_stack, m);
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
    send_byte(2);
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

        matrix4x4f_copy(m, identity_4x4f);
        m[12] = x;
        m[13] = y;
        m[14] = z;

        if(trace_functions) printf("%*stranslate %f %f %f\n", indent, "", x, y, z);
        matrix4x4f_stack_mult(&modelview_stack, m);
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
    send_byte(0);
    send_string(title);
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
    the_viewport[4] = 0.0;
    the_viewport[5] = 1.0;
}
