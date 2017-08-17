#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Increase resolution (move decimal point)
// Calibrate?
// Convert to degrees?

#define ADXL345_ADDRESS 0x53

#define ADXL345_THRESH_TAP 0x1D
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

const char* devname = "/dev/i2c-0";

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

uint16_t read_u16_le(int fd, int reg)
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
    return (buf[1] << 8) | buf[0];
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

void ADXL345_init(int fd)
{ 
    write_u8(fd, ADXL345_DATA_FORMAT, 0x0B); // +-16g, 13-bit
    write_u8(fd, ADXL345_POWER_CTL, 0x08); // start measurement
    //write_u8(fd, ADXL345_INT_ENABLE, 0x80); // enable DATA_READY interrupt
}

int main()
{
    int file;
  
    file = open(devname, O_RDWR);
    if (file < 0) {
        perror("open");
	exit(EXIT_FAILURE);
    }

    int addr = ADXL345_ADDRESS; /* The I2C address */

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	exit(EXIT_FAILURE);
    }

    uint16_t id = read_u16(file, 0x00);

    printf("id is 0x%04X\n", id);

    ADXL345_init(file);

    while(1) {
	int16_t xdata = read_u16_le(file, ADXL345_DATAX0);
	int16_t ydata = read_u16_le(file, ADXL345_DATAY0);
	int16_t zdata = read_u16_le(file, ADXL345_DATAZ0);
	float xg = xdata / 256.0;
	float yg = ydata / 256.0;
	float zg = zdata / 256.0;
	printf("%f, %f, %f\n", xg, yg, zg);
	float theta_y = atan2(yg, zg);
	float theta_x = atan2(xg, zg);
	printf("    %f, %f\n", theta_y / M_PI * 180.0, theta_x / M_PI * 180.0);
	usleep(100000);
    }
}
