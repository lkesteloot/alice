#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include "touchscreen.h"

int main()
{
    touchscreen_init();

    while(1) {
	int x, y;
	float z;

        TouchscreenEvent e;
	while((e = touchscreen_read(&x, &y, &z)) != TOUCHSCREEN_IDLE) {
	    switch(e) {
		case TOUCHSCREEN_START:
		    printf("start %d %d %f\n", x, y, z);
		    break;
		case TOUCHSCREEN_DRAG:
		    printf("drag %d %d %f\n", x, y, z);
		    break;
		case TOUCHSCREEN_STOP:
		    printf("stop\n");
		    break;
		case TOUCHSCREEN_IDLE:
		default:
		    break;
	    }
	}

	// usleep(100000);
    }
}
