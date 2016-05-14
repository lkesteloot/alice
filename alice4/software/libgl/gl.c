
// Alice 4 libgl implementation.

#include <stdlib.h>
#include <gl.h>
#include "connection.h"

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;

int indent = 0;

typedef struct element
{
    enum
    {
        TAG,
        CALLOBJ,
        COLOR,
        POLF,
        POLF2I,
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
    printf("editobj\n");
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
    printf("genobj\n");
    for(int i = 0; i < OBJ_MAX; i++)
        if(!object_allocation[i]) {
            object_allocation[i] = 1;
            objects[i] = NULL;
            return i;
        }
    abort();
}

Tag gentag() { 
    printf("gentag\n");
    for(int i = 0; i < OBJ_MAX; i++)
        if(!tag_allocation[i]) {
            tag_allocation[i] = 1;
            ptrs_to_tagptrs[i] = NULL;
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

void gl_sincos() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void gexit() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void makeobj(Object obj) { 
    printf("makeobj\n");
    if(objects[obj] != NULL) {
        element_free(objects[obj]);
    }
    objects[obj] = NULL;
    cur_ptr_to_nextptr = &(objects[obj]);
    replace_mode = 0;
}

void maketag(Tag tag) { 
    printf("maketag\n");
    if(cur_ptr_to_nextptr == NULL)
        printf("maketag : not editing\n");
    element *e = element_new();
    e->type = TAG;
    e->tag.tag = tag;
    ptrs_to_tagptrs[tag] = cur_ptr_to_nextptr;
    element_insert(&cur_ptr_to_nextptr, e);
}

void mapcolor() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void multmatrix() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void objreplace(Tag tag) { 
    printf("objreplace\n");
    cur_ptr_to_nextptr = ptrs_to_tagptrs[tag];
    replace_mode = 1;
}

void perspective() { 
    printf("%s unimplemented\n", __FUNCTION__);
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
    printf("%s unimplemented\n", __FUNCTION__);
}

void pushmatrix() { 
    printf("%s unimplemented\n", __FUNCTION__);
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

void rotate() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void setpattern() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void shademodel() {
    printf("%s unimplemented\n", __FUNCTION__);
}

void swapbuffers() { 
    printf("%s unimplemented\n", __FUNCTION__);
    send_byte(0x45);
}

void translate() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

void window() { 
    printf("%s unimplemented\n", __FUNCTION__);
}

long winopen() { 
    printf("%s unimplemented\n", __FUNCTION__);
    open_connection();
}

