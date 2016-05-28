#include <stdio.h>
#include "logprintf.h"

extern void SERIAL_flush(void); // XXX
extern void VIDEO_output_string(char *c, int inverse); // XXX

extern void errorchar(int c);
extern void errorchar_flush();

int gDebugLevel = DEBUG_WARNINGS;

void logprintf(int level, char *fmt, ...)
{
    va_list args;
    static char dummy[512];

    if(level > gDebugLevel)
        return;

    va_start(args, fmt);
    vsprintf(dummy, fmt, args);
    va_end(args);
    
    char *s = dummy;
    while(*s) {
        errorchar(*s++);
    }
    errorchar_flush();
}


