#ifndef __CONSOLE_QUEUE_H__
#define __CONSOLE_QUEUE_H__

#include "defs.h"
#include "byte_queue.h"

//----------------------------------------------------------------------------
// Console input queue

extern unsigned char gConsoleOverflowed;

#define CON_QUEUE_CAPACITY 64
struct con_queue_struct {
    struct queue q;
    unsigned char queue[CON_QUEUE_CAPACITY];
};
extern volatile struct con_queue_struct con_queue;

inline static void console_enqueue_key_unsafe(unsigned char d)
{
    unsigned char full;
    full = queue_isfull(&con_queue.q);
    if(full) {
        gConsoleOverflowed = 1;
    } else {
        queue_enq(&con_queue.q, d);
    }
}

#endif /* __CONSOLE_QUEUE_H__ */
