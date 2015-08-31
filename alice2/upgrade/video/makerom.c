#include <stdio.h>
#include <stdlib.h>

unsigned char pic[256][256];
int imgx, imgy;


void loadPic(char *name)
{
    FILE *fp;
    int x, y;
    int pixel;

    fp = fopen(name, "r");

    if (fp == NULL) {
	perror(name);
	exit(EXIT_FAILURE);
    }
    fscanf(fp, "P1 %d %d", &imgx, &imgy);
    
    for (y = 0; y < imgy; y++) {
        for (x = 0; x < imgx; x++) {
	    fscanf(fp, "%d", &pixel);
	    if(x < 156 && y < 238)
		pic[y][x] = (pixel == 0);
	}
    }
    if(imgx > 156)
	imgx = 156;
    if(imgy > 238)
	imgy = 238;

    fclose(fp);
}


int main(int argc, char **argv)
{
    int x, y, px, py, vx, vy, ox, oy;
    int i, j;
    unsigned int value;
    int pixx, pixy;
    int total;

    if (argc != 2) {
	fprintf(stderr, "usage: %s file.pbm\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    loadPic(argv[1]);

    total = 0;
    scanf("%d %d %d %d %d %d %d %d", &x, &y, &px, &py, &ox, &oy, &vx, &vy);
    for(j = 0; j < y; j++) {
	for(i = 0; i < x; i++) {
	    scanf("%d", &value);
	    pixx = i - (x - ox - vx / 2 - imgx / 2);
	    pixy = j - (y - oy - vy / 2 - imgy / 2);
	    if(pixx >= 0 && pixy >= 0 && pixx < imgx && pixy < imgy &&
		pic[pixy][pixx])
	    {
		putchar(value | 64 | 32);
	    } else {
		putchar(value | 32);
	    }

	    total++;
	}
    }

    while (total < (1 << 16)) {
	putchar(0); /* reset the counter. */
	total++;
    }

    return(0);
}
