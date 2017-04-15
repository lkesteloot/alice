#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

static char *program_name;

void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s write [-8|-16|-32] address value [count]\n",
    	program_name);
    fprintf(stderr, "    %s read [-8|-16|-32] address [count]\n",
    	program_name);
    fprintf(stderr, "Size defaults to 32, count defaults to 1.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    unsigned int word_size = 32;
    char *end;
    int writing;

    // Skip program name.
    program_name = argv[0];
    argc--;
    argv++;

    // Get command.
    if (argc == 0) {
	usage();
    }
    if (strcmp(argv[0], "write") == 0) {
	writing = 1;
    } else if (strcmp(argv[0], "read") == 0) {
	writing = 0;
    } else {
	usage();
    }
    argc--;
    argv++;

    // Get optional word size.
    if (argc > 0 && argv[0][0] == '-') {
	word_size = strtoul(argv[0] + 1, &end, 0);
	if (*end != '\0') {
	    usage();
	}
	argc--;
	argv++;
    }

    // Get address.
    if (argc == 0) {
	usage();
    }
    uint32_t address = strtoul(argv[0], &end, 0);
    if (*end != '\0') {
	usage();
    }
    argc--;
    argv++;

    // Get value if writing.
    uint32_t value = 0;
    if (writing) {
	if (argc == 0) {
	    usage();
	}
	value = strtoul(argv[0], &end, 0);
	if (*end != '\0') {
	    usage();
	}
	argc--;
	argv++;
    }

    // Get optional count.
    unsigned int count;
    if (argc == 0) {
	count = 1;
    } else {
	count = strtoul(argv[0], &end, 0);
	if (*end != '\0') {
	    usage();
	}
	argc--;
	argv++;
    }

    if (argc != 0) {
	usage();
    }

    int dev_mem = open("/dev/mem", O_RDWR);
    if(dev_mem == 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // The base address must be a multiple of the page size.
    uint32_t base_address = address & 0xFFFFF000;
    uint32_t offset = address - base_address;

    uint8_t *mem = (uint8_t *) mmap(0, offset + count*word_size/8,
	PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, base_address);
    if(mem == 0) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    switch (word_size) {
	case 8: {
	    uint8_t *p = (uint8_t *) (mem + offset);
	    for (int i = 0; i < count; i++) {
		if (writing) {
		    *p++ = (uint8_t) value;
		} else {
		    printf("%p = %x\n", ((uint8_t *) p) + base_address, *p++);
		}
	    }
	    break;
	}

	case 16: {
	    uint16_t *p = (uint16_t *) (mem + offset);
	    for (int i = 0; i < count; i++) {
		if (writing) {
		    *p++ = (uint16_t) value;
		} else {
		    printf("%p = %x\n", ((uint8_t *) p) + base_address, *p++);
		}
	    }
	    break;
	}

	case 32: {
	    uint32_t *p = (uint32_t *) (mem + offset);
	    for (int i = 0; i < count; i++) {
		if (writing) {
		    *p++ = (uint32_t) value;
		} else {
		    printf("%p = %x\n", ((uint8_t *) p) + base_address, *p++);
		}
	    }
	    break;
	}

	default:
	    usage();
	    break;
    }

    exit(EXIT_SUCCESS);
}
