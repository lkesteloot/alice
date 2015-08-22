

#ifndef __KERN_H__
#define __KERN_H__


#include "proc.h"


extern PROC *cur_proc;


PROC *KERN_init(void);

PROC *KERN_fork(PROC *proc);
void KERN_exec(PROC *proc, int (*func)(void), char *stack, int stack_size);
void KERN_exit(PROC *proc, int return_code);

void KERN_sleep(void *wait_channel);
void KERN_wakeup(void *wait_channel);

void KERN_resched(void);
void KERN_panic(char *string);

#endif /* __KERN_H__ */
