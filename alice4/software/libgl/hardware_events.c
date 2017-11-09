#include <stdio.h>
#include <math.h>
#include "event_service.h"
#include "touchscreen.h"
#include "awesome.h"
#include "device.h"
#include "accelerometer.h"
#include "gyro.h"

typedef struct event {
    int32_t device;
    int16_t val;
    // union here for other events like keys
} event;

static uint32_t tied_valuators[2048][2];
static uint32_t device_queued[2048];

#define INPUT_QUEUE_SIZE 128
static event input_queue[INPUT_QUEUE_SIZE];
// The next time that needs to be read:
static int input_queue_head = 0;
// The number of items in the queue (tail = (head + length) % len):
static int input_queue_length = 0;

static int mousex = -1; // valuator for touchscreen X
static int mousey = -1; // valuator for touchscreen Y

static int home_button_previous = 0;

// How many gyro devices have been read (0 to 2).
static int gyro_reads = 0;
static int32_t gyro_dx = 0;
static int32_t gyro_dy = 0;
static int32_t gyro_dz = 0;

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

static void drain_buttons()
{
    // Only Home button for now, which is mapped to ESC.
    int home_button;
    event ev;
    
    // libawesome will queue up two transitions per frame, so read the button
    // twice to get those.
    for (int i = 0; i < 2; i++) {
	home_button = awesome_get_home_button();
	if(home_button != home_button_previous) {
	    if(device_queued[ESCKEY]) {
		ev.device = ESCKEY;
		ev.val = home_button;
		enqueue_event(&ev);
	    }
	    home_button_previous = home_button;
	}
    }
}

static void drain_touchscreen()
{
    int x, y;
    float z;

    TouchscreenEvent e;
    event ev;
    int drained = 0;
    while((e = touchscreen_read(&x, &y, &z)) != TOUCHSCREEN_IDLE) {
	drained ++;
	switch(e) {
	    case TOUCHSCREEN_START:
		// N.B. The devices is rotated 180 degrees from it's
		// "natural," originally-developed orientation
		mousex = 800 - 1 - x;
		mousey = y; // GL valuator MOUSEY is 0 at bottom
		if(device_queued[LEFTMOUSE]) {
		    ev.device = LEFTMOUSE;
		    ev.val = 1;
		    enqueue_event(&ev);
		    for(int j = 0; j < 2; j++) {
			if(tied_valuators[LEFTMOUSE][j]) {
			    ev.device = tied_valuators[LEFTMOUSE][j];
			    ev.val = events_get_valuator(tied_valuators[LEFTMOUSE][j]);
			    enqueue_event(&ev);
			}
		    }
		}
		break;
	    case TOUCHSCREEN_DRAG:
	    	// XXX device is rotated 180 degrees
		mousex = 800 - 1 - x;
		mousey = y; // GL valuator MOUSEY is 0 at bottom
		break;
	    case TOUCHSCREEN_STOP:
		if(device_queued[LEFTMOUSE]) {
		    ev.device = LEFTMOUSE;
		    ev.val = 0;
		    enqueue_event(&ev);
		    for(int j = 0; j < 2; j++) {
			if(tied_valuators[LEFTMOUSE][j]) {
			    ev.device = tied_valuators[LEFTMOUSE][j];
			    ev.val = events_get_valuator(tied_valuators[LEFTMOUSE][j]);
			    enqueue_event(&ev);
			}
		    }
		}
		break;
	    case TOUCHSCREEN_IDLE:
	    default:
		break;
	}
    }
}

static float theta_x_smoothed, theta_y_smoothed;
static const float decay = .95;

int32_t events_get_valuator(int32_t device)
{
    drain_touchscreen();
    if(device == MOUSEX)
	return mousex;
    else if(device == MOUSEY)
	return mousey;
    else if(device == DIAL0 || device == DIAL1) {
	float theta_x, theta_y, value;

	accelerometer_read(&theta_y, &theta_x);
	// N.B. The devices is rotated 180 degrees from its "natural,"
	// originally-developed orientation
	theta_y = -theta_y;
	theta_x = -theta_x;

	theta_x_smoothed = theta_x_smoothed * decay + theta_x * (1 - decay);
	theta_y_smoothed = theta_y_smoothed * decay + theta_y * (1 - decay);

	if(device == DIAL0)
	    value = theta_x_smoothed / (2 * M_PI) * 3600;
	else /* device == DIAL1 */
	    value = theta_y_smoothed / (2 * M_PI) * 3600;

	return value;
    } else if(device == DIAL2 || device == DIAL3 || device == DIAL4) {
	if (gyro_reads == 0) {
	    // Get the data from the real device.
	    while (!gyro_ready()) {
		// Wait.
	    }

	    gyro_read(&gyro_dx, &gyro_dy, &gyro_dz);
	}

	gyro_reads = (gyro_reads + 1) % 3;

	switch (device) {
	    case DIAL2: return gyro_dx;
	    case DIAL3: return gyro_dy;
	    case DIAL4: return gyro_dz;
	}
    } else
	printf("warning: unimplemented evaluator %d\n", device);
    return 0;
}

void events_qdevice(int32_t device)
{
    switch(device) {
	case MOUSEX:
	case MOUSEY:
	case LEFTMOUSE:
	    break;
	default:
	    printf("warning: qdevice for unimplemented device %d\n", device);
    }
    device_queued[device] = device;
}

uint32_t event_qread_start(int blocking)
{
    drain_touchscreen();
    drain_buttons();
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
    awesome_init();
    touchscreen_init();
    accelerometer_init();
    accelerometer_read(&theta_y_smoothed, &theta_x_smoothed);
    gyro_init();
    drain_touchscreen();
    drain_buttons();
    return 0;
}

void events_tie(int32_t button, int32_t val1, int32_t val2)
{
    // XXX this maybe should be allowed to happen in libgl
    // XXX except that network_events is probably more efficient handling
    // XXX tie() itself...
    tied_valuators[button][0] = val1;
    tied_valuators[button][1] = val2;
}
