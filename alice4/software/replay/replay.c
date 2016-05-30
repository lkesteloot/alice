#include <stdio.h>
#include <stdlib.h>
#include "connection.h"

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "usage: %s capturefile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL) {
        fprintf(stderr, "couldn't open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    open_connection();
    unsigned char b;
    while(fread(&b, 1, 1, fp) == 1)
        send_uint8(b);
    flush();
}
