#include <stdio.h>
#include "event_service.h"
#include "touchscreen.h"
#include "device.h"

typedef struct event {
    int32_t device;
    int16_t val;
    // union here for other events like keys
} event;

#define INPUT_QUEUE_SIZE 128
static event input_queue[INPUT_QUEUE_SIZE];
// The next time that needs to be read:
static int input_queue_head = 0;
// The number of items in the queue (tail = (head + length) % len):
static int input_queue_length = 0;

static int mousex = -1; // valuator for touchscreen X
static int mousey = -1; // valuator for touchscreen X

static void enqueue_event(event *e)
{
    if (input_queue_length == INPUT_QUEUE_SIZE) {
        printf("Input queue overflow.");
    } else {
        int tail = (input_queue_head + input_queue_length) % INPUT_QUEUE_SIZE;
        input_queue[tail] = *e;
        input_queue_length++;
    }
}

static void drain_touchscreen()
{
    int x, y;
    float z;

    TouchscreenEvent e;
    event ev;
    while((e = touchscreen_read(&x, &y, &z)) != TOUCHSCREEN_IDLE) {
	switch(e) {
	    case TOUCHSCREEN_START:
		ev.device = LEFTMOUSE;
		ev.val = 1;
		enqueue_event(&ev);
		break;
	    case TOUCHSCREEN_DRAG:
		mousex = x;
		mousey = y;
		break;
	    case TOUCHSCREEN_STOP:
		ev.device = LEFTMOUSE;
		ev.val = 0;
		enqueue_event(&ev);
		break;
	    case TOUCHSCREEN_IDLE:
	    default:
		break;
	}
    }

    // usleep(100000);
}

int32_t events_get_valuator(int32_t device)
{
    drain_touchscreen();
    if(device == MOUSEX)
	return mousex;
    else if(device == MOUSEY)
	return mousey;
    return 0;
}

void events_qdevice(int32_t device)
{
}

uint32_t event_qread_start(int blocking)
{
    drain_touchscreen();
    return input_queue_length;
}

int32_t events_qread_continue(int16_t *value)
{
    *value = input_queue[input_queue_head].val;
    int32_t device = input_queue[input_queue_head].device;
    input_queue_head = (input_queue_head + 1) % INPUT_QUEUE_SIZE;
    input_queue_length--;
    return device;
}

int32_t events_winopen(char *title)
{
    touchscreen_init();
    drain_touchscreen();
    return 0;
}

void events_tie(int32_t button, int32_t val1, int32_t val2)
{
}
