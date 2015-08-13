
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "zcc.h"
#include "y.tab.h"

typedef struct SCOPE {
    VARIABLE *var_head;
    struct SCOPE *next;
} SCOPE;

static SCOPE *local = NULL;	/* Pointer to innermost scope */
static SCOPE *function = NULL;	/* Pointer to current function scope */
static SCOPE *global = NULL;	/* Pointer to global scope */

void scope_start(void)
{
    SCOPE *p;

    p = malloc(sizeof(SCOPE));

    p->var_head = NULL;
    p->next = local;

    local = p;

    if (global == NULL) {
	global = p;
    } else if (function == NULL) {
	function = p;
    }
}

void scope_end(void)
{
    SCOPE *q;
    VARIABLE *this, *next;

    assert(local != NULL);

    q = local;

    local = local->next;

    this = q->var_head;
    while (this != NULL) {
	next = this->next;
	free(this);
	this = next;
    }

    free(q);

    if (function == q) {
	function = NULL;
    } else if (global == q) {
	global = NULL;
    }
}

VARIABLE *new_ident(char *name, SPACE space, SCOPE_TYPE scope)
{
    VARIABLE *p;

    if (function == NULL) {
	scope = SCOPE_GLOBAL;
    }

    p = malloc(sizeof(VARIABLE));

    p->storage_class = 0;
    p->decl = NULL;
    p->last_decl = NULL;
    p->name = strdup(name);
    p->offset = 0;
    p->defined = 0;
    p->declared = 0;
    p->space = space;
    p->scope = scope;

    switch (scope) {
	case SCOPE_GLOBAL:
	    p->next = global->var_head;
	    global->var_head = p;
	    break;
	case SCOPE_FUNCTION:
	    p->next = function->var_head;
	    function->var_head = p;
	    break;
	case SCOPE_LOCAL:
	    p->next = local->var_head;
	    local->var_head = p;
	    break;
    }

    return p;
}

VARIABLE *get_ident(char *name, SPACE space)
{
    VARIABLE *p;
    SCOPE *s;

    for (s = local; s != NULL; s = s->next) {
	for (p = s->var_head; p != NULL; p = p->next) {
	    if (p->space == space && strcmp(p->name, name) == 0) {
		return p;
	    }
	}
    }

    return NULL;
}

int same_declarations(DECL *d1, DECL *d2)
{
    if (d1 == NULL && d2 == NULL) {
	return 1;
    }

    if (d1 == NULL || d2 == NULL) {
	return 0;
    }

    if (d1->decl_type != d2->decl_type) {
	return 0;
    }

    switch (d1->decl_type) {
	case DECL_BASIC_TYPE:
	    if (d1->data.type.type != d2->data.type.type ||
		d1->data.type.is_signed != d2->data.type.is_signed) {

		return 0;
	    }
	    break;
	case DECL_POINTER:
	    /* All okay */
	    break;
	case DECL_ARRAY:
	    if (d1->data.length != d2->data.length) {
		return 0;
	    }
	    break;
	case DECL_FUNCTION:
	    /* XXX Compare args */
	    break;
    }

    return same_declarations(d1->next, d2->next);
}

VARIABLE *get_global_vars(void)
{
    return global->var_head;
}

VARIABLE *get_function_vars(void)
{
    return function->var_head;
}

void output_variables(void)
{
#if 0
    VARIABLE *p;
    DECL *d;

    printf("Identifiers:\n");
    for (p = ident_head; p != NULL; p = p->next) {
	printf("    \"%s\" is ", p->name);
	for (d = p->decl; d != NULL; d = d->next) {
	    switch (d->decl_type) {
		case DECL_BASIC_TYPE:
		    if (d->data.type.is_signed) {
			printf("signed ");
		    } else {
			printf("unsigned ");
		    }
		    switch (d->data.type.type) {
			case INT: printf("int "); break;
			case CHAR: printf("char "); break;
		    }
		    break;
		case DECL_POINTER:
		    printf("pointer to ");
		    break;
		case DECL_ARRAY:
		    printf("array of size %d of ", d->data.length);
		    break;
		case DECL_FUNCTION:
		    printf("function returning ");
		    break;
	    }
	}
	printf("\n");
    }
#endif
}
