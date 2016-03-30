
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "kern.h"
#include "mem.h"


#define STACK_SIZE 4096
static char proc1_stack[STACK_SIZE];
static char proc2_stack[STACK_SIZE];


int foobar;


int
proc1(void)
{
    int i;

    for (i = 0; i < 5; i++) {
	printf("Proc 1 here (%d)\n", i);
	KERN_sleep(&foobar);
    }

    return 0;
}


int
proc2(void)
{
    int i;

    for (i = 0; i < 5; i++) {
	printf("Proc 2 here (%d)\n", i);
	if (i == 2) {
	    KERN_wakeup(&foobar);
	}
	KERN_resched();
    }

    return 0;
}


void
alarm_handler(int foo)
{
    printf("Alarm\n");
    alarm(2);
    KERN_wakeup(&foobar);
    printf("End alarm\n");
}


static void
test_signal(void)
{
    PROC *init;
    PROC *new_proc;
    struct sigaction action;

    init = KERN_init();

    new_proc = KERN_fork(init);

    action.sa_handler = alarm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    action.sa_restorer = NULL;

    sigaction(SIGALRM, &action, NULL);
/*    alarm(2); */

    KERN_exec(init, proc1, proc1_stack, STACK_SIZE);
    KERN_exec(new_proc, proc2, proc2_stack, STACK_SIZE);

    KERN_resched(); /* shouldn't return */
}


static void
test_mem(void)
{
#define MEM_COUNT 32
    void *foo[MEM_COUNT];
    int i, j;
    int oom_count;

    oom_count = 0;

    for (i = 0; i < MEM_COUNT; i++) {
	foo[i] = NULL;
    }

#if 1
    for (j = 0; j < 1024; j++) {
	i = rand() % MEM_COUNT;

	if (foo[i] != NULL) {
	    MEM_free(foo[i]);
	}

	foo[i] = MEM_alloc(rand()%1000 + 10);
	if (foo[i] == NULL) {
	    oom_count++;
	}
    }
#else
    for (i = 0; i < MEM_COUNT; i++) {
	foo[i] = MEM_alloc(rand()%1024 + 10);
    }
#endif

    for (i = 0; i < MEM_COUNT; i++) {
	if (foo[i] != NULL) {
	    MEM_free(foo[i]);
	}
    }

    printf("Couldn't allocate %d times\n", oom_count);
}


int
main(int argc, char *argv[])
{
    if (0) {
	test_signal();
    }

    if (1) {
	test_mem();
    }

    return 0;
}
