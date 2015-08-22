#include <stdio.h>
#include <string.h>
#include <math.h>


unsigned char buf[65536];


unsigned char pic[238][210];


void loadPic(char *name)
{
    FILE *fp;
    int width, height;
    int x, y;

    fp = fopen(name, "r");

    if(fp == NULL)
    {
	fprintf(stderr, "can't open \"%s\"\n", name);
	exit(1);
    }
    fscanf(fp, "P1 %d %d", &width, &height);
    
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++)
	{
	    int pixel;
	    fscanf(fp, "%d", &pixel);
	    pic[(238 - height) / 2 + y][(210 - width) / 2 + 10 + x] = (pixel == 0);
	}
}


int main(int argc, char **argv)
{
    int row;
    int col;
    int x, y;
    unsigned char *p;

    /*
     * Fill picture with black
     */
    memset(pic, 0x00, sizeof(pic));

    loadPic(argv[1]);

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
		x = (col - 17);
		y = (row - 24);
		if(pic[y][x])
		    *p++ = 0xc0;
		else
		    *p++ = 0x80;
	    }

    fwrite(buf, 65536, 1, stdout);

    return(0);
}
