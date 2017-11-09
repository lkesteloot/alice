
#include <stdio.h>
#include "gyro.h"

int main()
{
    int success = gyro_init();
    if (!success) {
	return 1;
    }

    int dx, dy, dz;
    int count = 0;
    while (1) {
	if (gyro_ready()) {
	    gyro_read(&dx, &dy, &dz);
	    count++;
	    if (1) {
		printf("%8d %8d %8d %s\n", dx, dy, dz,
		    gyro_overrun() ? "overrun" : "");
	    } else {
		if (gyro_overrun()) {
		    printf("overrun (%d)\n", count);
		}
	    }

	    if (count > 200 && 0) {
		break;
	    }
	}
    }

    return 0;
}
