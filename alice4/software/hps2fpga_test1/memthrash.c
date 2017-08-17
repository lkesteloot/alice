
// Thrashes memory to test interferance with scanout.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    size_t size = 128*1024*1024;
    void *p = malloc(size);
    if (p == NULL) {
	perror("malloc");
	return 1;
    }

    puts("Thrashing memory . . . (Ctrl-C to stop)");
    int counter;
    while (1) {
	memset(p, counter & 0xFF, size);
	counter++;
    }

    return 0;
}
