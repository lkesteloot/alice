#include <stdio.h>


#define LOADDATA	0x10
#define INTRCPU		0x20
#define CLOCKLOW	0x40


unsigned char kbdSequence[4][16] = 
{
    {
	1,
	2 + LOADDATA,
	3 + LOADDATA,
	4 + LOADDATA,
	5 + LOADDATA,
	6 + LOADDATA,
	7 + LOADDATA,
	8 + LOADDATA,
	9 + LOADDATA,
	10,
	11,
	11 + INTRCPU + CLOCKLOW,	/* get into loop if clk keeps going */
	0,
	0,
	0,
	0
    },
    {
	1,
	2 + LOADDATA,
	3 + LOADDATA,
	4 + LOADDATA,
	5 + LOADDATA,
	6 + LOADDATA,
	7 + LOADDATA,
	8 + LOADDATA,
	9 + LOADDATA,
	10,
	10 + INTRCPU + CLOCKLOW,
	0,
	0,
	0,
	0,
	0
    },
    {
	1,
	2,
	3 + LOADDATA,
	4 + LOADDATA,
	5 + LOADDATA,
	6 + LOADDATA,
	7 + LOADDATA,
	8 + LOADDATA,
	9 + LOADDATA,
	10 + LOADDATA,
	11,
	12,
	12 + INTRCPU + CLOCKLOW,
	0,
	0,
	0
    },
    {
	1,
	2,
	3 + LOADDATA,
	4 + LOADDATA,
	5 + LOADDATA,
	6 + LOADDATA,
	7 + LOADDATA,
	8 + LOADDATA,
	9 + LOADDATA,
	10 + LOADDATA,
	11,
	11 + INTRCPU + CLOCKLOW,
	0,
	0,
	0,
	0
    },
};


int main(int argc, char **argv)
{
    int j;
    int i;

    for(j = 0; j < 4; j++)
        for(i = 0; i < 16; i++)
	    putchar(kbdSequence[j][i]);
}
