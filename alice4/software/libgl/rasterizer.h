#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "basic_types.h"

void rasterizer_winopen(char *title);

void rasterizer_clear(uint32_t c);
void rasterizer_zclear(uint32_t z);
void rasterizer_swap();
void rasterizer_draw(int type, int count, screen_vertex *v);
void rasterizer_bitmap(int w, int h, screen_vertex *v, unsigned char *bitmap);
void rasterizer_setpattern(uint16_t pattern[16]);
void rasterizer_pattern(int enable);
void rasterizer_zbuffer(int enable);

#endif /* __RASTERIZER_H__ */
