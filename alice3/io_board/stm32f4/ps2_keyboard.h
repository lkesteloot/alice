#ifndef __PS2_KEYBOARD_H__
#define __PS2_KEYBOARD_H__

#include "byte_queue.h"

#define PS2_KBD_BAT 0xAA

int PS2_process_byte(int dump_data, unsigned char kbd_byte);

#endif /* __PS2_KEYBOARD_H__ */

