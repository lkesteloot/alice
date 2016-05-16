
#ifndef __DRIVER_H__
#define __DRIVER_H__

// Single byte sent for each command to the GPU.
typedef enum {
    COMMAND_WINOPEN,
    COMMAND_CLEAR,
    COMMAND_SWAPBUFFERS,
    COMMAND_TRIANGLE,
    COMMAND_GET_VALUATOR,
} Command;

#endif // __DRIVER_H__
