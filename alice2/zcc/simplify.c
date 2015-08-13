
/*
 * This routine makes some conversions on the syntax tree to simplify it
 * a bit before the next step.  The following conversions are done:
 *
 *   p + i  -->    p + i * sizeof(*p)
 *   p - p  -->    (p - p) / sizeof(*p)
 *   constant operation folding
 */

#include <stdio.h>
#include <malloc.h>

#include "zcc.h"
#include "y.tab.h"

static void simplify_node(NODE *node)
{
    int i, size;
    NODE *n;
    int type1;
    int type2;

    if (node->op == '+' && node->numargs == 2) {
	type1 = node->arg[0]->decl->decl_type;
	type2 = node->arg[1]->decl->decl_type;

	if ((type1 == DECL_POINTER || type1 == DECL_ARRAY) ||
	    (type2 == DECL_POINTER || type2 == DECL_ARRAY)) {

	    /* p + i  -->  p + i * sizeof(*p) */
	    
	    if (type2 == DECL_POINTER || type2 == DECL_ARRAY) {
		n = node->arg[0];
		node->arg[0] = node->arg[1];
		node->arg[1] = n;
	    }

	    size = get_decl_size(node->arg[0]->decl->next);
	    n = new_node('*', 2);
	    n->line = node->line;
	    n->arg[0] = node->arg[1];
	    n->arg[1] = new_node(NUMBER, 0);
	    n->arg[1]->line = node->line;
	    n->arg[1]->data.value = size;
	    n->arg[1]->decl = malloc(sizeof(DECL));
	    n->arg[1]->decl->decl_type = DECL_BASIC_TYPE;
	    n->arg[1]->decl->data.type.type = INT;
	    n->arg[1]->decl->data.type.is_signed = 0;
	    n->arg[1]->decl->next = NULL;

	    node->arg[1] = n;
	}
    } else if (node->op == '-' &&
	node->numargs == 2 &&
	node->arg[0]->decl->decl_type == DECL_POINTER &&
	node->arg[1]->decl->decl_type == DECL_POINTER &&
	!node->tagged) {

	/* p - p  -->  (p - p) / sizeof(*p) */

	size = get_decl_size(node->arg[0]->decl->next);
	if (size != get_decl_size(node->arg[1]->decl->next)) {
	    yyerror("incompatible pointer types in subtraction");
	} else {
	    n = new_node('-', 2);
	    *n = *node;

	    node->arg[0] = n;
	    node->arg[1] = new_node(NUMBER, 0);
	    node->arg[1]->line = node->line;
	    node->arg[1]->data.value = size;
	    node->arg[1]->decl = malloc(sizeof(DECL));
	    node->arg[1]->decl->decl_type = DECL_BASIC_TYPE;
	    node->arg[1]->decl->data.type.type = INT;
	    node->arg[1]->decl->data.type.is_signed = 0;
	    node->arg[1]->decl->next = NULL;

	    node->op = '/';
	    node->decl = malloc(sizeof(DECL));
	    node->decl->decl_type = DECL_BASIC_TYPE;
	    node->decl->data.type.type = INT;
	    node->decl->data.type.is_signed = 1;
	    node->decl->next = NULL;

	    /* Avoid infinite recursion and simplify divide: */
	    node->arg[0]->tagged = 1;
	    simplify_node(node);
	    return;
	}
    } else if (node->numargs == 2 &&
	node->arg[0]->op == NUMBER &&
	node->arg[1]->op == NUMBER &&
	    (node->op == '+' ||
	     node->op == '-' ||
	     node->op == '*' ||
	     node->op == '/')) { /* XXX Add more */

	switch (node->op) {
	    case '+':
		node->data.value = node->arg[0]->data.value +
		    node->arg[1]->data.value;
		break;
	    case '-':
		node->data.value = node->arg[0]->data.value -
		    node->arg[1]->data.value;
		break;
	    case '*':
		/* XXX Handle signed and unsigned */
		node->data.value = node->arg[0]->data.value *
		    node->arg[1]->data.value;
		break;
	    case '/':
		node->data.value = node->arg[0]->data.value /
		    node->arg[1]->data.value;
		break;
	}

	node->op = NUMBER;
	node->numargs = 0;

	delete_node(node->arg[0]);
	delete_node(node->arg[1]);
    }

    for (i = 0; i < node->numargs; i++) {
	simplify_node(node->arg[i]);
    }
}

static void clean_tagged(NODE *node)
{
    int i;

    if (node != NULL) {
	node->tagged = 0;
	for (i = 0; i < node->numargs; i++) {
	    clean_tagged(node->arg[i]);
	}
    }
}

void simplify(NODE *node)
{
    clean_tagged(node);
    simplify_node(node);
}
