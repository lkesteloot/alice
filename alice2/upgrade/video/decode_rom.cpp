#include <cstdio>
#include <cstdlib>
#include <unistd.h>

void output_color(FILE *fp, int r, int g, int b)
{
    fputc(r, fp);
    fputc(g, fp);
    fputc(b, fp);
}

struct palette_entry {
    int v;
    int r;
    int g;
    int b;
};

int main(int argc, char **argv)
{
    int width, height;
    unsigned char buffer[128 * 1024];
    size_t length;
    palette_entry palette[256];
    int entry_count = 0;

    if(argc < 3) {
        fprintf(stderr, "%s w h [xx=rrggbb [ yy=rrggbb [...]] < file.bin\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    for(int i = 3; i < argc ; i++) {
        palette_entry &e = palette[entry_count];
        sscanf(argv[i], "%2x=%2x%2x%2x", &e.v, &e.r, &e.g, &e.b);
        fprintf(stderr, "%02x = %02x, %02x, %02x\n", e.v, e.r, e.g, e.b);
        entry_count ++;
    }

    length = fread(buffer, 1, 128 * 1024, stdin);

    printf("P6 %d %d 255\n", width, height);
    unsigned char *p = buffer;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++, p++) {
            if(p - buffer < length) {
                bool found = false;
                for(int k = 0; k < entry_count; k++) {
                    palette_entry &e = palette[k];
                    if(e.v == *p) {
                        output_color(stdout, e.r, e.g, e.b);
                        found = true;
                        break;
                    }
                }
                if(!found)
                    output_color(stdout, 0, 0, 0);
            } else {
                output_color(stdout, 32, 0, 0);
            }
        }
}
