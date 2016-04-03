#ifndef __PS2_KEYBOARD_H__
#define __PS2_KEYBOARD_H__

#include "byte_queue.h"

extern volatile unsigned char gKeyboardOverflowed;

int PS2_process_byte(int dump_data, unsigned char kbd_byte);

#endif /* __PS2_KEYBOARD_H__ */

