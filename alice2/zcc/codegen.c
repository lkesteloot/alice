
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "zcc.h"
#include "y.tab.h"

#define STACKTOP 0xC000
#define LABEL_PREFIX "zcc$"

typedef struct string {
    char label[16];
    char *value; /* Malloc'ed */
    struct string *next;
} STRING_CONST;

static const int INVALID_LABEL = 0;

FILE *outf;
STRING_CONST *string_head = NULL;
int unsupported = 0;

void output_node(NODE *node, int break_label, int continue_label);

static int get_new_label(void)
{
    static int label = INVALID_LABEL + 1;

    return label++;
}

void output_function_indent(NODE *node, int indent)
{
    int i;
 
    if (node == NULL) {
	return;
    }

    for (i = 0; i < indent * 2; i++) {
	printf("  ");
    }

    printf("%s\n", get_op_name(node->op));

    for (i = 0; i < node->numargs; i++) {
	output_function_indent(node->arg[i], indent + 1);
    }
}

void output_code(char *op, char *format, ...)
{
    va_list args;

    fprintf(outf, "\t%s\t", op);

    va_start(args, format);
    vfprintf(outf, format, args);
    va_end(args);

    fprintf(outf, "\n");
}

void output_blank(void)
{
    fprintf(outf, "\n");
}

void output_comment(char *comment, ...)
{
    va_list args;

    fprintf(outf, "\t; ");

    va_start(args, comment);
    vfprintf(outf, comment, args);
    va_end(args);

    fprintf(outf, "\n");
}

void output_label(char *label, ...)
{
    va_list args;

    va_start(args, label);
    vfprintf(outf, label, args);
    va_end(args);

    fprintf(outf, "\n");
}

void output_start(char *outfilename)
{
    outf = fopen(outfilename, "w");
    if (outf == NULL) {
	perror(outfilename);
	exit(EXIT_FAILURE);
    }

#if 0 /* we'll let the host program set the stack */
    output_code("LD", "SP, %d", STACKTOP);
#endif
    output_code("LD", "A, 0");
    output_code("LD", "(SHOULD_RUN), A");
    output_code("CALL", "_main");
    output_code("HALT", "");
    output_blank();
}

void
output_line(int line)
{
    extern char filename[];
    extern char pathname[];
    FILE *f;
    char buf[1024];
    int i;

    if (line <= 0) {
	return;
    }

    f = fopen(pathname, "r");
    if (f != NULL) {
	i = line;
	while (i-- != 0) {
	    fgets(buf, sizeof(buf), f);
	}

        char *s = buf;
	while (isspace(*s)) {
            s++;
	}
	fprintf(outf, "\t; (%s, %d) %s", filename, line, s);

	fclose(f);
    } else {
	fprintf(outf, "\t; (%s, %d)\n", filename, line);
    }
}

int
output_parameters(NODE *node, int break_label, int continue_label)
{
    int stack_size;

    stack_size = 0;

    if (node != NULL) {
	if (node->op == FUNCTION_PARAM) {
	    stack_size += output_parameters(node->arg[1], break_label, continue_label);
	    stack_size += output_parameters(node->arg[0], break_label, continue_label);
	} else {
	    output_node(node, break_label, continue_label);
	    output_code("PUSH", "HL");
	    stack_size = 2;
	}
    }

    return stack_size;
}

void output_node(NODE *node, int break_label, int continue_label)
{
    NODE *n;
    int top, cont, out, bottom, not, tmp1, imm, state;
    int stack_size;
    static int last_line = -1;

    if (node == NULL) {
	return;
    }

    // In case we call yyerror().
    yylineno = node->line;

    if (node->line != last_line && node->op != ';' && node->op != FOR) {
	last_line = node->line;

	/* fprintf(outf, "%d, %s\n", node->op, get_op_name(node->op)); */
	output_line(node->line);
    }

    switch (node->op) {
	case NUMBER:
	    output_code("LD", "HL, %d", node->data.value);
	    break;
	case STRING:
	    output_code("LD", "HL, %s", node->data.address);
	    break;
	case IDENT:
	    if (node->data.var->scope == SCOPE_GLOBAL) {
		if (node->lhs) {
		    output_code("LD", "IX, _%s", node->data.var->name);
		} else if (node->data.var->decl->decl_type == DECL_ARRAY) {
		    output_code("LD", "HL, _%s", node->data.var->name);
		} else {
		    output_code("LD", "HL, (_%s)", node->data.var->name);
		}
	    } else {
		if (node->lhs) {
		    output_code("PUSH", "IY");
		    output_code("POP", "IX");
		    output_code("LD", "BC, %d", node->data.var->offset);
		    output_code("ADD", "IX, BC");
		} else if (node->data.var->decl->decl_type == DECL_ARRAY) {
		    output_code("LD", "BC, %d", node->data.var->offset);
		    output_code("PUSH", "IY");
		    output_code("POP", "HL");
		    output_code("ADD", "HL, BC");
		} else {
		    output_code("LD", "L, (IY + %d)", node->data.var->offset);
		    output_code("LD", "H, (IY + %d)",
			node->data.var->offset + 1);
		}
	    }
	    break;
	case '(':
	    /* limited function call support */
	    stack_size = output_parameters(node->arg[1], break_label, continue_label);
	    output_code("CALL", "_%s", node->arg[0]->data.var->name);
	    while (stack_size-- > 0) {
		output_code("INC", "SP");
	    }
	    break;
	case INCR:
	    output_node(node->arg[0], break_label, continue_label); /* Address is in IX */
	    if (node->data.detail == -1) {  /* Preincrement */
		output_code("LD", "L, (IX + 0)");
		output_code("LD", "H, (IX + 1)");
		output_code("INC", "HL");
		output_code("LD", "(IX + 0), L");
		output_code("LD", "(IX + 1), H");
	    } else {  /* Postincrement */
		output_code("LD", "L, (IX + 0)");
		output_code("LD", "H, (IX + 1)");
		output_code("LD", "E, L");
		output_code("LD", "D, H");
		output_code("INC", "DE");
		output_code("LD", "(IX + 0), E");
		output_code("LD", "(IX + 1), D");
	    }
	    break;
	case DECR:
	    output_node(node->arg[0], break_label, continue_label); /* Address is in IX */
	    if (node->data.detail == -1) {  /* Predecrement */
		output_code("LD", "L, (IX + 0)");
		output_code("LD", "H, (IX + 1)");
		output_code("DEC", "HL");
		output_code("LD", "(IX + 0), L");
		output_code("LD", "(IX + 1), H");
	    } else {  /* Postdecrement */
		output_code("LD", "L, (IX + 0)");
		output_code("LD", "H, (IX + 1)");
		output_code("LD", "E, L");
		output_code("LD", "D, H");
		output_code("DEC", "DE");
		output_code("LD", "(IX + 0), E");
		output_code("LD", "(IX + 1), D");
	    }
	    break;
	case CAST:
	    output_node(node->arg[0], break_label, continue_label);
	    /*
	     * Use get_decl_size() to do the right thing here. Not
	     * necessary right now.
	     */
	    break;
	case SIZEOF:
	    output_code("LD", "HL, %d", get_decl_size(node->arg[0]->decl));
	    break;
	case '*':
	    if (node->numargs == 1) {
		output_node(node->arg[0], break_label, continue_label);
		output_code("PUSH", "HL");
		output_code("POP", "IX");
		if (!node->lhs) {
		    output_code("LD", "H, (IX + 1)");
		    output_code("LD", "L, (IX)");
		}
	    } else {
		if (node->arg[0]->op == NUMBER ||
		    node->arg[1]->op == NUMBER) {

		    if (node->arg[0]->op == NUMBER) {
			n = node->arg[0];
			node->arg[0] = node->arg[1];
			node->arg[1] = n;
		    }

		    output_node(node->arg[0], break_label, continue_label);

		    imm = node->arg[1]->data.value;
		    if (imm < 0) {
			imm = -imm;
			output_code("LD", "A, 255");
			output_code("XOR", "H");
			output_code("XOR", "L");
			output_code("INC", "HL");
		    }

		    if (imm == 0) {
			output_code("LD", "HL, 0");
		    } else if ((imm & (imm - 1)) == 0) { /* Power of two */
			while (imm != 1) {
			    output_code("ADD", "HL, HL");
			    imm >>= 1;
			}
		    } else {
			if ((imm & 1) != 0) {
			    output_code("LD", "D, H");
			    output_code("LD", "E, L");
			    imm &= ~1;
			} else {
			    output_code("LD", "DE, 0");
			}
			state = 0;  /* state = 1 when HL contains the output */
			while (imm != 1) {
			    if ((imm & 1) != 0) {
				if (!state) {
				    output_code("EX", "DE, HL");
				}
				state = 1;
				output_code("ADD", "HL, DE");
			    }
			    if (state) {
				output_code("EX", "DE, HL");
				state = 0;
			    }
			    output_code("ADD", "HL, HL");
			    imm >>= 1;
			}
			/* Doesn't matter what "state" is */
			output_code("ADD", "HL, DE");
		    }
		} else {
		    output_comment("Unsupported operands: %s.",
			get_op_name(node->op));
		    unsupported++;
		}
	    }
	    break;
	case '-':
	    if (node->numargs == 1) {
		output_node(node->arg[0], break_label, continue_label);
		output_code("LD", "A, 255");
		output_code("XOR", "H");
		output_code("XOR", "L");
		output_code("INC", "HL");
	    } else {
		output_node(node->arg[1], break_label, continue_label);
		output_code("PUSH", "HL");
		output_node(node->arg[0], break_label, continue_label);
		output_code("POP", "BC");
		output_code("SCF", "");
		output_code("CCF", "");
		output_code("SBC", "HL, BC");
	    }
	    break;
	case '+':
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "BC");
	    output_code("ADD", "HL, BC");
	    break;
	case '<':
	    tmp1 = get_new_label();
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "DE");
	    output_code("OR", "A ; Clear carry flag");
	    output_code("SBC", "HL, DE ; HL = HL - DE - C");
	    output_code("LD", "HL, 0");
	    output_code("JP", "M, " LABEL_PREFIX "%d", tmp1);
	    output_code("JP", "Z, " LABEL_PREFIX "%d", tmp1);
	    output_code("LD", "HL, 1");
	    output_label(LABEL_PREFIX "%d", tmp1);
	    break;
        case GREATER_EQUALS:
	    tmp1 = get_new_label();
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "DE");
	    output_code("OR", "A ; Clear carry flag");
	    output_code("SBC", "HL, DE ; HL = HL - DE - C");
	    output_code("LD", "HL, 1");
	    output_code("JP", "M, " LABEL_PREFIX "%d", tmp1);
	    output_code("JP", "Z, " LABEL_PREFIX "%d", tmp1);
	    output_code("LD", "HL, 0");
	    output_label(LABEL_PREFIX "%d", tmp1);
	    break;
	case EQUALS:
	    tmp1 = get_new_label();
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "DE");
	    output_code("SCF", "");
	    output_code("CCF", "");
	    output_code("SBC", "HL, DE");
	    output_code("LD", "HL, 1");
	    output_code("JP", "Z, " LABEL_PREFIX "%d", tmp1);
	    output_code("LD", "HL, 0");
	    output_label(LABEL_PREFIX "%d", tmp1);
	    break;
	case NOT_EQUALS:
	    tmp1 = get_new_label();
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "DE");
	    output_code("SCF", "");
	    output_code("CCF", "");
	    output_code("SBC", "HL, DE");
	    output_code("LD", "HL, 0");
	    output_code("JP", "Z, " LABEL_PREFIX "%d", tmp1);
	    output_code("LD", "HL, 1");
	    output_label(LABEL_PREFIX "%d", tmp1);
	    break;
	case '&':
	    if (node->numargs == 1) {
		output_node(node->arg[0], break_label, continue_label);
		output_code("PUSH", "IX");
		output_code("POP", "HL");
	    } else {
		output_node(node->arg[0], break_label, continue_label);
		output_code("PUSH", "HL");
		output_node(node->arg[1], break_label, continue_label);
		output_code("POP", "DE");
		output_code("LD", "A, H");
		output_code("AND", "D");
		output_code("LD", "H, A");
		output_code("LD", "A, L");
		output_code("AND", "E");
		output_code("LD", "L, A");
	    }
	    break;
	case '|':
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("PUSH", "HL");
	    output_node(node->arg[1], break_label, continue_label);
	    output_code("POP", "DE");
	    output_code("LD", "A, H");
	    output_code("OR", "D");
	    output_code("LD", "H, A");
	    output_code("LD", "A, L");
	    output_code("OR", "E");
	    output_code("LD", "L, A");
	    break;
	case '=':
	    output_node(node->arg[1], break_label, continue_label); /* Value is in HL */
	    output_code("PUSH", "HL");
	    output_node(node->arg[0], break_label, continue_label); /* Address is in IX */
	    output_code("POP", "HL");  /* Value is in HL */
	    output_code("LD", "(IX + 0), L");
	    output_code("LD", "(IX + 1), H");
	    break;
	case ',':
	    output_node(node->arg[0], break_label, continue_label);
	    output_node(node->arg[1], break_label, continue_label);
	    break;
	case IF:
	    not = get_new_label();
	    if (node->numargs == 3) {
		bottom = get_new_label();
	    }
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("OR", "A"); // Reset carry.
	    output_code("LD", "DE, 0");
	    output_code("SBC", "HL, DE"); // HL -= DE - carry. NOP except sets Z.
	    output_code("JP", "Z, " LABEL_PREFIX "%d", not);
	    output_node(node->arg[1], break_label, continue_label);
	    if (node->numargs == 3) {
		output_code("JP", LABEL_PREFIX "%d", bottom);
		output_node(node->arg[3], break_label, continue_label);
		output_label(LABEL_PREFIX "%d", bottom);
	    } else {
		output_label(LABEL_PREFIX "%d", not);
	    }
	    break;
	case FOR:
	    top = get_new_label();
	    cont = get_new_label();
	    out = get_new_label();
	    bottom = get_new_label();
	    output_node(node->arg[0], break_label, continue_label);
	    output_code("JP", LABEL_PREFIX "%d", bottom);
	    output_label(LABEL_PREFIX "%d", top);
	    output_node(node->arg[3], out, cont);
	    output_label(LABEL_PREFIX "%d", cont);
	    output_node(node->arg[2], out, cont);
	    output_label(LABEL_PREFIX "%d", bottom);
	    output_node(node->arg[1], out, cont);
	    output_code("OR", "A"); // Reset carry.
	    output_code("LD", "DE, 0");
	    output_code("SBC", "HL, DE"); // HL -= DE - carry. NOP except sets Z.
	    output_code("JP", "NZ, " LABEL_PREFIX "%d", top);
	    output_label(LABEL_PREFIX "%d", out);
	    break;
	case WHILE:
	    top = get_new_label();
	    out = get_new_label();
	    output_label(LABEL_PREFIX "%d", top);
	    output_node(node->arg[0], out, top);
	    output_code("OR", "A"); // Reset carry.
	    output_code("LD", "DE, 0");
	    output_code("SBC", "HL, DE"); // HL -= DE - carry. NOP except sets Z.
	    output_code("JP", "Z, " LABEL_PREFIX "%d", out);
	    output_node(node->arg[1], out, top);
	    output_code("JP", LABEL_PREFIX "%d", top);
	    output_label(LABEL_PREFIX "%d", out);
	    break;
	case DO:
	    top = get_new_label();
	    cont = get_new_label();
	    out = get_new_label();
	    output_label(LABEL_PREFIX "%d", top);
	    output_node(node->arg[0], out, cont);
	    output_label(LABEL_PREFIX "%d", cont);
	    output_node(node->arg[1], out, cont);
	    output_code("OR", "A"); // Reset carry.
	    output_code("LD", "DE, 0");
	    output_code("SBC", "HL, DE"); // HL -= DE - carry. NOP except sets Z.
	    output_code("JP", "NZ, " LABEL_PREFIX "%d", top);
	    output_label(LABEL_PREFIX "%d", out);
	    break;
        case BREAK:
            if (break_label == INVALID_LABEL) {
                yyerror("break not inside loop or switch");
            } else {
                output_code("JP", LABEL_PREFIX "%d", break_label);
            }
            break;
        case CONTINUE:
            if (continue_label == INVALID_LABEL) {
                yyerror("continue not inside loop");
            } else {
                output_code("JP", LABEL_PREFIX "%d", continue_label);
            }
            break;
	case ';':
	    output_node(node->arg[0], break_label, continue_label);
	    output_blank();
	    output_node(node->arg[1], break_label, continue_label);
	    break;
	default:
	    output_comment("Unsupported operator: %s.",
		get_op_name(node->op));
	    unsupported++;
	    break;
    }
}

static void output_globals(void)
{
    VARIABLE *global;

    output_blank();
    for (global = get_global_vars(); global != NULL; global = global->next) {
	if (global->defined) {
	    switch (global->decl->decl_type) {
		case DECL_BASIC_TYPE:
		case DECL_POINTER:
		    fprintf(outf, "_%s\tDW\t0\n", global->name);
		    break;
		case DECL_FUNCTION:
		    /* We've already output this */
		    break;
		case DECL_ARRAY:
		    fprintf(outf, "_%s\tDW\t0\n", global->name); /*XXX*/
		    break;
	    }
	}
    }
}

static void output_strings(void)
{
    STRING_CONST *string;
    char *s;
    int in_string;

    output_blank();
    for (string = string_head; string != NULL; string = string->next) {
	in_string = 0;
	fprintf(outf, "%s\tDB\t", string->label);
	for (s = string->value; *s != '\0'; s++) {
	    if (isprint(*s) && *s != '\"' && *s != '\\') {
		if (!in_string) {
		    fprintf(outf, "\"");
		    in_string = 1;
		}
		fprintf(outf, "%c", *s);
	    } else {
		if (in_string) {
		    fprintf(outf, "\", ");
		    in_string = 0;
		}
		fprintf(outf, "%d, ", (int)*s);
	    }
	}
	if (in_string) {
	    fprintf(outf, "\", ");
	    in_string = 0;
	}
	fprintf(outf, "0\n");
	free(string->value);
    }
}

char *register_string(char *str)
{
    STRING_CONST *string;

    string = malloc(sizeof(STRING_CONST));
    sprintf(string->label, LABEL_PREFIX "%d", get_new_label());
    string->value = strdup(str);
    string->next = string_head;
    string_head = string;

    return string->label;
}

void output_end(void)
{
    output_globals();
    output_strings();
    output_blank();
#if 0  /* leave it to the host program to do this */
    output_code("END", "");
#endif

    fclose(outf);
}

void output_function(char *name, NODE *node)
{
    VARIABLE *var;
    int offset, max;

    /* output_function_indent(node, 0); */

    output_blank();
    output_comment("");
    output_comment("Function \"%s\".", name);
    output_comment("");
    output_label("_%s", name);

    max = 0;
    offset = 0;
    for (var = get_function_vars(); var != NULL; var = var->next) {
	offset += get_decl_size(var->decl);
	if (offset > max) {
	    max = offset;
	}
	var->offset = -offset;
    }

    output_code("PUSH", "IY");
    output_code("LD", "IY, 0");
    output_code("ADD", "IY, SP");
    if (max != 0) {
	output_code("LD", "HL, %d", -max);
	output_code("ADD", "HL, SP");
	output_code("LD", "SP, HL");
	for (var = get_function_vars(); var != NULL; var = var->next) {
	    output_comment("Variable \"%s\" is at location IY - %d",
		var->name, -var->offset);
	}
    }
    output_blank();
    output_node(node, INVALID_LABEL, INVALID_LABEL);
    output_blank();
    output_code("LD", "SP, IY");
    output_code("POP", "IY");
    output_blank();
    output_code("RET", "");
}
