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

const int ROM_BITMASK_NOT_SYNC = 0x80;
const int ROM_BITMASK_NOT_RESET_COUNTER = 0x20;
const int ROM_BITMASK_NOT_BLANKING = 0x08;

int timing_rom_byte(int sync, int blank)
{
    int mask = 0;

    if(!sync)
        mask |= ROM_BITMASK_NOT_SYNC;
    if(!blank)
        mask |= ROM_BITMASK_NOT_BLANKING;

    return mask;
}

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

    if(0)
        printf("clock = %f, fieldTicks = %d, lineTicks = %d, horSyncTicks = %d\n",
            clock, fieldTicks, lineTicks, horSyncTicks);

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
	for (col = 0; col < lineTicks; col++) {
	    int sync = (col < horSyncTicks || (col > lineTicks/2 && col < lineTicks/2 + horSyncTicks));
            int blank = 1;
            printf("%d ", timing_rom_byte(sync, blank));
        }

    for (count = 0; count < NTSC_VSYNC_LINES ; count++)
	for (col = 0; col < lineTicks; col++) {
	    int sync = (col < lineTicks/2-horSyncTicks || (col > lineTicks/2 && col < 210));
            int blank = 1;
            printf("%d ", timing_rom_byte(sync, blank));
        }

    for (count = 0; count < NTSC_EQPULSE_LINES ; count++)
	for (col = 0; col < lineTicks; col++) {
	    int sync = (col < horSyncTicks || (col > lineTicks/2 && col < lineTicks/2 + horSyncTicks));
            int blank = 1;
            printf("%d ", timing_rom_byte(sync, blank));
        }

    /*
     * Rows 9 through 23 are other part of vertical blank
     */
    for (count = 0; count < NTSC_VBLANK_LINES ; count++)
	for (col = 0; col < lineTicks; col++) {
	    int sync = (col < horSyncTicks);
            int blank = 1;
            printf("%d ", timing_rom_byte(sync, blank));
        }

    /*
     * Remaining rows are video data plus horizontal blanking
     */
    for (count = 0; count < NTSC_PIXEL_LINES; count++)
	for (col = 0; col < lineTicks; col++) {
	    int sync = (col < horSyncTicks);
            int blank = sync;
            printf("%d ", timing_rom_byte(sync, blank));
        }

    return 0;
}
