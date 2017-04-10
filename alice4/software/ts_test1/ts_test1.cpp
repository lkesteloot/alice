#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Cribbed from https://www.kernel.org/doc/Documentation/i2c/dev-interface

const char* devname = "/dev/i2c-1";

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

int main()
{
    int file;
    unsigned char buf[5];
  
    file = open(devname, O_RDWR);
    if (file < 0) {
        perror("open");
	exit(EXIT_FAILURE);
    }

    int addr = 0x41; /* The I2C address */

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	exit(EXIT_FAILURE);
    }

    uint16_t id = read_u16(file, 0x00);

    printf("id is 0x%04X\n", id);

    while(1) {
	uint8_t tsc_ctrl = read_u8(file, 0x40);
	uint8_t fifo_size = read_u8(file, 0x4C);
	uint8_t d0 = read_u8(file, 0xD7);
	uint8_t d1 = read_u8(file, 0xD7);
	uint8_t d2 = read_u8(file, 0xD7);
	uint8_t d3 = read_u8(file, 0xD7);
	write_u8(file, 0x0b, 0xff);

	usleep(100000);

	int x = (d0 << 4) | (d1 >> 4);
	int y = ((d1 && 0xF) << 8) | d2;
	int z = d3;

	printf("%02x %02x %d %d %d\n", tsc_ctrl, fifo_size, x, y, z);
    }
}
