#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
// sudo apt-get install libfreeimageplus-dev
#include <FreeImagePlus.h>
#include <sys/mman.h>
#include <stdint.h>

void usage(char *argv[])
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: %s buffer0imagename buffer1imagename\n", argv[0]);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

static const unsigned int FRAMEBUFFER_WIDTH = 800;
static const unsigned int FRAMEBUFFER_HEIGHT = 480;
static const unsigned int FRAMEBUFFER_ADDRESS = 0x38000000;
static const unsigned int BYTES_PER_ROW = 3200;
static const unsigned int BYTES_PER_PIXEL = 4;

void get_image(unsigned char *fb, fipImage *img)
{
    for (unsigned int j = 0; j < FRAMEBUFFER_HEIGHT; j++) {

        unsigned char* src = fb + BYTES_PER_ROW * (480 - 1 - j);
        unsigned char* dst = img->accessPixels() + img->getScanWidth() * j;

        for (unsigned int i = 0; i < FRAMEBUFFER_WIDTH; i++) {

            unsigned char *srcp = src + i * BYTES_PER_PIXEL;
            unsigned char *dstp = dst + i * 3;

            dstp[0] = srcp[2];
            dstp[1] = srcp[1];
            dstp[2] = srcp[0];
        }
    }
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        usage(argv);
    }

    FreeImage_Initialise();

    const char *filename0 = argv[1];
    const char *filename1 = argv[2];

    fipImage img(FIT_BITMAP, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 24);

    int dev_mem = open("/dev/mem", O_RDWR);

    if(dev_mem == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned char *fb = (unsigned char *)mmap(0, FRAMEBUFFER_HEIGHT * BYTES_PER_ROW * 2, PROT_READ | PROT_WRITE, /* MAP_NOCACHE | */ MAP_SHARED , dev_mem, FRAMEBUFFER_ADDRESS);

    if(fb == 0) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    get_image(fb, &img);
    if (!img.save(filename0)) {
        std::cerr << "failed to save image " << filename0 << std::endl;
        exit(EXIT_FAILURE);
    }

    get_image(fb + FRAMEBUFFER_HEIGHT * BYTES_PER_ROW, &img);
    if (!img.save(filename1)) {
        std::cerr << "failed to save image " << filename1 << std::endl;
        exit(EXIT_FAILURE);
    }
}
