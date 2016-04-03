#ifndef __CRC7_H__
#define __CRC7_H__

#include <stdlib.h>

unsigned char crc7_generate_bytes(unsigned char *b, int count);
unsigned short crc_itu_t(unsigned short crc, const unsigned char *buffer, size_t len);

#endif /* __CRC7_H__ */
