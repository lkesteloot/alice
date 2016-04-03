#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

extern volatile unsigned char gKeyboardOverflowed;

void KBD_init();
int KBD_process_queue();

#endif /* __KEYBOARD_H__ */
