#include <stdio.h>
#include <ctype.h>
#include "logprintf.h"
#include "ps2_keyboard.h"

static char kbd_up_key_flag = 0;
static char kbd_shift_status = 0;
static char kbd_alt_status = 0;
static char kbd_ctrl_status = 0;

#define LSHIFT_KEY 0x12
#define RSHIFT_KEY 0x59
#define CTRL_KEY 0x14
#define ALT_KEY 0x11
#define UP_KEY 0xF0
#define EXT_KEY 0xE0
#define EXT2_KEY 0xE1

// Normal, shift, ctrl, alt
static const unsigned char keymap_table[] = {
   '?', '?', '?', '?',
   '9', '9', '9', '9',
   '?', '?', '?', '?',
   '5', '5', '5', '5',
   '3', '3', '3', '3',
   '1', '1', '1', '1',
   '2', '2', '2', '2',
   '1', '1', '1', '1',
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '8', '8', '8', '8',
   '6', '6', '6', '6',
   '4', '4', '4', '4',
   9,   9,   9,   9,
   '`', '~', '`', '`',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'q', 'Q',  17,  17,
   '1', '!', '1', '1',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'z', 'Z',  26,  26,
   's', 'S',  19,  19,
   'a', 'A',   1,   1,
   'w', 'W',  23,  23,
   '2', '@', '2', '2',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'c', 'C',   3,   3,
   'x', 'X',  24,  24,
   'd', 'D',   4,   4,
   'e', 'E',   5,   5,
   '4', '$', '4', '4',
   '3', '#', '3', '3',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   ' ', ' ', ' ', ' ' ,
   'v', 'V',  22,  22,
   'f', 'F',   6,   6,
   't', 'T',  20,  20,
   'r', 'R',  18,  18,
   '5', '%', '5', '5',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'n', 'N',  14,  14,
   'b', 'B',   2,   2,
   'h', 'H',   8,   8,
   'g', 'G',   7,   7,
   'y', 'Y',  25,  25,
   '6', '^', '6', '6',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'm', 'M',  13,  13,
   'j', 'J',  10,  10,
   'u', 'U',  21,  21,
   '7', '&', '7', '7',
   '8', '*', '8', '8',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   ',', '<', ',', ',',
   'k', 'K',  11,  11,
   'i', 'I',   9,   9,
   'o', 'O',  15,  15,
   '0', ')', '0', '0',
   '9', '(', '9', '9',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '.', '>', '.', '.',
   '/', '?', '/', '/',
   'l', 'L',  12,  12,
   ';', ':', ';', ';',
   'p', 'P',  16,  16,
   '-', '_', '-', '-',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   39, '"',  39,  39,
   '?', '?', '?', '?',
   '[', '{', '[', '[',
   '=', '+', '=', '=',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   13,  13,  13,  13,
   ']', '}', ']', ']',
   '?', '?', '?', '?',
   92, '|',  92,  92,
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   8,   8,   8,   8,
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '?', '?', '?', '?',
   '4', '4', '4', '4',
   '7', '7', '7', '7',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '0', '0', '0', '0',
   '.', '.', '.', '.',
   '2', '2', '2', '2',
   '5', '5', '5', '5',
   '6', '6', '6', '6',
   '8', '8', '8', '8',
   27,  27,  27,  27,
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '+', '+', '+', '+',
   '3', '3', '3', '3',
   '-', '-', '-', '-',
   '*', '*', '*', '*',
   '9', '9', '9', '9',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
};

static unsigned char kbd_lookup(int shift, int alt, int ctrl, unsigned char byte)
{
    int which = 0;
    if(shift) which = 1;
    else if(ctrl) which = 2;
    else if(alt) which = 3;
    return keymap_table[byte * 4 + which];
}

int PS2_process_byte(int dump_data, unsigned char kbd_byte)
{
    int result = -1;

    if(kbd_byte == PS2_KBD_BAT) {
        // Keyboard successful assurance test
    } else if(kbd_byte == UP_KEY) {
        kbd_up_key_flag = 1;
        if(dump_data) 
            logprintf(DEBUG_DATA, "keyboard key up\n");
    } else {
        switch(kbd_byte) {
            case LSHIFT_KEY:
            case RSHIFT_KEY:
                kbd_shift_status = !kbd_up_key_flag;
                if(dump_data) 
                    logprintf(DEBUG_DATA, "shift status is now %d\n", kbd_shift_status);
                break;
            case ALT_KEY:
                kbd_alt_status = !kbd_up_key_flag;
                if(dump_data) 
                    logprintf(DEBUG_DATA, "alt status is now %d\n", kbd_alt_status);
                break;
            case CTRL_KEY:
                kbd_ctrl_status = !kbd_up_key_flag;
                if(dump_data) 
                    logprintf(DEBUG_DATA, "ctrl status is now %d\n", kbd_ctrl_status);
                break;
            default:
                if(!kbd_up_key_flag)
                    if(!(kbd_byte & 0x80)) {
                        unsigned char c = kbd_lookup(kbd_shift_status, kbd_alt_status, kbd_ctrl_status, kbd_byte);
                        if(dump_data) {
                            logprintf(DEBUG_DATA, "keyboard ASCII: %02X", c);
                            if(isprint(c))
                                logprintf(DEBUG_DATA, "(%c)\n", c);
                            else 
                                putchar('\n');
                        }
                        result = c;
                    }
                break;
        }
        kbd_up_key_flag = 0;
    }

    return result;
}


