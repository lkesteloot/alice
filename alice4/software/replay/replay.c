#include <stdio.h>
#include <stdlib.h>
#include "driver.h"
#include "connection.h"

    // Graphics commands (to real FPGA).
const int NO_COMMAND_DEFINED = 0xffffffff;
size_t payload_sizes[256];

int main(int argc, char **argv)
{
    for(int i = 0; i < 256; i++)
        payload_sizes[i] = NO_COMMAND_DEFINED;

    // For commands with constant sized payloads, store the size in bytes of
    // the payload
    payload_sizes[COMMAND_ZBUFFER] = 1;
    payload_sizes[COMMAND_CLEAR] = 3;
    payload_sizes[COMMAND_ZCLEAR] = 0;
    payload_sizes[COMMAND_SWAPBUFFERS] = 0;
    payload_sizes[COMMAND_TRIANGLE] = 36;
    payload_sizes[COMMAND_SET_PATTERN] = 32;
    payload_sizes[COMMAND_ENABLE_PATTERN] = 0;
    payload_sizes[COMMAND_DISABLE_PATTERN] = 0;

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
    unsigned char s;
    unsigned char buffer[512];
    size_t where = 0;

    int frame = 0;
    int triangles = 0;
    size_t prev = 0;

    printf("frame, bytes, triangles\n");

    while(fread(&b, 1, 1, fp) == 1) {
        size_t payload_size;
        
        // Special case of variable-length payload for WINOPEN
        if(b == COMMAND_WINOPEN) {
            if(fread(&s, 1, 1, fp) != 1)
                abort();

            payload_size = s;
        } else {
            payload_size = payload_sizes[b];
        }

        if(payload_size == NO_COMMAND_DEFINED) {
            printf("ERROR: Unknown command byte 0x%02X found in capture at byte %zd\n", b, where);
            break;
        }

        size_t wasread = fread(buffer, 1, payload_size, fp);

        if(wasread < payload_size) {
            // Incomplete command is discarded, and stop trying to parse
            // This happens pretty much for every capture, so don't bother
            // printf("NOTE: Incomplete command received and discarded (probably EOF)\n");
            break;
        }

        // Pass the command byte through
        send_uint8(b);
        where ++;

        // Special case of passing through extra byte for WINOPEN
        if(b == COMMAND_WINOPEN) {
            send_uint8(s);
            where ++;
        }

        // Pass through the payload
        for(int i = 0; i < payload_size; i++)
            send_uint8(buffer[i]);
        where += payload_size;

        // Analyze some things
        if(b == COMMAND_TRIANGLE) {
            triangles++;
        } else if(b == COMMAND_SWAPBUFFERS) {
            printf("%d, %zd, %d\n", frame, where - prev, triangles);
            prev = where;
            triangles = 0;
            frame++;
        }
    }
    flush();
}
