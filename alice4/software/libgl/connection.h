
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

// Interface for connection (to network or FPGA).
void open_connection();
void send_uint8(unsigned char b);
unsigned char receive_uint8();
void flush();
void close_connection();

#endif // __CONNECTION_H__
