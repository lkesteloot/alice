#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "i2c_utility.h"

uint32_t read_u32(int fd, int reg)
{
    unsigned char buf[4];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 4) != 4) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

uint16_t read_u16(int fd, int reg)
{
    unsigned char buf[2];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 2) != 2) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return (buf[0] << 8) | buf[1];
}

uint8_t read_u8(int fd, int reg)
{
    unsigned char buf[1];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 1) != 1) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return buf[0];
}

void write_u16(int fd, int reg, uint16_t value)
{
    unsigned char buf[3];
    buf[0] = reg;
    buf[1] = value >> 8;
    buf[2] = value & 0xFF;
    if (write(fd, buf, 3) != 3) {
        fprintf(stderr, "write register number and value failed\n");
	exit(EXIT_FAILURE);
    }
}

void write_u8(int fd, int reg, uint8_t value)
{
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    if (write(fd, buf, 2) != 2) {
        fprintf(stderr, "write register number and value failed\n");
	exit(EXIT_FAILURE);
    }
}

