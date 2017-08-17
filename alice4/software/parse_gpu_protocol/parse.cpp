#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <stdint.h>

// Protocol command number:
#define CMD_CLEAR 1
#define CMD_ZCLEAR 2
#define CMD_PATTERN 3
#define CMD_DRAW 4
#define CMD_BITMAP 5
#define CMD_SWAP 6
#define CMD_END 7

// Draw type:
#define DRAW_TRIANGLES 0
#define DRAW_LINES 1
#define DRAW_POINTS 2
#define DRAW_LINE_STRIP 3
#define DRAW_TRIANGLE_STRIP 5
#define DRAW_TRIANGLE_FAN 6

#define MAX_WORDS 1000000 // 300K triangles, also more than max BITMAP

int frame_number = 0;
bool next_is_first_in_frame = true;

void new_frame()
{
    next_is_first_in_frame = true;
}

void check_new_frame()
{
    if(next_is_first_in_frame) {
        printf("# frame %d\n", frame_number++);
        next_is_first_in_frame = false;
    }
}

void parse_vertex(uint64_t v, unsigned int *x, unsigned int *y, unsigned int *z, unsigned char *r, unsigned char *g, unsigned char *b)
{
    *x = ((v >> 0) & 0xFFF) * 8;
    *y = ((v >> 12) & 0xFFF) * 4;
    *z = (v >> 24) & 0xFFF;
    *r = (v >> 56) & 0xFF;
    *g = (v >> 48) & 0xFF;
    *b = (v >> 40) & 0xFF;
}

void parse_PATTERN(uint64_t command_quad, uint64_t pattern[])
{
    printf("setpattern");
    for (int i = 0; i < 16; i++)
        printf(" %04llX", (pattern[i / 4] >> ((i % 4) * 16)) & 0xFFFF);
    printf("\n");
}

void parse_DRAW(uint64_t command_quad, uint64_t vertex_data[])
{
    static int prev_zbuffer = 0;
    static int prev_pattern = 0;
    check_new_frame();

    unsigned int type = (command_quad >> 8) & 0xFF;
    int primcount = (command_quad >> 16) & 0xFFFF;
    int enable_zbuffer = (command_quad >> 32) & 0x1;
    int enable_pattern = (command_quad >> 33) & 0x1;

    switch(type)
    {
        if(prev_zbuffer != enable_zbuffer)
            printf("zbuffer %d\n", enable_zbuffer);
        prev_zbuffer = enable_zbuffer;
        if(prev_pattern != enable_pattern)
            printf("pattern %d\n", enable_pattern);
        prev_pattern = enable_pattern;
        case DRAW_TRIANGLES:
            printf("draw %d %d # zbuffer %s, pattern %s, %d TRIANGLES\n",
                type, primcount,
                enable_zbuffer ? "enabled" : "disabled",
                enable_pattern ? "enabled" : "disabled",
                primcount);
            for(int i = 0; i < primcount; i++) {
                for(int j = 0; j < 3; j++) {
                    unsigned int x, y, z;
                    unsigned char r, g, b;
                    parse_vertex(vertex_data[i * 3 + j], &x, &y, &z, &r, &g, &b);
                    printf("        %u %u %u %u %u %u # %f %f\n", x, y, z * 65535, r, g, b, x / 32.0, y / 32.0);
                }
            }
            break;

#if 0
        case DRAW_LINES:
            break;

        case DRAW_POINTS:
            break;

        case DRAW_LINE_STRIP:
            break;

        case DRAW_TRIANGLE_STRIP:
            break;

        case DRAW_TRIANGLE_FAN:
            break;
#endif

        default:
            fprintf(stderr, "unexpected draw type %u in command 0x%08llX in command stream\n", type, command_quad);
            exit(EXIT_FAILURE);
            break;
    }
}

void parse_ZCLEAR(uint64_t command_quad)
{
    check_new_frame();

    unsigned short z = (command_quad >> 16) & 0xFFFF;
    printf("zclear %u\n", z);
}

void parse_CLEAR(uint64_t command_quad)
{
    check_new_frame();

    unsigned char r = (command_quad >> 56) & 0xFF;
    unsigned char g = (command_quad >> 48) & 0xFF;
    unsigned char b = (command_quad >> 40) & 0xFF;
    printf("clear %u %u %u\n", r, g, b);
}

void parse_SWAP(uint64_t command_quad)
{
    printf("swap\n");
    new_frame();
}

void parse_END(uint64_t command_quad)
{
    check_new_frame();

    printf("end\n");
}

int quad_count_PATTERN(uint64_t command_quad)
{
    return 4;
}

int quad_count_DRAW(uint64_t command_quad)
{
    unsigned int type = (command_quad >> 8) & 0xFF;
    int primcount = (command_quad >> 16) & 0xFFFF;
    int vertexcount = 0;

    switch(type)
    {
        case DRAW_TRIANGLES:
            vertexcount = primcount * 3;
            break;

        case DRAW_LINES:
            vertexcount = primcount * 2;
            break;

        case DRAW_POINTS:
            vertexcount = primcount * 1;
            break;

        case DRAW_LINE_STRIP:
            vertexcount = primcount + 1;
            break;

        case DRAW_TRIANGLE_STRIP:
            vertexcount = primcount + 2;
            break;

        case DRAW_TRIANGLE_FAN:
            vertexcount = primcount + 2;
            break;

        default:
            fprintf(stderr, "unexpected draw type %u in command 0x%08llX in command stream\n", type, command_quad);
            exit(EXIT_FAILURE);
            break;
    }

    return vertexcount;
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "usage: %s protocol-output\n", argv[0]);
        fprintf(stderr, "\noutput must be a series of 64-bit numbers containing a GPU command stream\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        fprintf(stderr, "couldn't open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("winopen protocol\n");

    enum { NEW_COMMAND, GATHERING } state = NEW_COMMAND;

    static uint64_t more_quads[MAX_WORDS];
    int next_quad;
    int quad_count;

    uint64_t command_quad;

    static char word[16];
    static char line[1024];
    uint64_t quad;

    while(fscanf(fp, " %s ", word) == 1) {
        if(strcmp(word, "#") == 0) {
            fgets(line, sizeof(line), fp);
            printf("# %s\n", line);
            continue;
        }

        char *endptr;
        quad = strtoull(word, &endptr, 0);
        if(*endptr != '\0') {
            fprintf(stderr, "failed to parse \"%s\" as a number\n", word);
            exit(EXIT_FAILURE);
        }

        if(state == NEW_COMMAND) {
            command_quad = quad;
            switch(command_quad & 0xF) {
                case CMD_CLEAR:
                    parse_CLEAR(command_quad);
                    break;

                case CMD_ZCLEAR:
                    parse_ZCLEAR(command_quad);
                    break;

                case CMD_PATTERN:
                    quad_count = quad_count_PATTERN(command_quad);
                    next_quad = 0;
                    state = GATHERING;
                    break;

                case CMD_DRAW:
                    quad_count = quad_count_DRAW(command_quad);
                    next_quad = 0;
                    state = GATHERING;
                    break;

                case CMD_BITMAP:
                    fprintf(stderr, "parsed BITMAP, not yet implemented\n");
                    exit(EXIT_FAILURE);
                    break;

                case CMD_SWAP:
                    parse_SWAP(command_quad);
                    break;

                case CMD_END:
                    parse_END(command_quad);
                    break;

                default:
                    fprintf(stderr, "unexpected word 0x%08llX in command stream\n", command_quad);
                    exit(EXIT_FAILURE);
                    break;
            }
        } else if(state == GATHERING) {
            more_quads[next_quad++] = quad;
            if(--quad_count == 0) {
                switch(command_quad & 0xF) {
                    case CMD_PATTERN:
                        parse_PATTERN(command_quad, more_quads);
                        break;

                    case CMD_DRAW:
                        parse_DRAW(command_quad, more_quads);
                        break;

                    // case CMD_BITMAP:
                }
                state = NEW_COMMAND;
            }
        }
    }
}

