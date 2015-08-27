
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#if 0
int main()
{
    char *p;
    int i;

    for (i = 0; i < 16384; i++) {
	p = (char *)i;
	*p = i & 0xFF;
    }

    for (i = 0; i < 16384; i++) {
	p = (char *)i;
	if (*p != (i & 0xFF)) {
	    printf("Error at %d\nFix it now!\\\t\n", i);
	}
    }

    for (; i < 16384; i++) {
	p;
    }

    i = i * 5;


    p = "This is\na long\tstring.  Dude.";
}
#else
int
main()
{
#if 0
    int foo[5];

    foo[2] = 1;

    for (;;) {
	printf("This was compiled.\n");
	printf("Cool, eh? 0x%x %d\n", foo[2], foo[2]);
	foo[2] = foo[2] * 2;
    }
#endif
#if 0
    int i;

    i = 1;

    for (;;) {
	printf("This was compiled.\n");
	printf("Cool, eh? 0x%x %d\n", i, i);
	i = i * 2;
    }
#endif

#if 0
    char buffer[30];
    int i, j;

    while (1) {
	printf("What's your name?\n");
	gets(buffer);
	printf("Hey there %s\n", buffer);

        for (j = 0; j < 8; j++) {
            for (i = 0; i < 10000; i++) {
                /* Nothing. */
            }
        }
    }
#endif

#if 0
    int i;

    while (1) {
        i = get_timer_counter();
	printf("Timer %d\n", i);
        for (i = 0; i < 10000; i++) {
            /* Nothing. */
        }
    }
#endif

#if 0
    /* fill the screen with white */
    int i, j;
    char *p;

    p = (char *)(0x4000);
    for (j = 100; j < 200 + 100; j++) {
        p[j] = 0xff;
    }

#endif

#if 1
    /* fill the screen with white */
    int i, j;
    char *p;

    for (i = 200; i >= 100; i--) {
    /* for (i = 100; i < 200; i++) { */
	p = (char *)(0x4000 + i*22);

	for (j = 0; j < 22; j++) {
	    p[j] = 0xff;
	}
    }
#endif

#if 0
    int i, j;
    char *p;

    for (i = 0; i < 160; i++) {
	p = (char *)(0x4000 + i*22);

	for (j = 0; j < 22; j++) {
	    p[j] = 0;
	}
    }

    for (i = 0; i < 4; i++) {
	p = (char *)(0x4000 + (i + 10)*22*4);

	for (j = 0; j < 22; j++) {
	    p[j] = 0xff;
	}
    }
#endif

#if 0
    /* fill the screen with white */
    int i, j;
    char *p;

    for (i = 56; i < 56 + 170; i++) {
	p = (char *)(16396 + i*44);  /* 352/8 */
	for (j = 0; j < 25; j++) {
	    *p = 0xff;
	    p++;
	}
    }

#endif
}
#endif
