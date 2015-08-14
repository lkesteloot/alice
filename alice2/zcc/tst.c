
#include <stdio.h>
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

#if 1
    char buffer[30];

    while (1) {
	printf("What's your name?\n");
	gets(buffer);
	printf("Hey there %s\n", buffer);
    }
#endif

#if 0
    /* fill the screen with white */
    int i, j;
    char *p;

    for (i = 56; i < 170 + 56; i++) {
	p = (char *)(0x4000 + i*352);

	for (j = 100; j < 200 + 100; j++) {
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
