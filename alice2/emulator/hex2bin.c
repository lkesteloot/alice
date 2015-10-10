#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readhex.h"

// Convert a .hex file to a .bin file.
int main(int argc, char *argv[])
{
    char **argvp = argv + 1;
    int argcp = argc - 1;
    int size = 16384;   // Assume the ROM we're writing is 16K
    int offset = 0;     // At offset 0 within the SRECORDs

    if (argc < 2) {
        fprintf(stderr, "usage: %s [-r OFFSET BYTES] in.hex out.bin\n", argv[0]);
        return 1;
    }
    
    if(strcmp(argvp[0], "-r") == 0) {
        size = strtol(argvp[1], NULL, 0);
        offset = strtol(argvp[2], NULL, 0);
        argvp += 3;
        argcp -= 3;
    }

    char *in_filename = argvp[0];
    char *out_filename = argvp[1];

    FILE *inf = fopen(in_filename, "ra");
    if (inf == NULL) {
        perror(in_filename);
        return 1;
    }

    unsigned char *b = malloc(size);
    if(b == NULL) {
        fprintf(stderr, "couldn't allocated %d bytes\n", size);
        exit(EXIT_FAILURE);
    }
    memset(b, '\377', size);

    struct memory_desc md;
    memory_desc_init(&md, b, offset, size);
    int success = read_hex(inf, memory_desc_store, &md, 1);
    fclose(inf);

    if (success) {
        FILE *outf = fopen(out_filename, "wb");
        if (outf == NULL) {
            perror(out_filename);
            return 1;
        }

        fwrite(b, 1, size, outf);
        fclose(outf);
    } else {
        // The read_hex() routine will print the error.
        return 1;
    }

    return 0;
}
