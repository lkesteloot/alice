#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "basic_types.h"

static const int DRAW_TRIANGLES = 0;
static const int DRAW_LINES = 1;
static const int DRAW_POINTS = 2;
static const int DRAW_LINE_STRIP = 3;
static const int DRAW_LINE_LOOP = 4;
static const int DRAW_TRIANGLE_STRIP = 5;
static const int DRAW_TRIANGLE_FAN = 6;

int32_t rasterizer_winopen(char *title);

void rasterizer_clear(uint8_t r, uint8_t g, uint8_t b);
void rasterizer_zclear(uint32_t z);
void rasterizer_swap();
void rasterizer_draw(uint32_t type, uint32_t count, screen_vertex *v); // count is the number of primitives.
void rasterizer_bitmap(uint32_t width, uint32_t rowbytes, uint32_t height, screen_vertex *sv, uint8_t *bits);
void rasterizer_setpattern(uint16_t pattern[16]);
void rasterizer_pattern(int enable);
void rasterizer_zbuffer(int enable);
void rasterizer_linewidth(float w);

#endif /* __RASTERIZER_H__ */