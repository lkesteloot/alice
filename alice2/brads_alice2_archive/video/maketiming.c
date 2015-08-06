#include <stdio.h>
#include <string.h>
#include <math.h>

char picture[23][22] = 
{
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    ".....***.....***.....",
    ".....***.....***.....",
    ".....***.....***.....",
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    "....*...........*....",
    "....**.........**....",
    "....***.......***....",
    ".....****...****.....",
    "......*********......",
    "........*****........",
    ".....................",
    ".....................",
    ".....................",
    "....................."
};


unsigned char buf[65536];


int main(int argc, char **argv)
{
    int row;
    int col;
    int x, y;
    unsigned char *p;

    /*
     * Fill memory with reset command
     */
    memset(buf, 0x20, sizeof(buf));

    p = buf;

    /*
     * Rows 0 through 2 are vertical blank
     */
    for(row = 0; row < 3; row++)
	for(col = 0; col < 227; col++)
	    if(col < 210)
		*p++ = 0x00;
	    else
		*p++ = 0x80;

    /*
     * Rows 3 through 23 are other part of vertical blank
     */
    for(row = 3; row < 24; row++)
	for(col = 0; col < 227; col++)
	    if(col < 17)
		*p++ = 0x00;
	    else
		*p++ = 0x80;

    /*
     * Remaining rows are video data plus horizontal blanking
     */
    for(row = 24; row < 262; row++)
	for(col = 0; col < 227; col++)
	    if(col < 17)
		*p++ = 0x00;
	    else {
		x = (col - 17) / 10;
		y = (row - 24) / 10;
		if(picture[y][x] == '*')
		    *p++ = 0xc0;
		else
		    *p++ = 0x80;
	    }

    fwrite(buf, 65536, 1, stdout);

    return(0);
}
