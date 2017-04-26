#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FPGA_SHARED_QWORD 0x38000000

int main()
{
    int dev_mem = open("/dev/mem", O_RDWR);

    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    unsigned char *mem = (unsigned char *)mmap(0, 64, PROT_READ | PROT_WRITE, /* MAP_NOCACHE | */ MAP_SHARED , dev_mem, FPGA_SHARED_QWORD);

    if(mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    volatile unsigned long long *shared = (unsigned long long*)(mem + 0);

    while(1) {
	*shared = 0xC0DEC0DEC0DEC0DEll;
	sleep(1);
	*shared = 0x0DEC0DEC0DEC0DECll;
	sleep(1);
	*shared = 0xDEC0DEC0DEC0DEC0ll;
	sleep(1);
	*shared = 0xEC0DEC0DEC0DEC0Dll;
	sleep(1);
    }

    exit(EXIT_SUCCESS);
}
