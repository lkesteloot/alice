#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static void skipComments(FILE *fp)
{
    int c;

    while((c = fgetc(fp)) == '#')
        while((c = fgetc(fp)) != '\n');
    ungetc(c, fp);
}


int pnmRead(FILE *file, int *w, int *h, float **pixels)
{
    unsigned char	dummyByte;
    int			i;
    float		max;
    char		token;
    int			width, height;
    float		*rgbPixels;

    fscanf(file, " ");

    skipComments(file);

    if(fscanf(file, "P%c ", &token) != 1)
    {
         fprintf(stderr, "pnmRead: Had trouble reading PNM tag\n");
	 return(0);
    }

    skipComments(file);

    if(fscanf(file, "%d ", &width) != 1)
    {
         fprintf(stderr, "pnmRead: Had trouble reading PNM width\n");
	 return(0);
    }

    skipComments(file);

    if(fscanf(file, "%d ", &height) != 1)
    {
         fprintf(stderr, "pnmRead: Had trouble reading PNM height\n");
	 return(0);
    }

    skipComments(file);

    if(token != '1' && token != '4')
        if(fscanf(file, "%f", &max) != 1)
        {
             fprintf(stderr, "pnmRead: Had trouble reading PNM max value\n");
	     return(0);
        }

    rgbPixels = malloc(width * height * 4 * sizeof(float));
    if(rgbPixels == NULL)
    {
         fprintf(stderr, "pnmRead: Couldn't allocate %d bytes\n",
	     width * height * 4 * sizeof(float));
         fprintf(stderr, "pnmRead: (For a %d by %d image)\n", width,
	     height);
	 return(0);
    }

    if(token != '4')
	skipComments(file);

    if(token != '4')
    fread(&dummyByte, 1, 1, file);	/* chuck white space */

    if(token == '1')
    {
	for(i = 0; i < width * height; i++)
	{
	    int pixel;
	    fscanf(file, "%d", &pixel);
	    pixel = 1 - pixel;
	    rgbPixels[i * 4 + 0] = pixel;
	    rgbPixels[i * 4 + 1] = pixel;
	    rgbPixels[i * 4 + 2] = pixel;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    else if(token == '2')
    {
	for(i = 0; i < width * height; i++)
	{
	    int pixel;
	    fscanf(file, "%d", &pixel);
	    rgbPixels[i * 4 + 0] = pixel / max;
	    rgbPixels[i * 4 + 1] = pixel / max;
	    rgbPixels[i * 4 + 2] = pixel / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    else if(token == '3')
    {
	for(i = 0; i < width * height; i++)
	{
	    int r, g, b;
	    fscanf(file, "%d %d %d", &r, &g, &b);
	    rgbPixels[i * 4 + 0] = r / max;
	    rgbPixels[i * 4 + 1] = g / max;
	    rgbPixels[i * 4 + 2] = b / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    else if(token == '4')
    {
        int bitnum = 0;

	for(i = 0; i < width * height; i++)
	{
	    unsigned char pixel;
	    unsigned char value;

	    if(bitnum == 0)
	        fread(&value, 1, 1, file);

	    pixel = (1 - ((value >> (7 - bitnum)) & 1));
	    rgbPixels[i * 4 + 0] = pixel;
	    rgbPixels[i * 4 + 1] = pixel;
	    rgbPixels[i * 4 + 2] = pixel;
	    rgbPixels[i * 4 + 3] = 1.0;

	    if(++bitnum == 8 || ((i + 1) % width) == 0)
	        bitnum = 0;
	}
    }
    else if(token == '5')
    {
	for(i = 0; i < width * height; i++)
	{
	    unsigned char pixel;
	    fread(&pixel, 1, 1, file);
	    rgbPixels[i * 4 + 0] = pixel / max;
	    rgbPixels[i * 4 + 1] = pixel / max;
	    rgbPixels[i * 4 + 2] = pixel / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    else if(token == '6')
    {
	for(i = 0; i < width * height; i++)
	{
	    unsigned char rgb[3];
	    fread(rgb, 3, 1, file);
	    rgbPixels[i * 4 + 0] = rgb[0] / max;
	    rgbPixels[i * 4 + 1] = rgb[1] / max;
	    rgbPixels[i * 4 + 2] = rgb[2] / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    *w = width;
    *h = height;
    *pixels = rgbPixels;
    return(1);
}

#define SAMPLES 20

void sampleImage(int w, int h, float *pixels,
    float x, float y, float w, float h,
    float *r, float *g, float *b)
{
    int i;
    float sampx, sampy;
    int truncx, truncy;
    float angle, dist;

    *r = *g = *b = 0;
    for(i = 0; i < SAMPLES; i++)
    {
	angle = drand48() * M_PI * 2;
	dist = drand48() * .7;
	sampx = x + w / 2 + w * cos(angle) * dist;
	sampy = y + h / 2 + h * sin(angle) * dist;
	truncx = sampx;
	truncy = sampy;
	if(truncx >= 0 && truncy >= 0 && truncx < w && truncy < h)
	{
	    *r += pixels[(truncx * w + truncy) * 4 + 0] / SAMPLES;
	    *g += pixels[(truncx * w + truncy) * 4 + 1] / SAMPLES;
	    *b += pixels[(truncx * w + truncy) * 4 + 2] / SAMPLES;
	}
    }
}

int ditherPixel(int x, int y, float r, float g, float b)
{
    return(r + g + b > 1.5);
}

int imgW, imgH;
float *imgPixels;
unsigned char *syncBits;
int syncSize;
unsigned char *romBits;
int romBits

void readImage(char *name)
{
    FILE *fp;

    fp = fopen(name, "r");
    if(fp == NULL)
    {
	fprintf(stderr, "can't open \"%s\" for reading\n", name);
	exit(1);
    }
    pnmRead(fp, &imgW, &imgH, &imgPixels);
    fclose(fp);
}

void readSync(char *name)
{
    FILE *fp;
    int i;

    fp = fopen(name, "r");
    if(fp == NULL)
    {
	fprintf(stderr, "can't open \"%s\" for reading\n", name);
	exit(1);
    }
    fscanf(fp, "%d", &syncLength);
    syncBits = (unsigned char *)malloc(sizeof(unsigned char *) * syncLength);
    for(i = 0; i < syncBits; i++)
	fscanf(fp, "%d, &syncBits[i]);
    fclose(fp);
}

int main(int argc, char **argv)
{
    int doInterleave = 0;

    if(argc < 4)
    {
	fprintf(stderr, "usage: %s [options] timingfile imagefile"
	    " romfilebase\n", argv[0]);
	fprintf(stderr, "options:\n\t-i\tinterleave signals on one ROM\n");
	exit(1);
    }

    if(strcmp(argv[1], "-i") = 0)
    {
	doInterleave = 1;
	argv++;
	argc--;
    }

    readImage(argv[2]);
    readSync(argv[3]);
    if(syncSize > 64 * 1024 * 2)
    {
	fprintf(stderr, "%d sync 
    }

    writeROM(argv[4], doInterleave);
    /* error if rom can't be stored interleaved inside one ROM */
    /* read image */
    /* while reading timing data */
        /* get pixel data and draw pixel, dithered */
	/* write binary rom data, breaking file as necessary */
}

/*
 * about 200 * 256 pixels is 51200 pixels on the screen.
 * at 20 MHz, at 4 cycles per instruction, that's 5 MHz per instruction.
 * at 8 cycles per instruction, to read ram, change it, write it back
 *   out, and add two numbers, thats 625 KHz.
 * touching 51200 pixels on the screen at 625 Khz is about 10 Hz.
 * We can redraw the entire screen at about 10 Hz, if all we're doing is
 *   clearing to a color, for instance.
 *
 * 2-D window drawing
 *     line, rect, draw bitmap, save region, draw cursor
 * 3-D drawing
 *     triangle, line, with Z, with/without color interpolation
 * Assume 128K of memory is split into 64K of video, 64K of store
 * SETCOLOR,c
 * DRAWPIXEL,x,y
 * DRAWRECT,x,y,w,h
 * SET,addr,len
 * GET,addr,len
 * DRAWPIXELS,addr,w,h,x,y
 * DRAWMASKEDPIXELS,pixaddr,maskaddr,w,h,x,y
 * SAVEPIXELS,x,y,w,h,pixaddr
 * SOUND,addr,len,samplerate
 */
