
#ifndef __ZCC_H__
#define __ZCC_H__

/* The max number of operands per operator: */
#define MAX_ARG	4

/* Max size of an identifier */
#define IDENT_LEN 32

extern int yylineno;
struct VARIABLE;

typedef enum {
    NAME_SPACE_LABEL,
    NAME_SPACE_VARIABLE
} SPACE;

typedef enum {
    SCOPE_GLOBAL,
    SCOPE_FUNCTION,
    SCOPE_LOCAL
} SCOPE_TYPE;

typedef struct {
    unsigned int type : 31;	/* INT, CHAR, ... */
    unsigned int is_signed : 1;
} TYPE;

typedef enum {
    DECL_BASIC_TYPE,
    DECL_POINTER,
    DECL_ARRAY,
    DECL_FUNCTION
} DECL_TYPE;

typedef struct DECL {
    DECL_TYPE decl_type;
    union {
	TYPE type;              /* DECL_BASIC_TYPE */
	int length;             /* DECL_ARRAY */
	struct VARIABLE *args;  /* DECL_FUNCTION */
    } data;
    struct DECL *next;
} DECL;

typedef struct VARIABLE {
    int storage_class;          /* STATIC, EXTERN, ... */
    DECL *decl, **last_decl;
    char *name;
    int offset;   /* From frame pointer */
    int declared;
    int defined;
    SPACE space;
    SCOPE_TYPE scope;

    struct VARIABLE *next;
} VARIABLE;

typedef struct NODE {
    int op;
    int line;

    union {
	int detail;
	int value;
	char *address;
	VARIABLE *var;
    } data;

    DECL *decl;
    int numargs;
    struct NODE *arg[MAX_ARG];
    int lhs : 1;
    int tagged : 1;
} NODE;

extern int unsupported;
void output_start(char *outfilename);
void output_function(char *name, NODE *node);
void output_end(void);
char *register_string(char *str);

char *get_op_name(int op);
void free_tree(NODE *node);
void check_lhs(NODE *node);

void scope_start(void);
void scope_end(void);
VARIABLE *new_ident(char *name, SPACE space, SCOPE_TYPE scope);
VARIABLE *get_ident(char *name, SPACE space);
int same_declarations(DECL *d1, DECL *d2);
void output_variables(void);
VARIABLE *get_global_vars(void);
VARIABLE *get_function_vars(void);

NODE *new_node(int op, int numargs);
void delete_node(NODE *node);
int get_decl_size(DECL *decl);

void yyerror(char *error, ...);
void yywarning(char *warning, ...);
extern char pathname[];  /* In zcc.y */
extern char filename[];  /* In zcc.y */

void simplify(NODE *node);

void init_lex(void);
void check_newfile(void);
void check_newline(void);

#endif /* __ZCC_H__ */
