#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include "touchscreen.h"

int main()
{
    touchscreen_init();

    while(1) {
	static bool dragging = false;
	int x, y;
	float z;

        TouchscreenEvent e;
	while((e = touchscreen_read(&x, &y, &z)) != TOUCHSCREEN_IDLE) {
	    switch(e) {
		case TOUCHSCREEN_START:
		    printf("start %d %d %f\n", x, y, z);
		    if(dragging)
			printf("ERROR: START WHILE ALREADY DRAGGING\n");
		    dragging = true;
		    break;
		case TOUCHSCREEN_DRAG:
		    printf("drag %d %d %f\n", x, y, z);
		    if(!dragging)
			printf("ERROR: DRAG WHILE NOT STARTED\n");
		    break;
		case TOUCHSCREEN_STOP:
		    printf("stop\n");
		    if(!dragging)
			printf("ERROR: STOP WHILE NOT STARTED\n");
		    dragging = false;
		    break;
		case TOUCHSCREEN_IDLE:
		default:
		    break;
	    }
	}

	// usleep(100000);
    }
}
