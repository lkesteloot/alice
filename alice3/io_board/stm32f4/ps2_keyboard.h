#ifndef __PS2_KEYBOARD_H__
#define __PS2_KEYBOARD_H__

#include "byte_queue.h"

extern volatile unsigned char gKeyboardOverflowed;
extern volatile unsigned char gDumpKeyboardData;

int PS2_process_byte(unsigned char kbd_byte);

#endif /* __PS2_KEYBOARD_H__ */

