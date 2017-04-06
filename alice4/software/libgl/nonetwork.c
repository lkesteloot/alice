#include <stdio.h>
#include <time.h>
#include "connection.h"

typedef unsigned char uint8_t;

uint8_t receive_uint8()
{
}

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];
uint8_t buffer_count = 0;

static void write_buffer()
{
#if 0
    if(buffer_count == 0)
        return;

    UINT waswritten;
    FRESULT result = f_write(&capture_file, &buffer, BUFFER_SIZE, &waswritten);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't write buffer to capture file\n");
        panic();
    }
    if(waswritten != BUFFER_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to write %ld bytes, wrote %ld bytes\n", BUFFER_SIZE, waswritten);
        panic();
    }
    buffer_count = 0;
#endif
}

void send_uint8(uint8_t b)
{
#if 0
    buffer[buffer_count++] = b;

    if(buffer_count == BUFFER_SIZE) {
        SERIAL_enqueue_one_char('.'); SERIAL_flush();
        write_buffer();
    }
#endif
}

int count = 0;
time_t previoustime = 0;
int previouscount = 0;

void flush()
{
    count++;
    time_t now = time(0);
    if(now - previoustime > 0) {
	printf("%d frames per second\n", count - previouscount);
	previoustime = now;
	previouscount = count;
    }
#if 0
	SERIAL_flush();
    if(++count > 60) {
        write_buffer();
        f_close(&capture_file);
        f_mount(NULL, "0:", 1);
        printf("finished capturing\n"); SERIAL_flush();
        panic();
    }
#endif
}

void open_connection()
{
    previoustime = time(0);
}

