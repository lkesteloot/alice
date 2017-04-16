#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "touchscreen.h"

#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define STMPE610_ADDRESS 0x41

#define STMPE610_CHIP_ID 0x00
#define STMPE610_ID_VER 0x02
#define STMPE610_SYS_CTRL1 0x03
#define STMPE610_SYS_CTRL2 0x04
#define STMPE610_SPI_CFG 0x08
#define STMPE610_INT_CTRL 0x09
#define STMPE610_INT_EN 0x0A
#define STMPE610_INT_STA 0x0B
#define STMPE610_GPIO_EN 0x0C
#define STMPE610_GPIO_INT_STA 0x0D
#define STMPE610_ADC_INT_EN 0x0E
#define STMPE610_ADC_INT_STA 0x0F
#define STMPE610_GPIO_SET_PIN 0x10
#define STMPE610_GPIO_CLR_PIN 0x11
#define STMPE610_GPIO_MP_STA 0x12
#define STMPE610_GPIO_DIR 0x13
#define STMPE610_GPIO_ED 0x14
#define STMPE610_GPIO_RE 0x15
#define STMPE610_GPIO_FE 0x16
#define STMPE610_GPIO_AF 0x17
#define STMPE610_ADC_CTRL1 0x20
#define STMPE610_ADC_CTRL2 0x21
#define STMPE610_ADC_CAPT 0x22
#define STMPE610_ADC_DATA_CH0 0x30
#define STMPE610_ADC_DATA_CH1 0x32
#define STMPE610_ADC_DATA_CH4 0x38
#define STMPE610_ADC_DATA_CH5 0x3A
#define STMPE610_ADC_DATA_CH6 0x3C
#define STMPE610_ADC_DATA_CH7 0x3E
#define STMPE610_TSC_CTRL 0x40
#define STMPE610_TSC_CFG 0x41
#define STMPE610_WDW_TR_X 0x42
#define STMPE610_WDW_TR_Y 0x44
#define STMPE610_WDW_BL_X 0x46
#define STMPE610_WDW_BL_Y 0x48
#define STMPE610_FIFO_TH 0x4A
#define STMPE610_FIFO_STA 0x4B
#define STMPE610_FIFO_SIZE 0x4C
#define STMPE610_TSC_DATA_X 0x4D
#define STMPE610_TSC_DATA_Y 0x4F
#define STMPE610_TSC_DATA_Z 0x51
#define STMPE610_TSC_DATA_XYZ 0x52
#define STMPE610_TSC_FRACT_XYZ 0x56
#define STMPE610_TSC_DATA 0x57
#define STMPE610_TSC_DATA_NOAUTOINC 0xD7
#define STMPE610_TSC_I_DRIVE 0x58
#define STMPE610_TSC_SHIELD 0x59

// SYS_CTRL1
#define STMPE610_SOFT_RESET 0x02

const char* devname = "/dev/i2c-1";

static uint32_t read_u32(int fd, int reg)
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

static uint16_t read_u16(int fd, int reg)
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

static uint8_t read_u8(int fd, int reg)
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

static void write_u16(int fd, int reg, uint16_t value)
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

static void write_u8(int fd, int reg, uint8_t value)
{
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    if (write(fd, buf, 2) != 2) {
        fprintf(stderr, "write register number and value failed\n");
	exit(EXIT_FAILURE);
    }
}

static void STMPE610_init(int fd)
{ 
    write_u8(fd, STMPE610_SYS_CTRL1, STMPE610_SOFT_RESET);
    usleep(1000);
    write_u8(fd, STMPE610_SYS_CTRL1, 0);

    write_u8(fd, STMPE610_SYS_CTRL2, 0x0C);

    write_u8(fd, STMPE610_TSC_CTRL, 0x00);
    write_u8(fd, STMPE610_TSC_CTRL, 0x01);

    write_u8(fd, STMPE610_INT_EN, 0x01); // TOUCH_DETECT-Interrupt are enabled

    write_u8(fd, STMPE610_ADC_CTRL1, 0x60);
    write_u8(fd, STMPE610_ADC_CTRL2, 0x02);

    // write_u8(fd, STMPE610_GPIO_AF, 0x00);

    write_u8(fd, STMPE610_TSC_CFG, 0xA4);

    write_u8(fd, STMPE610_FIFO_TH, 0x01);
    write_u8(fd, STMPE610_FIFO_STA, 0x01); // Clear the FIFO
    write_u8(fd, STMPE610_FIFO_STA, 0x00); // FIFO back to operation mode

    write_u8(fd, STMPE610_TSC_FRACT_XYZ, 0x04);// Data Format for Z-Value
    write_u8(fd, STMPE610_TSC_I_DRIVE, 0x01); // Choose 20mA driving channel

    write_u8(fd, STMPE610_INT_STA, 0xFF); // Clear all interrupt status
    write_u8(fd, STMPE610_INT_CTRL, 0x05); // Enable the interrupts

    // define initial Touch Window
    // write_u16(fd, STMPE610_WDW_TR_X, 0x0FFF); // Top Right X
    // write_u16(fd, STMPE610_WDW_TR_Y, 0x0FFF); // Top Right Y
    // write_u16(fd, STMPE610_WDW_BL_X, 0x0000); // Bottom Left X
    // write_u16(fd, STMPE610_WDW_BL_Y, 0x0000); // Bottom Left Y

}

static int touchscreen_fd;

int touchscreen_init()
{
    touchscreen_fd = open(devname, O_RDWR);
    if (touchscreen_fd < 0) {
        perror("open");
	return 0;
    }

    int addr = STMPE610_ADDRESS; /* The I2C address */

    if (ioctl(touchscreen_fd, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	return 0;
    }

    // uint16_t id = read_u16(touchscreen_fd, 0x00);
    // printf("id is 0x%04X\n", id);

    STMPE610_init(touchscreen_fd);
    return 1;
}

TouchscreenEvent touchscreen_read(int *x, int *y, float *z)
{
    static int was_touching = 0;
    static int pending_start = 0;
    TouchscreenEvent event_type = TOUCHSCREEN_IDLE;

    uint8_t tsc_ctrl = read_u8(touchscreen_fd, STMPE610_TSC_CTRL);
    int now_touching = (tsc_ctrl & 0x80) != 0;
    int dequeue_fifo = 0;

    if(!was_touching) {

        // If we weren't touching but now are, mark the next xyz as "START"
	if(now_touching)  {
	    pending_start = 1;
	    dequeue_fifo = 1;
	}

    } else { // was_touching

        if(!now_touching) {
            if(!pending_start) {
                // If we were touching but now aren't and had sent one "DRAG",
                // return "STOP"
                event_type = TOUCHSCREEN_STOP; 
            } else {
		// If we were touching but now aren't but were still
		// waiting to send a DRAG, don't send anything and
		// go back to waiting for an initial press.  I
		// don't know if this circumstance will happen.
                pending_start = 0;
            }
        } else 
            dequeue_fifo = 1;
    }

    if(dequeue_fifo) {
        uint8_t fifo_size;
        fifo_size = read_u8(touchscreen_fd, STMPE610_FIFO_SIZE);
        if(fifo_size > 0) {
            uint8_t d0 = read_u8(touchscreen_fd, STMPE610_TSC_DATA_NOAUTOINC);
            uint8_t d1 = read_u8(touchscreen_fd, STMPE610_TSC_DATA_NOAUTOINC);
            uint8_t d2 = read_u8(touchscreen_fd, STMPE610_TSC_DATA_NOAUTOINC);
            uint8_t d3 = read_u8(touchscreen_fd, STMPE610_TSC_DATA_NOAUTOINC);


            *x = (4096 - 1 - ((d0 << 4) | (d1 >> 4))) * 800 / 4096;
            *y = (4096 - 1 - (((d1 & 0xF) << 8) | d2)) * 480 / 4096;
            *z = 16.0 - d3 / 16.0;

            if(pending_start) {
                event_type = TOUCHSCREEN_START;
                pending_start = 0;
            } else {
                event_type = TOUCHSCREEN_DRAG;
            }
        }
        // write_u8(touchscreen_fd, STMPE610_INT_STA, 0xff);
    }

    was_touching = now_touching;

    return event_type;
}

