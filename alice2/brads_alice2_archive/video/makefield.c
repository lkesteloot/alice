/*
    H line = 63.6 usec (15.720 Hz)
    H sync = 5 usec (7.86%)
    V sync = 400 usec (80 H)
    live scan = 160 H
    retrace = 102 H
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define NTSC_EQPULSE_LINES	3
#define NTSC_VSYNC_LINES	3
#define NTSC_VBLANK_LINES	15
#define NTSC_FIELD_LINES	262
#define NTSC_PIXEL_LINES	(NTSC_FIELD_LINES - NTSC_VBLANK_LINES - NTSC_VSYNC_LINES - NTSC_EQPULSE_LINES * 2)
#define NTSC_HOR_SYNC_DUR	.08
#define NTSC_FIELDS		60
#define NTSC_FRONTPORCH		.02
#define NTSC_BACKPORCH		.06


int main(int argc, char **argv)
{
    int count;
    int col;
    double clock;
    int horSyncTicks;
    int lineTicks;
    int fieldTicks;
    int frontporchTicks;
    int backporchTicks;

    if (argc != 2) {
	fprintf(stderr, "usage: %s timingclock\n", argv[0]);
	fprintf(stderr, "timingclock is specified in megahertz\n");
	exit(EXIT_FAILURE);
    }

    clock = atof(argv[1]);

    fieldTicks = floor(clock * 1000000.0 / NTSC_FIELDS + 0.5);
    lineTicks = floor((double)fieldTicks / NTSC_FIELD_LINES + 0.5);
    horSyncTicks = floor(lineTicks * NTSC_HOR_SYNC_DUR + 0.5);
    frontporchTicks = lineTicks * NTSC_FRONTPORCH;
    backporchTicks = lineTicks * NTSC_FRONTPORCH;

    /* printf("clock = %f, fieldTicks = %d, lineTicks = %d, horSyncTicks = %d\n", */
        /* clock, fieldTicks, lineTicks, horSyncTicks); */

    /*
     * Video signal length in clock ticks
     */
    printf("%d %d\n", lineTicks, NTSC_FIELD_LINES);

    /*
     * Pixel area (justified to lower-right of sync image) 
     */
    printf("%d %d\n", lineTicks - horSyncTicks, NTSC_PIXEL_LINES);

    /*
     * Actual visible image area offset from lower-right and size
     */
    printf("%d %d %d %d\n", /* XXX grantham - get calculations from notebook */
        backporchTicks, 0,
        lineTicks - horSyncTicks - frontporchTicks - backporchTicks,
	NTSC_PIXEL_LINES);

    /*
     * Rows 0 through 8 are vertical blank
     */
    for (count = 0; count < NTSC_EQPULSE_LINES ; count++)
	for (col = 0; col < lineTicks; col++)
	    if (col < horSyncTicks || (col > lineTicks/2 && col < lineTicks/2 + horSyncTicks))
		printf("0 ");
	    else
		printf("128 ");

    for (count = 0; count < NTSC_VSYNC_LINES ; count++)
	for (col = 0; col < lineTicks; col++)
	    if (col < lineTicks/2-horSyncTicks || (col > lineTicks/2 && col < 210))
		printf("0 ");
	    else
		printf("128 ");

    for (count = 0; count < NTSC_EQPULSE_LINES ; count++)
	for (col = 0; col < lineTicks; col++)
	    if (col < horSyncTicks || (col > lineTicks/2 && col < lineTicks/2 + horSyncTicks))
		printf("0 ");
	    else
		printf("128 ");

    /*
     * Rows 9 through 23 are other part of vertical blank
     */
    for (count = 0; count < NTSC_VBLANK_LINES ; count++)
	for (col = 0; col < lineTicks; col++)
	    if (col < horSyncTicks)
		printf("0 ");
	    else
		printf("128 ");

    /*
     * Remaining rows are video data plus horizontal blanking
     */
    for (count = 0; count < NTSC_PIXEL_LINES; count++)
	for (col = 0; col < lineTicks; col++)
	    if (col < horSyncTicks)
		printf("0 ");
	    else
		/* if(col > horSyncTicks + 20 && col < lineTicks - 20) */
		    /* printf("192 "); */
		/* else */
		    printf("128 ");

    return(0);
}
