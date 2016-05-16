
#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

typedef float matrix4x4f[16];
typedef float vec4f[4];
typedef float vec3f[3];
typedef unsigned short vec3us[3];
typedef unsigned short vec4us[4];
typedef unsigned char vec3ub[3];
typedef unsigned char vec4ub[4];

typedef struct screen_vertex
{
    unsigned short x, y;
    unsigned long z;
    unsigned char r, g, b, a;
} screen_vertex;

#endif // __BASIC_TYPES_H__
