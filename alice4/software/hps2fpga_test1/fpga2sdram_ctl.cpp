#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

#define FPGA_BASE 0xFFC20000
#define FPGA_PORT_RESET 0x5080

void usage(char *argv[])
{
    fprintf(stderr, "Usage: %s [reset|enable]\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
	usage(argv);
    }
    char *command = argv[1];

    int dev_mem = open("/dev/mem", O_RDWR);

    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned char *mem = (unsigned char *)mmap(0, FPGA_PORT_RESET + 4, PROT_READ | PROT_WRITE, /* MAP_NOCACHE | */ MAP_SHARED , dev_mem, FPGA_BASE);

    if(mem == 0) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    volatile uint32_t *fpgaportrst = (uint32_t *)(mem + FPGA_PORT_RESET);

    uint32_t value;
    if (strcmp(command, "reset") == 0) {
	value = 0;
    } else if (strcmp(command, "enable") == 0) {
	value = 0x111;
    } else {
	usage(argv);
    }

    *fpgaportrst = value;

    exit(EXIT_SUCCESS);
}
