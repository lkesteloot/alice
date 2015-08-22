
#include <stdio.h>
#include <unistd.h>

#include "kern.h"
#include "locore.h"


static PROC_LIST *run_queue;
static PROC_LIST *sleep_queue;
static PROC_LIST *zombie_queue;

PROC *cur_proc;
static PROC *init_proc;


static int
new_pid(void)
{
    static int pid = 0;

    return pid++;
}


PROC *
KERN_init(void)
{
    /* create all the different queues.  a process is either
       in one of these queues or it's in "cur_proc" */
     
    run_queue = PROC_LIST_create();
    sleep_queue = PROC_LIST_create();
    zombie_queue = PROC_LIST_create();

    /* no process running */
    cur_proc = NULL;

    /* make the "init" process, parent of all processes */
    init_proc = PROC_create();
    init_proc->pid = new_pid();
    init_proc->state = STATE_RUN;
    PROC_LIST_append(run_queue, init_proc);

    return init_proc;
}


PROC *
KERN_fork(PROC *proc)
{
    PROC *new_proc;

    new_proc = PROC_create();

    PROC_copy(proc, new_proc);
    new_proc->ppid = proc->pid;
    new_proc->pid = new_pid();

    if (proc->list == NULL) {
	/* parent was running */
	PROC_LIST_append(run_queue, new_proc);
    } else {
	PROC_LIST_append(proc->list, new_proc);
    }

    return new_proc;
}


static void
kern_terminated(void)
{
    if (cur_proc != NULL) {
	printf("kernel: process %d terminated\n", cur_proc->pid);

	PROC_destroy(cur_proc);
	cur_proc = NULL;
    } else {
	printf("kernel: unknown process died\n");
    }

    KERN_resched();
}


void
KERN_exec(PROC *proc, int (*func)(void), char *stack, int stack_size)
{
    unsigned long *sp;

    sp = (unsigned long *)(stack + stack_size);

    *--sp = (unsigned long)kern_terminated;
    *--sp = (unsigned long)func;

    *--sp = 0;  /* bp */
    *--sp = 0;  /* ax */
    *--sp = 0;  /* bx */
    *--sp = 0;  /* cx */
    *--sp = 0;  /* dx */

    proc->sp = (void *)sp;
}


void
KERN_exit(PROC *proc, int return_code)
{
    /* now what? */
}


void
KERN_sleep(void *wait_channel)
{
    if (cur_proc == NULL) {
	KERN_panic("sleep() with no cur_proc\n");
    }

    cur_proc->state = STATE_SLEEP;
    cur_proc->wait_channel = wait_channel;

    KERN_resched();
}


void
KERN_wakeup(void *wait_channel)
{
    int need_resched;
    PROC *proc;
    PROC *next;

    need_resched = 0;

    for (proc = PROC_LIST_head(sleep_queue); proc != NULL; proc = next) {

	next = proc->next;

	if (proc->wait_channel == wait_channel) {
	    PROC_LIST_remove(sleep_queue, proc);
	    PROC_LIST_prepend(run_queue, proc);

	    proc->state = STATE_RUN;

	    need_resched = 1;
	}
    }

    if (need_resched) {
	/* perhaps unnecessary, at least now */
/*	KERN_resched(); */
    }
}


void
KERN_resched(void)
{
    PROC *next_proc;
    PROC *old_cur_proc;

#if DEBUG
    printf("Resched\n");

    printf("Processes: ");
    for (next_proc = run_queue->head; next_proc != NULL;
	next_proc = next_proc->next) {

	printf(" %d", next_proc->pid);
    }
    printf("\n");
    if (cur_proc != NULL) {
	printf("Current process: %d\n", cur_proc->pid);
    }
#endif

    /* idle loop */
    do {
	/* get next process to run */
	next_proc = PROC_LIST_head(run_queue);

	if (next_proc == NULL) {
	    if (cur_proc != NULL && cur_proc->state == STATE_RUN) {
		/* only process is already running */
		return;
	    }

/*	    printf("Idle loop\n");
	    sleep(1); */
	}
    } while (next_proc == NULL);

    PROC_LIST_remove(run_queue, next_proc);

    old_cur_proc = cur_proc;
    cur_proc = next_proc;

    if (old_cur_proc == NULL) {
	LOCORE_task_switch(NULL, &next_proc->sp);
    } else {
	switch (old_cur_proc->state) {
	    case STATE_RUN:
		PROC_LIST_append(run_queue, old_cur_proc);
		break;
	    case STATE_SLEEP:
		PROC_LIST_append(sleep_queue, old_cur_proc);
		break;
	    case STATE_ZOMBIE:
		PROC_LIST_append(zombie_queue, old_cur_proc);
		break;
	}
	LOCORE_task_switch(&old_cur_proc->sp, &next_proc->sp);
    }
}


void
KERN_panic(char *string)
{
    printf("kernel panic: %s\n", string);
    exit(1);
}
