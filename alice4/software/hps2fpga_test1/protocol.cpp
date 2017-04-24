#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

#define FPGA_MANAGER_BASE 0xFF706000
#define FPGA_GPO_OFFSET 0x10
#define FPGA_GPI_OFFSET 0x14

// HPS-to-FPGA:
#define H2F_DATA_READY (1 << 0)

// FPGA-to-HPS:
#define F2H_BUSY (1 << 0)

int main()
{
    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    uint8_t *mem = (uint8_t *) mmap(0, 64, PROT_READ | PROT_WRITE,
	MAP_SHARED , dev_mem, FPGA_MANAGER_BASE);
    if(mem == 0) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    volatile uint32_t *gpo = (uint32_t*)(mem + FPGA_GPO_OFFSET);
    volatile uint32_t *gpi = (uint32_t*)(mem + FPGA_GPI_OFFSET);

    *gpo = 0;

    while (1) {
	// Start of frame.

	// Write protocol buffer.
	printf("Writing protocol buffer.\n");
	// (Nothing yet.)

	// Tell FPGA that our data is ready.
	printf("Telling FPGA that the data is ready.\n");
	*gpo |= H2F_DATA_READY;

	// Wait until we find out that it has heard us.
	printf("Waiting for FPGA to get busy.\n");
	while ((*gpi & F2H_BUSY) == 0) {
	    // Busy loop.
	}

	// Let FPGA know that we know that it's busy.
	printf("Telling FPGA that we know it's busy.\n");
	*gpo &= ~H2F_DATA_READY;

	// Wait until it's done rasterizing.
	printf("Waiting for FPGA to finish rasterizing.\n");
	while ((*gpi & F2H_BUSY) != 0) {
	    // Busy loop.
	}
    }

    exit(EXIT_SUCCESS);
}

