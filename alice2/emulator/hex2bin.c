
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readhex.h"

// Convert a .hex file to a .bin file.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: hex2bin in.hex out.bin\n");
        return 1;
    }

    char *in_filename = argv[1];
    char *out_filename = argv[2];

    FILE *inf = fopen(in_filename, "r");
    if (inf == NULL) {
        perror(in_filename);
        return 1;
    }

    // Assume the ROM we're writing is 16K.
    unsigned char b[16384];
    memset(b, '\0', sizeof(b));

    int success = read_hex(inf, b, sizeof(b), 1);
    fclose(inf);

    if (success) {
        FILE *outf = fopen(out_filename, "wb");
        if (outf == NULL) {
            perror(out_filename);
            return 1;
        }

        fwrite(b, 1, sizeof(b), outf);
        fclose(outf);
    } else {
        // The read_hex() routine will print the error.
        return 1;
    }

    return 0;
}
