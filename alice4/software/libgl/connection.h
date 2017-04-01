
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

// Interface for connection to network
void open_connection();
void send_uint8(unsigned char b);
unsigned char receive_uint8();
void flush();
void close_connection();
void send_and_capture_string(char *s);
void send_and_capture_uint16(uint16_t x);
void send_and_capture_uint32(uint32_t x);
void send_uint16(uint16_t x);
void send_uint32(uint32_t x);
uint16_t receive_uint16();
uint32_t receive_uint32();

#endif // __CONNECTION_H__
