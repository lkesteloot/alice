
#include <stdio.h>
#include "readhex.h"

static int get_nybble(unsigned char *s)
{
    int value;

    if (*s >= '0' && *s <= '9') {
	value = *s - '0';
    } else if (*s >= 'a' && *s <= 'f') {
	value = *s - 'a' + 10;
    } else if (*s >= 'A' && *s <= 'F') {
	value = *s - 'A' + 10;
    } else {
	printf("Invalid hex character: %c\n", *s);
	value = 0;
    }

    return value;
}

static int get_byte(unsigned char *s)
{
    /* this is right, shut up: */
    return get_nybble(s)*16 + get_nybble(s + 1);
}

static int get_word(unsigned char *s)
{
    /* yes this too */
    return get_byte(s)*256 + get_byte(s + 2);
}

int read_hex(FILE *f, unsigned char buf[], int max_bytes)
{
    unsigned char *s;
    int num_bytes;
    int address;
    unsigned char checksum;
    int i;
    int byte;
    char line[256];
    int line_number = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
	s = (unsigned char *) line;
        line_number++;

        // Must start with colon.
	if (*s != ':') {
	    printf("Bad format on line %d: %s\n", line_number, line);
            return 0;
	}
	s++;

	checksum = 0;
	num_bytes = get_byte(s);
	if (num_bytes == 0) {
            // All done.
	    break;
	}
        checksum += (unsigned char) num_bytes;
	s += 2;

	address = get_word(s);
        checksum += (unsigned char) get_byte(s);
        checksum += (unsigned char) get_byte(s + 2);
	s += 4;

	if (get_byte(s) != 0) {
            // Done.
	    break;
	}
        // Doesn't affect checksum.
	s += 2;

	for (i = 0; i < num_bytes; i++) {
	    byte = get_byte(s);
	    s += 2;

	    checksum += (unsigned char) byte;

            if (address > max_bytes) {
                printf("Too many bytes in hex file on line %d (%d > %d).\n",
                        line_number, address, max_bytes);
                return 0;
            }

            buf[address] = byte;
	    address++;
	}

        // Verify checksum.
        checksum = ~checksum + 1;
        int file_checksum = get_byte(s);
        if (checksum != file_checksum) {
            printf("Checksum mismatch on line %d: %02x vs %02x\n",
                    line_number, checksum, file_checksum);
            return 0;
        }
    }

    return 1;
}

