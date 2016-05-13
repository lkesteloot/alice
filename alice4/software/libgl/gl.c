
// Alice 4 libgl implementation.

#include <gl.h>

static long DISPLAY_WIDTH = 800;
static long DISPLAY_HEIGHT = 600;

void backface() {
}

void callobj() { 
}

void clear() { 
}

void closeobj() { 
}

void color() { 
}

void deflinestyle() { 
}

void defpattern() {
}

void doublebuffer() { 
}

void editobj() { 
}

void frontbuffer() { 
}

void gconfig() { 
}

Object genobj() { 
}

Tag gentag() { 
}

Boolean getbutton() { 
}

void getmcolor() { 
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
}

void gflush() {
}

void glcompat() {
}

void gl_sincos() {
}

void gexit() { 
}

void makeobj() { 
}

void maketag() { 
}

void mapcolor() { 
}

void multmatrix() { 
}

void objreplace() { 
}

void perspective() { 
}

void polf() { 
}

void polf2i() { 
}

void popmatrix() { 
}

void pushmatrix() { 
}

// We're interested in events from this device.
void qdevice(long device) { 
}

long qread() { 
}

// Non-zero if there's something in the queue. Not sure if that's
// TRUE or the queue content.
long qtest() { 
    return FALSE;
}

void reshapeviewport() { 
}

void rotate() { 
}

void setpattern() { 
}

void shademodel() {
}

void swapbuffers() { 
}

void translate() { 
}

void window() { 
}

long winopen() { 
}

