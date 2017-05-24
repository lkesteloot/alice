#ifndef _I2C_UTILITY_
#define _I2C_UTILITY_

#include <inttypes.h>

uint32_t read_u32(int fd, int reg);
uint16_t read_u16(int fd, int reg);
uint8_t read_u8(int fd, int reg);
void write_u16(int fd, int reg, uint16_t value);
void write_u8(int fd, int reg, uint8_t value);

#endif /* _I2C_UTILITY_ */
