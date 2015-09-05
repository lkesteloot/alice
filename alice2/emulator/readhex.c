
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

int read_hex_func(FILE *f, memory_func memory, void *memory_arg, int bad_checksum_is_error)
{
    unsigned char *s;
    int num_bytes;
    int address;
    unsigned char checksum;
    int i;
    int byte;
    char line[256];
    int line_number = 0;
    int skipped_bytes = 0;

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

            memory(memory_arg, address, byte);

	    address++;
	}

        // Verify checksum.
        checksum = ~checksum + 1;
        int file_checksum = get_byte(s);
        if (checksum != file_checksum) {
            printf("Checksum mismatch on line %d: %02x vs %02x\n",
                    line_number, checksum, file_checksum);
            if(bad_checksum_is_error)
                return 0;
        }
    }

    if (skipped_bytes > 0) {
        printf("Skipped %d bytes.\n", skipped_bytes);
    }

    return 1;
}

int read_hex(FILE *f, unsigned char buf[], int max_bytes, int bad_checksum_is_error)
{
    unsigned char *s;
    int num_bytes;
    int address;
    unsigned char checksum;
    int i;
    int byte;
    char line[256];
    int line_number = 0;
    int skipped_bytes = 0;

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

            if (address >= max_bytes) {
                skipped_bytes++;
            } else {
                buf[address] = byte;
            }

	    address++;
	}

        // Verify checksum.
        checksum = ~checksum + 1;
        int file_checksum = get_byte(s);
        if (checksum != file_checksum) {
            printf("Checksum mismatch on line %d: %02x vs %02x\n",
                    line_number, checksum, file_checksum);
            if(bad_checksum_is_error)
                return 0;
        }
    }

    if (skipped_bytes > 0) {
        printf("Skipped %d bytes.\n", skipped_bytes);
    }

    return 1;
}

