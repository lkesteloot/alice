
#include <stdio.h>
#include <malloc.h>

#include "zcc.h"
#include "y.tab.h"

NODE *new_node(int op, int numargs)
{
    int i;
    NODE *p;

    p = malloc(sizeof(NODE));

    p->op = op;
    p->line = yylineno;

    p->data.detail = 0;

    p->decl = NULL;

    p->numargs = numargs;
    p->lhs = 0;

    for (i = 0; i < MAX_ARG; i++) {
	p->arg[i] = NULL;
    }

    return p;
}

void delete_node(NODE *node)
{
    if (node == NULL) {
	yyerror("internal error (delete_node got NULL).");
    } else {
	/* Don't delete children -- might have other references */
	free(node);
    }
}

void free_tree(NODE *node)
{
    int i;

    if (node == NULL) {
	return;
    }

    for (i = 0; i < node->numargs; i++) {
	free_tree(node->arg[i]);
    }

    delete_node(node);
}

void check_lhs(NODE *node)
{
    if (node == NULL) {
	return;
    }

    if (node->op == IDENT ||
	(node->op == '*' && node->numargs == 1) ||
	node->op == DEREF_PTR ||
	node->op == DEREF) {

	/* Everything okay */
    } else {
	yyerror("operator expects left-hand-side expression.");
    }

    node->lhs = 1;
}

int get_decl_size(DECL *decl)
{
    int size;

    switch (decl->decl_type) {
	case DECL_BASIC_TYPE:
	    switch (decl->data.type.type) {
		case INT:
		    size = 2;
		    break;
		case CHAR:
		    size = 1;
		    break;
		default:
		    printf("get_decl_size: unknown type %d\n",
			decl->data.type.type);
		    size = 2;
		    break;
	    }
	    break;
	case DECL_POINTER:
	    size = 2;
	    break;
	case DECL_ARRAY:
	    size = decl->data.length * get_decl_size(decl->next);
	    break;
	case DECL_FUNCTION:
	    size = get_decl_size(decl->next);
	    break;
    }

    return size;
}
