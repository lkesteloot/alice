

#ifndef __PROC_H__
#define __PROC_H__


typedef struct proc PROC;
typedef struct proc_list PROC_LIST;


typedef enum {
    STATE_RUN,
    STATE_SLEEP,
    STATE_ZOMBIE
} PROC_STATE;


struct proc {
    int pid;
    int ppid;

    PROC_STATE state;
    void *wait_channel;

    void *sp;

    PROC_LIST *list; /* list it's in */

    PROC *next; /* in process list */
    PROC *prev;
};

struct proc_list {
    PROC *head;
    PROC *tail;
};


PROC *PROC_create(void);
void PROC_destroy(PROC *proc);

void PROC_copy(PROC *src, PROC *dst);


PROC_LIST *PROC_LIST_create(void);
void PROC_LIST_destroy(PROC_LIST *list);

PROC *PROC_LIST_head(PROC_LIST *list);
PROC *PROC_LIST_tail(PROC_LIST *list);

void PROC_LIST_append(PROC_LIST *list, PROC *proc);
void PROC_LIST_prepend(PROC_LIST *list, PROC *proc);
void PROC_LIST_remove(PROC_LIST *list, PROC *proc);

#endif /* __PROC_H__ */
