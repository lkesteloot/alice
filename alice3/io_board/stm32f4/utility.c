#include <stdio.h>
#include <ctype.h>
#include "utility.h"

extern void SERIAL_flush(void);

void dump_buffer_hex(int indent, const unsigned char *data, int size)
{
    int address = 0;
    int i;

    while(size > 0) {
        int howmany = (size < 16) ? size : 16;

        printf("%*s0x%04X: ", indent, "", address);
        for(i = 0; i < howmany; i++)
            printf("%02X ", data[i]);
        printf("\n");
        SERIAL_flush(); // XXX - printf should buffer and block and discard as necessary

        printf("%*s        ", indent, "");
        for(i = 0; i < howmany; i++)
            printf(" %c ", isprint(data[i]) ? data[i] : '.');
        printf("\n");
        SERIAL_flush(); // XXX - printf should buffer and block and discard as necessary

        size -= howmany;
        data += howmany;
        address += howmany;
    }
}

#define BELL '\a'

void bell()
{
    putchar(BELL);
}


