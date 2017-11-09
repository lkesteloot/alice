#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "i2c_utility.h"
#include "gyro.h"

// Device address.
#define L3GD20H_ADDRESS 0x6A

// Addresses.
#define L3GD20H_WHO_AM_I           0x0F
#define L3GD20H_CTRL_REG1          0x20
#define L3GD20H_CTRL_REG2          0x21
#define L3GD20H_CTRL_REG3          0x22
#define L3GD20H_CTRL_REG4          0x23
#define L3GD20H_CTRL_REG5          0x24
#define L3GD20H_REFERENCE          0x25
#define L3GD20H_OUT_TEMP           0x26
#define L3GD20H_STATUS_REG         0x27
#define L3GD20H_OUT_X_L            0x28
#define L3GD20H_OUT_X_H            0x29
#define L3GD20H_OUT_Y_L            0x2A
#define L3GD20H_OUT_Y_H            0x2B
#define L3GD20H_OUT_Z_L            0x2C
#define L3GD20H_OUT_Z_H            0x2D
#define L3GD20H_FIFO_CTRL_REG      0x2E
#define L3GD20H_FIFO_SRC_REG       0x2F
#define L3GD20H_INT1_CFG           0x30
#define L3GD20H_INT1_SRC           0x31
#define L3GD20H_TSH_XH             0x32
#define L3GD20H_TSH_XL             0x33
#define L3GD20H_TSH_YH             0x34
#define L3GD20H_TSH_YL             0x35
#define L3GD20H_TSH_ZH             0x36
#define L3GD20H_TSH_ZL             0x37
#define L3GD20H_INT1_DURATION      0x38

#define L3GD20H_DEVICE_ID          0xD7

static const char *DEVICE_NAME = "/dev/i2c-1";

static int gyro_fd;

static void L3GD20H_init(int fd)
{ 
    // Reset.
    write_u8(fd, L3GD20H_CTRL_REG1, 0x00);

    // 250dps, 8.75mdps resolution.
    write_u8(fd, L3GD20H_CTRL_REG4, 0x00);

    // Enable the FIFO.
    write_u8(fd, L3GD20H_CTRL_REG5, 0x52);

    // Power on.
    write_u8(fd, L3GD20H_CTRL_REG1, 0x0F);
}

int gyro_init()
{
    gyro_fd = open(DEVICE_NAME, O_RDWR);
    if (gyro_fd < 0) {
        perror("open for gyro");
	return 0;
    }

    int addr = L3GD20H_ADDRESS; /* The I2C address */

    if (ioctl(gyro_fd, I2C_SLAVE, addr) < 0) {
        perror("ioctl for gyro");
	return 0;
    }

    int device_id = read_u8(gyro_fd, L3GD20H_WHO_AM_I);
    if (device_id != L3GD20H_DEVICE_ID) {
	fprintf(stderr, "Wrong gyro device id: 0x%x\n", device_id);
	return 0;
    }

    L3GD20H_init(gyro_fd);

    return 1;
}

int gyro_ready()
{
    int status = read_u8(gyro_fd, L3GD20H_STATUS_REG);
    return (status & 0x08) != 0;
}

void gyro_read(int *dx, int *dy, int *dz)
{
    int16_t xdata = read_u16_le(gyro_fd, L3GD20H_OUT_X_L | 0x80);
    int16_t ydata = read_u16_le(gyro_fd, L3GD20H_OUT_Y_L | 0x80);
    int16_t zdata = read_u16_le(gyro_fd, L3GD20H_OUT_Z_L | 0x80);

    *dx = xdata;
    *dy = ydata;
    *dz = zdata;
}

int gyro_overrun()
{
    int status = read_u8(gyro_fd, L3GD20H_STATUS_REG);
    return (status & 0x80) != 0;
}

