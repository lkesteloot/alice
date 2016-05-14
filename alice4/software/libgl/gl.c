
// Alice 4 libgl implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gl.h>
#include "connection.h"

typedef float matrix4x4f[16];
typedef float vec4f[4];
typedef float vec3f[3];
typedef unsigned short vec3us[3];
typedef unsigned char vec3ub[3];

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

#define STACK_MAX 8

typedef struct matrix4x4f_stack
{
    matrix4x4f s[STACK_MAX];

    int top;

    int inverse_needs_calculation;
    matrix4x4f inverse;
} matrix4x4f_stack;

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
}

void matrix4x4f_stack_mult(matrix4x4f_stack *stack, const matrix4x4f m)
{
    matrix4x4f_mult_matrix4x4f(m, stack->s[stack->top], stack->s[stack->top]);
    stack->inverse_needs_calculation = 1;
}

matrix4x4f_stack modelview_stack;
matrix4x4f_stack projection_stack;

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;

int indent = 0;

/*
implementing a display-listable function:
    add to enum in struct element
    if parameters, add new struct to union
    if needed, add free()/dtor in element_free()
    in function, add preamble like one in rotate()
    in callobj, add case entry with actual call
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

// Send a string as a length and the bytes. Max string length is 255.
void send_string(char *s) {
    send_byte(strlen(s));
    while (*s != '\0') {
        send_byte(*s);
        s++;
    }
}

void backface() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void callobj(Object obj) { 
    if(cur_ptr_to_nextptr != NULL) {

        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = CALLOBJ;
        e->callobj.obj = obj;

    } else {

        printf("%*scallobj %ld\n", indent, "", obj);
        indent += 4;
        element *p = objects[obj];
        while(p) {
            switch(p->type) {
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
                    printf("%*stag %ld\n", indent, "", p->tag.tag);
                    break;
            }
            p = p->next;
        }
        indent -= 4;
    }
}

void clear() { 
    printf("%s unimplemented\n", __FUNCTION__);
    send_byte(1);
}

void closeobj() { 
    printf("closeobj\n");
    cur_ptr_to_nextptr = NULL;
}

void color(Colorindex color) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = COLOR;
        e->color.color = color;
    } else {
        printf("%*scolor %u\n", indent, "", color);
    }
}

void deflinestyle() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void defpattern() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void doublebuffer() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void editobj(Object obj) { 
    printf("editobj %ld\n", obj);
    cur_ptr_to_nextptr = &(objects[obj]);
    while(*cur_ptr_to_nextptr != NULL)
        cur_ptr_to_nextptr = &(*cur_ptr_to_nextptr)->next;
    replace_mode = 0;
}

void frontbuffer() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void gconfig() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

Object genobj() { 
    for(int i = 0; i < OBJ_MAX; i++)
        if(!object_allocation[i]) {
            object_allocation[i] = 1;
            objects[i] = NULL;
            printf("genobj -> %d\n", i);
            return i;
        }
    abort();
}

Tag gentag() { 
    for(int i = 0; i < OBJ_MAX; i++)
        if(!tag_allocation[i]) {
            tag_allocation[i] = 1;
            ptrs_to_tagptrs[i] = NULL;
            printf("gentag -> %d\n", i);
            return i;
        }
    abort();
}

Boolean getbutton() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void getmcolor() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void getorigin(long *x, long *y) { 
    *x = 0;
    *y = 0;
}

long getplanes() { 
}

void getsize(long *width, long *height) { 
    *width = DISPLAY_WIDTH;
    *height = DISPLAY_HEIGHT;
}

long getvaluator() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void gflush() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void glcompat() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void gl_sincos(Angle angle, float *s, float *c) {
    float a = angle / 1800.0 * M_PI;
    *s = sin(a);
    *c = cos(a);
}

void gexit() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void makeobj(Object obj) { 
    printf("makeobj %ld\n", obj);
    if(objects[obj] != NULL) {
        element_free(objects[obj]);
    }
    objects[obj] = NULL;
    cur_ptr_to_nextptr = &(objects[obj]);
    replace_mode = 0;
}

void maketag(Tag tag) { 
    printf("maketag %ld\n", tag);
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

void mapcolor() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void multmatrix(Matrix m) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = MULTMATRIX;
        memcpy(e->multmatrix.m, m, sizeof(Matrix));
    } else { 
        matrix4x4f_stack_mult(&modelview_stack, (float *)m);
        printf("%*smultmatrix\n", indent, "");
    }
}

void objreplace(Tag tag) { 
    printf("objreplace %ld\n", tag);
    cur_ptr_to_nextptr = &(*ptrs_to_tagptrs[tag])->next;
    replace_mode = 1;
}

void perspective(Angle fovy_, float aspect, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
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
        m[11] = 2 * far * near / (near - far);
        m[14] = -1.0;
        m[15] = 0.0;
        matrix4x4f_stack_load(&projection_stack, m);
        printf("%*sperspective %d %f %f %f\n", indent, "", fovy_, aspect, near, far);
    }
}

void polf(long n, Coord parray[ ][3]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = POLF;
        e->polf.n = n;
        e->polf.parray = (Coord*) malloc(sizeof(Coord) * 3 * n);
        memcpy(e->polf.parray, parray, sizeof(Coord) * 3 * n);
    } else {
        printf("%*spolf %ld\n", indent, "", n);
    }
}

void polf2i(long n, Icoord parray[ ][2]) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = POLF2I;
        e->polf2i.n = n;
        e->polf2i.parray = (Icoord*) malloc(sizeof(Icoord) * 2 * n);
        memcpy(e->polf.parray, parray, sizeof(Icoord) * 2 * n);
    } else {
        printf("%*spolf2i %ld\n", indent, "", n);
    }
}

void popmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = POPMATRIX;
    } else {
        printf("%s\n", __FUNCTION__);
        matrix4x4f_stack_pop(&modelview_stack);
    }
}

void pushmatrix() { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
        e->type = PUSHMATRIX;
    } else {
        printf("%s\n", __FUNCTION__);
        matrix4x4f_stack_push(&modelview_stack);
    }
}

// We're interested in events from this device.
void qdevice(long device) { 
    printf("%s unimplemented\n", __FUNCTION__);
}

long qread() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

// Non-zero if there's something in the queue. Not sure if that's
// TRUE or the queue content.
long qtest() { 
    printf("%s stub\n", __FUNCTION__);
    return FALSE;
}

void reshapeviewport() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void rotate(Angle ang, unsigned char axis) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
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

        matrix4x4f_stack_mult(&modelview_stack, m);
        printf("%*srotate %d %c\n", indent, "", ang, axis);
    }
}

void setpattern() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void shademodel() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void swapbuffers() { 
    printf("%s\n", __FUNCTION__);
    send_byte(2);
}

void translate(Coord x, Coord y, Coord z) {
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
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

        matrix4x4f_stack_mult(&modelview_stack, m);
        printf("%*stranslate %f %f %f\n", indent, "", x, y, z);
    }
}

void window(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far) { 
    if(cur_ptr_to_nextptr != NULL) {
        element *e;
        if(!replace_mode) {
            e = element_new();
            element_insert(&cur_ptr_to_nextptr, e);
            // XXX Need to skip tags?
        } else {
            e = *cur_ptr_to_nextptr;
        }
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
        matrix4x4f_stack_load(&projection_stack, m);

        printf("%*swindow %f %f %f %f %f %f\n", indent, "", left, right, bottom, top, near, far);
    }
}

long winopen(char *title) {
    printf("%s\n", __FUNCTION__);
    open_connection();
    send_byte(0);
    send_string(title);
}

static void init_gl_state() __attribute__((constructor));
static void init_gl_state()
{
    matrix4x4f_stack_load(&modelview_stack, identity_4x4f);
    matrix4x4f_stack_load(&projection_stack, identity_4x4f);
}
