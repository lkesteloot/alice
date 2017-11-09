#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "i2c_utility.h"
#include "accelerometer.h"

#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

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

static const char* devname = "/dev/i2c-0";

static int accelerometer_fd;

static void ADXL345_init(int fd)
{ 
    write_u8(fd, ADXL345_DATA_FORMAT, 0x0B); // +-16g, 13-bit
    write_u8(fd, ADXL345_POWER_CTL, 0x08); // start measurement
    //write_u8(fd, ADXL345_INT_ENABLE, 0x80); // enable DATA_READY interrupt
}

int accelerometer_init()
{
    accelerometer_fd = open(devname, O_RDWR);
    if (accelerometer_fd < 0) {
        perror("open");
	return 0;
    }

    int addr = ADXL345_ADDRESS; /* The I2C address */

    if (ioctl(accelerometer_fd, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	return 0;
    }

    ADXL345_init(accelerometer_fd);
    return 1;
}

void accelerometer_read(float *theta_y, float *theta_x)
{
    float xg = accelerometer_read_x() / 256.0;
    float yg = accelerometer_read_y() / 256.0;
    float zg = accelerometer_read_z() / 256.0;

    *theta_y = -atan2(yg, zg);
    *theta_x = atan2(xg, zg);
}

int accelerometer_read_x()
{
    return (int16_t) read_u16_le(accelerometer_fd, ADXL345_DATAX0);
}

int accelerometer_read_y()
{
    return (int16_t) read_u16_le(accelerometer_fd, ADXL345_DATAY0);
}

int accelerometer_read_z()
{
    return (int16_t) read_u16_le(accelerometer_fd, ADXL345_DATAZ0);
}

