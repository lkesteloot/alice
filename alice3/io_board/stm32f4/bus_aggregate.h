#ifndef __BUS_AGGREGATE_H__
#define __BUS_AGGREGATE_H__

void BUS_write_memory_byte(unsigned int a, unsigned char d);
unsigned char BUS_read_memory_byte(unsigned int a);
void BUS_write_io_byte(unsigned int a, unsigned char d);
unsigned char BUS_read_io_byte(unsigned int a);
void BUS_acquire_bus(int releaseReset);
void BUS_release_bus(int startReset);

#endif /* __BUS_AGGREGATE_H__ */
