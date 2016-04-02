#include "console_queue.h"

unsigned char gConsoleOverflowed = 0;

volatile struct con_queue_struct con_queue;
