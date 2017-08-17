//----------------------------------------------------------------------------
// Command monitor input queue

#include "monitor_queue.h"

volatile struct mon_queue_struct mon_queue;

void MON_init()
{
    queue_init(&mon_queue.q, MON_QUEUE_CAPACITY);
}
