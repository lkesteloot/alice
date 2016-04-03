#ifndef __MONITOR_QUEUE_H__
#define __MONITOR_QUEUE_H__

#include "byte_queue.h"

#define MON_QUEUE_CAPACITY 32
struct mon_queue_struct {
    struct queue q;
    unsigned char queue[MON_QUEUE_CAPACITY];
};
extern volatile struct mon_queue_struct mon_queue;

// Call this from ISR, so skip di/ei
static inline void monitor_enqueue_key_unsafe(unsigned char d)
{
    if(!queue_isfull(&mon_queue.q)) {
        queue_enq(&mon_queue.q, d);
    }
}

void MON_init();

#endif /* __MONITOR_QUEUE_H__ */

