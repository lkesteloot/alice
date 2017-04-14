
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

// GPU emulation over the network
typedef enum {
    // Graphics commands
    COMMAND_ZBUFFER,
    COMMAND_CLEAR,
    COMMAND_ZCLEAR,
    COMMAND_SWAPBUFFERS,
    COMMAND_TRIANGLE,
    COMMAND_SET_PATTERN,
    COMMAND_ENABLE_PATTERN,
    COMMAND_DISABLE_PATTERN,

    // Window and input commands
    COMMAND_WINOPEN,
    COMMAND_GET_VALUATOR,
    COMMAND_QDEVICE,
    COMMAND_UNQDEVICE,
    COMMAND_TIE,
    COMMAND_QREAD,
} Command;

// Interface for connection to network
void open_connection();
int32_t network_winopen(char *title);
void send_uint8(unsigned char b);
void send_uint16(uint16_t x);
void send_uint32(uint32_t x);
void send_and_capture_string(char *s);
void send_and_capture_uint8(uint8_t b);
void send_and_capture_uint16(uint16_t x);
void send_and_capture_uint32(uint32_t x);
unsigned char receive_uint8();
uint16_t receive_uint16();
uint32_t receive_uint32();
void flush();
void close_connection();

#endif // __CONNECTION_H__
