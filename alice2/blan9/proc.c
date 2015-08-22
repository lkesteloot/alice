
#include <stdio.h>

#include "proc.h"
#include "mem.h"


PROC *
PROC_create(void)
{
    PROC *proc;

    proc = (PROC *)MEM_alloc(sizeof(PROC));
    if (proc == NULL) {
	return NULL;
    }

    proc->pid = 0;
    proc->ppid = 0;

    proc->state = STATE_RUN;
    proc->wait_channel = 0;

    proc->sp = NULL;

    proc->list = NULL;

    proc->next = NULL;
    proc->prev = NULL;

    return proc;
}


void
PROC_destroy(PROC *proc)
{
    if (proc->list != NULL) {
	PROC_LIST_remove(proc->list, proc);
    }

    MEM_free(proc);
}


void
PROC_copy(PROC *src, PROC *dst)
{
    dst->pid = src->pid;
    dst->ppid = src->ppid;

    dst->state = src->state;
    dst->wait_channel = src->wait_channel;

    dst->sp = src->sp;

    /* doesn't put it in any list */
}



PROC_LIST *
PROC_LIST_create(void)
{
    PROC_LIST *list;

    list = (PROC_LIST *)MEM_alloc(sizeof(PROC_LIST));
    if (list == NULL) {
	return NULL;
    }

    list->head = NULL;
    list->tail = NULL;

    return list;
}


void
PROC_LIST_destroy(PROC_LIST *list)
{
    while (list->head != NULL) {
	PROC_LIST_remove(list, list->head);
    }

    MEM_free(list);
}


void
PROC_LIST_append(PROC_LIST *list, PROC *proc)
{
    if (list->head == NULL) {
	list->head = proc;
	list->tail = proc;
	proc->prev = NULL;
    } else {
	proc->prev = list->tail;
	if (list->tail != NULL) {
	    list->tail->next = proc;
	}
	list->tail = proc;
    }

    proc->next = NULL;
    proc->list = list;
}


void
PROC_LIST_prepend(PROC_LIST *list, PROC *proc)
{
    if (list->head == NULL) {
	list->head = proc;
	list->tail = proc;
	proc->next = NULL;
    } else {
	proc->next = list->head;
	if (list->head != NULL) {
	    list->head->prev = proc;
	}
	list->head = proc;
    }

    proc->prev = NULL;
    proc->list = list;
}


void
PROC_LIST_remove(PROC_LIST *list, PROC *proc)
{
    if (proc->prev == NULL) {
	list->head = proc->next;
    } else {
	proc->prev->next = proc->next;
    }

    if (proc->next == NULL) {
	list->tail = proc->prev;
    } else {
	proc->next->prev = proc->prev;
    }

    proc->prev = NULL;
    proc->next = NULL;
    proc->list = NULL;
}


PROC *
PROC_LIST_head(PROC_LIST *list)
{
    return list->head;
}


PROC *
PROC_LIST_tail(PROC_LIST *list)
{
    return list->tail;
}
