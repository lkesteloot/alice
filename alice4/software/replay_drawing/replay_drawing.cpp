#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

extern "C" {
#include "rasterizer.h"
};

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "usage: %s capturefile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        fprintf(stderr, "couldn't open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    static char command[16];
    static char line[1024];
    while(fscanf(fp, " %s ", command) == 1) {
        if(strcmp(command, "#") == 0) {
            fgets(line, sizeof(line), fp);
        } else if(strcmp(command, "winopen") == 0) {
            fgets(line, sizeof(line), fp);
            rasterizer_winopen(line);
        } else if(strcmp(command, "zbuffer") == 0) {
            int e;
            fscanf(fp, "%d ", &e);
            rasterizer_zbuffer(e);
        } else if(strcmp(command, "pattern") == 0) {
            int e;
            fscanf(fp, "%d ", &e);
            rasterizer_pattern(e);
        } else if(strcmp(command, "linewidth") == 0) {
            float w;
            fscanf(fp, "%f ", &w);
            rasterizer_linewidth(w);
        } else if(strcmp(command, "setpattern") == 0) {
            uint16_t pattern[16];
            for(int i = 0; i < 16; i++)
                fscanf(fp, "%hx ", &pattern[i]);
            rasterizer_setpattern(pattern);
        } else if(strcmp(command, "swap") == 0) {
            rasterizer_swap();
        } else if(strcmp(command, "clear") == 0) {
            int r, g, b;
            fscanf(fp, "%d %d %d ", &r, &g, &b);
            rasterizer_clear(r, g, b);
        } else if(strcmp(command, "zclear") == 0) {
            uint32_t z;
            fscanf(fp, "%u ", &z);
            rasterizer_zclear(z);
        } else if(strcmp(command, "draw") == 0) {
            static screen_vertex v[4096];
            int type, count;
            fscanf(fp, "%d %d ", &type, &count);
            for(int i = 0; i < count; i++)
                fscanf(fp, "%hu %hu %u %hhu %hhu %hhu ", &v[i].x, &v[i].y, &v[i].z, &v[i].r, &v[i].g, &v[i].b);
            rasterizer_draw(type, count, v);
        } else if(strcmp(command, "bitmap") == 0) {
            uint32_t width, rowbytes, height;
            screen_vertex v;
            static unsigned char bits[1024 * 1024];
            fscanf(fp, "%u %u %u ", &width, &rowbytes, &height);
            fscanf(fp, "%hu %hu %u %hhu %hhu %hhu ", &v.x, &v.y, &v.z, &v.r, &v.g, &v.b);
            for(int j = 0; j < height; j++)
                for(int i = 0; i < rowbytes; i++)
                    fscanf(fp, "%hhX ", &bits[j * rowbytes + i]);
            rasterizer_bitmap(width, rowbytes, height, &v, bits);
        }
    }
}

