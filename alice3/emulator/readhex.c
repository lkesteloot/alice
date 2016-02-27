
#include <stdio.h>
#include "readhex.h"

#define RECORD_EXTENDED_LINEAR_ADDRESS 0x04
#define RECORD_SET_EXECUTION_START 0x05

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

int read_hex(FILE *f, memory_func memory, void *memory_arg, int bad_checksum_is_error)
{
    unsigned char *s;
    int base_address = 0x0;
    int start_execution = -1;
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

	if ((byte = get_byte(s)) != 0) {
            s += 2;
            checksum += (unsigned char) byte;

            if(byte == RECORD_SET_EXECUTION_START) {
                start_execution = get_word(s) * 65536 + get_word(s + 4);
                checksum += (unsigned char) get_byte(s);
                checksum += (unsigned char) get_byte(s + 2);
                checksum += (unsigned char) get_byte(s + 4);
                checksum += (unsigned char) get_byte(s + 6);
                s += 8;
            } else if(byte == RECORD_EXTENDED_LINEAR_ADDRESS) {
                base_address = get_word(s) * 65536;
                checksum += (unsigned char) get_byte(s);
                checksum += (unsigned char) get_byte(s + 2);
                s += 4;
            } else {
                printf("record type other than 0 or 4\n");
                return 0;
            }
	} else {
            // byte doesn't affect checksum because it's 0
            s += 2;

            for (i = 0; i < num_bytes; i++) {
                byte = get_byte(s);
                s += 2;

                checksum += (unsigned char) byte;

                memory(memory_arg, base_address + address, byte);

                address++;
            }
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

    if(start_execution >= 0)
        printf("start execution at 0x%08X\n", start_execution);

    return 1;
}

void memory_desc_init(struct memory_desc *mi, unsigned char *p, off_t offset, size_t size)
{
    mi->p = p;
    mi->offset = offset;
    mi->size = size;
    mi->size_written = 0;
}

void memory_desc_store(void *arg, int address, unsigned char c)
{
    struct memory_desc *mi = (struct memory_desc *)arg;
    if(address >= mi->offset && address < mi->offset + mi->size) {
        int offset = address - mi->offset;
        mi->p[offset] = c;
        if (offset + 1 > mi->size_written) {
            mi->size_written = offset + 1;
        }
    }
}

