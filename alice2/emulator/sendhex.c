#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define ACTUALLY_SEND 1
#define SKIP_ROM 1
#define RAM_START 0x4000
#define SEND_DELAY 20000


#define SER_CMD_SEND	1
#define SER_CMD_RUN	2

int write_count;

int get_nybble(char *s)
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


int get_byte(char *s)
{
    /* this is right, shut up: */
    return get_nybble(s)*16 + get_nybble(s + 1);
}


int get_word(char *s)
{
    /* yes this too */
    return get_byte(s)*256 + get_byte(s + 2);
}


void
send_byte(int f, int byte)
{
    char c;
    int count;

    c = byte;

    /* printf("Sending %d\n", byte); */

    count = write(f, &c, 1);
    if (count == -1) {
	perror("write");
    }
    if (count == 0) {
	fprintf(stderr, "Didn't send the byte.\n");
    }

    /*int status;
    ioctl(f, LPGETSTATUS, &status);
    printf("Status = 0x%02x\n", status); */

    /* usleep(1); */
    {
	int i;

	for(i = 0; i < SEND_DELAY; i++);
    }

    write_count++;
}


int
main(int argc, char *argv[])
{
    FILE *f;
    int device_fd;
    char buf[256];
    char *s;
    int num_bytes;
    int address;
    int checksum;
    int i;
    int byte;
    int do_jump = 0;
    int jump_addr;

    write_count = 0;

    if (argc < 3) {
	printf("usage: sendhex [-j hexaddr] device file.hex\n");
	printf("options:\n");
	printf("\t-j\tJump to hexaddr when finished\n");
	exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "-j") == 0) {
        do_jump = 1;
        jump_addr = strtol(argv[2], NULL, 16);
        argc -= 2;
        argv += 2;
    }

    f = fopen(argv[2], "r");
    if (f == NULL) {
	perror(argv[2]);
	exit(EXIT_FAILURE);
    }

#if ACTUALLY_SEND
    if(argv[1][0] != '-')  {
        device_fd = open(argv[1], O_CREAT | O_WRONLY, 0644 );
        if (device_fd == -1) {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        device_fd = 1;
    }
    /*
    ioctl(device_fd, LPRESET, 0);
    ioctl(device_fd, LPCAREFUL, 0);
    ioctl(device_fd, LPABORT, 1);
    ioctl(device_fd, LPSETIRQ, 0);
    */
#endif

    while (fgets(buf, sizeof(buf), f) != NULL) {
	s = buf;
	if (*s != ':') {
	    printf("Bad format: %s", buf);
	    continue;
	}
	s++;

	num_bytes = get_byte(s);
	if (num_bytes == 0) {
	    break;
	}
	s += 2;

	address = get_word(s);
	s += 4;

	if (get_byte(s) != 0) {
	    break;
	}
	s += 2;

#if SKIP_ROM
	if (address + num_bytes <= RAM_START) {
	    continue;
	}
	if (address < RAM_START) {
	    num_bytes -= RAM_START - address;
	    s += (RAM_START - address)*2;
	    address = RAM_START;
	}
#endif

#if ACTUALLY_SEND
	send_byte(device_fd, SER_CMD_SEND);
	send_byte(device_fd, address & 0xff);
	send_byte(device_fd, (address >> 8) & 0xff);
	send_byte(device_fd, num_bytes);
#endif

	printf("Sending a packet of %d byte%s\n",
	    num_bytes, num_bytes == 1 ? "" : "s");

	checksum = 0;
	for (i = 0; i < num_bytes; i++) {
	    byte = get_byte(s);
	    s += 2;

	    checksum += byte;
	    address++;

#if ACTUALLY_SEND
	    send_byte(device_fd, byte);
#endif
	}

	/* ignore checksum, they don't match anyway */
    }

    fclose(f);

#if ACTUALLY_SEND
    if(do_jump) {
        send_byte(device_fd, SER_CMD_RUN);
        send_byte(device_fd, jump_addr & 0xff);
        send_byte(device_fd, (jump_addr >> 8) & 0xff);
        send_byte(device_fd, 0);
    }
    close(device_fd);
#endif

    printf("Wrote %d bytes to %s.\n", write_count, argv[1]);

    return 0;
}

