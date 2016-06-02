
#ifndef __DRIVER_H__
#define __DRIVER_H__

// Single byte sent for each command to the GPU.
typedef enum {
    // Graphics commands (to real FPGA).
    COMMAND_ZBUFFER,
    COMMAND_CLEAR,
    COMMAND_ZCLEAR,
    COMMAND_SWAPBUFFERS,
    COMMAND_TRIANGLE,
    COMMAND_SET_PATTERN,
    COMMAND_ENABLE_PATTERN,
    COMMAND_DISABLE_PATTERN,

    // Window and input commands (later to be handled locally).
    COMMAND_WINOPEN,
    COMMAND_GET_VALUATOR,
    COMMAND_QDEVICE,
    COMMAND_UNQDEVICE,
    COMMAND_TIE,
    COMMAND_QREAD,
} Command;

#endif // __DRIVER_H__
