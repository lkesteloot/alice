#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

extern volatile int gKeyboardOverflowed;
extern volatile int gKeyboardBATBadParity;
extern volatile int gKeyboardParityError;
extern volatile int gKeyboardRawData;

void KBD_init();
int KBD_process_queue();

#endif /* __KEYBOARD_H__ */
