#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FPGA_MANAGER_BASE 0xFF706000
#define FPGA_GPO_OFFSET 0x10
#define FPGA_GPI_OFFSET 0x14

int main()
{
    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned char *mem = (unsigned char *)mmap(0, 64, PROT_READ | PROT_WRITE, /* MAP_NOCACHE | */ MAP_SHARED , dev_mem, FPGA_MANAGER_BASE);
    if(mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    volatile unsigned long *gpo = (unsigned long*)(mem + FPGA_GPO_OFFSET);

    int direction = 0;
    int value = 0;

    while (1) {
	*gpo = (value << 2) | (1 << 1);

	if (direction) {
	    if (value == 1000) {
		direction = 0;
	    } else {
		value++;
	    }
	} else {
	    if (value == 0) {
		direction = 1;
	    } else {
		value--;
	    }
	}

	usleep(1000);
    }

    exit(EXIT_SUCCESS);
}
