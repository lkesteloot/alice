%expect 1  /* Dangling 'else' */

%{

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "zcc.h"

#define YYDEBUG 1

extern int yylex(void);

int yylineno;
extern FILE *yyin;
int error_return;

char pathname[1024], filename[1024], outfilename[1024];

int cur_class = -1;
TYPE cur_type;
int already_report_once = 0;

%}

%union {
    int	number;
    int storage_class;
    char ident[IDENT_LEN];
    char *string;
    NODE *node;
    TYPE type;
    VARIABLE *variable;
}

%token<ident> IDENT
%token<number> NUMBER
%token<string> STRING
%token<node> RETURN IF FOR WHILE DO ELSE BREAK CONTINUE GOTO DEFAULT CASE SWITCH
%token STATIC EXTERN VOLATILE AUTO REGISTER DEFAULT
%token INT CHAR SIGNED UNSIGNED VOID
%token FUNCTION_PARAM VARIABLE_NODE

/* Lowest to highest precedence: */

%type<node> expression    /* comma */
%type<node> expr_assign
%type<node> expr_ternary
%type<node> expr_log_or
%type<node> expr_log_and
%type<node> expr_or
%type<node> expr_xor
%type<node> expr_and
%type<node> expr_equality
%type<node> expr_inequality
%type<node> expr_shift
%type<node> expr_sum
%type<node> expr_factor
%type<node> expr_unary
%type<node> expr_deref
%type<node> expr_item

%type<node> stats stat poss_expr func_params non_empty_func_params
%type<node> arg_decls non_empty_arg_decls arg_decl

%type<type> type

%type<variable> decl
%type<variable> decl_deref
%type<variable> decl_item
%type<variable> cast_decl
%type<variable> cast_decl_deref
%type<variable> cast_decl_item
%type<variable> cast_decl_item_nonempty

%type<storage_class> storage_class

%right UNARY INCR DECR

%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIV_ASSIGN MOD_ASSIGN
%token OR_ASSIGN AND_ASSIGN XOR_ASSIGN SHIFT_LEFT_ASSIGN SHIFT_RIGHT_ASSIGN
%token LOG_OR LOG_AND EQUALS NOT_EQUALS
%token LESS_EQUALS GREATER_EQUALS SHIFT_LEFT SHIFT_RIGHT
%token SIZEOF DEREF DEREF_PTR CAST DOTDOTDOT

%%

start
	: /* Nothing */
	| start top_level
	;

top_level
	: var_def
	| func_def
	;

var_def
	: class_type decls ';'
	  {
	      /* reset type -- it's set by "class_type" and used
	         by "decls" */
	      cur_class = DEFAULT;
	      cur_type.type = INT;
	      cur_type.is_signed = 1;
	  }
	;

func_def
	: class_type decl '{' { scope_start(); } defs stats '}'
	  {
	      simplify($6);
	      output_function($2->name, $6);
	      free_tree($6);
	      output_variables();
	      scope_end();
	  }
	;

defs
	: /* Nothing */
	| defs var_def
	;

storage_class
	: STATIC     { $$ = STATIC; }
	| EXTERN     { $$ = EXTERN; }
	| VOLATILE   { $$ = VOLATILE; }
	| AUTO       { $$ = AUTO; }
	| REGISTER   { $$ = REGISTER; }
	;

type
	: INT            { $$.type = INT;  $$.is_signed = 1; }
	| UNSIGNED       { $$.type = INT;  $$.is_signed = 0; }
	| SIGNED         { $$.type = INT;  $$.is_signed = 1; }
	| UNSIGNED INT   { $$.type = INT;  $$.is_signed = 0; }
	| SIGNED INT     { $$.type = INT;  $$.is_signed = 1; }
	| CHAR           { $$.type = CHAR; $$.is_signed = 0; }
	| UNSIGNED CHAR  { $$.type = CHAR; $$.is_signed = 0; }
	| SIGNED CHAR    { $$.type = CHAR; $$.is_signed = 1; }
	;

class_type:
	  storage_class type
	  {
	      cur_class = $1;
	      cur_type = $2;
	  }
	| storage_class
	  {
	      cur_class = $1;
	      cur_type.type = INT;
	      cur_type.is_signed = 1;
	  }
	| type
	  {
	      cur_class = DEFAULT;
	      cur_type = $1;
	  }
	;

decls
	: decl {}
	| decls ',' decl {}
	;

decl
	: decl_deref
	| '*' decl
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_POINTER;
	      dec->next = *$2->last_decl;

	      *$2->last_decl = dec;
	      $2->last_decl = &dec->next;
	      $$ = $2;
	  }
	;

decl_deref
	: decl_item
	| decl_item '(' arg_decls ')'
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_FUNCTION;
	      dec->data.args = NULL;
	      dec->next = *$1->last_decl;

	      *$1->last_decl = dec;
	      $1->last_decl = &dec->next;
	      $$ = $1;
	  }
	| decl_deref '[' NUMBER ']'
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_ARRAY;
	      dec->data.length = $3;
	      dec->next = *$1->last_decl;

	      *$1->last_decl = dec;
	      $1->last_decl = &dec->next;
	      $$ = $1;
	  }
	;

decl_item
	: IDENT
	  {
	      VARIABLE *var;
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_BASIC_TYPE;
	      dec->data.type = cur_type;
	      dec->next = NULL;

	      var = new_ident($1, NAME_SPACE_VARIABLE, SCOPE_LOCAL);
	      var->storage_class = cur_class;
	      var->decl = dec;
	      var->last_decl = &var->decl;
	      var->declared = 1;
	      var->defined = (cur_class != EXTERN);

	      $$ = var;
	  }
	| '(' decl ')'
	  {
	      $$ = $2;
	  }
	;

cast_decl:
	  cast_decl_deref
	| '*' cast_decl
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_POINTER;
	      dec->next = *$2->last_decl;

	      *$2->last_decl = dec;
	      $2->last_decl = &dec->next;
	      $$ = $2;
	  }
	;

cast_decl_deref:
	  cast_decl_item
	| cast_decl_item_nonempty '(' arg_decls ')'
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_FUNCTION;
	      dec->data.args = NULL;
	      dec->next = *$1->last_decl;

	      *$1->last_decl = dec;
	      $1->last_decl = &dec->next;
	      $$ = $1;
	  }
	| cast_decl_deref '[' NUMBER ']'
	  {
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_ARRAY;
	      dec->data.length = $3;
	      dec->next = *$1->last_decl;

	      *$1->last_decl = dec;
	      $1->last_decl = &dec->next;
	      $$ = $1;
	  }
	;

cast_decl_item:
	  {
	      VARIABLE *var;
	      DECL *dec;

	      dec = malloc(sizeof(DECL));
	      dec->decl_type = DECL_BASIC_TYPE;
	      dec->data.type = cur_type;
	      dec->next = NULL;

	      var = new_ident("", NAME_SPACE_VARIABLE, SCOPE_LOCAL);
	      var->storage_class = cur_class;
	      var->decl = dec;
	      var->last_decl = &var->decl;
	      var->declared = 1;
	      var->defined = (cur_class != EXTERN);

	      $$ = var;
	  }
	| cast_decl_item_nonempty
	;

cast_decl_item_nonempty:
	  '(' cast_decl ')'
	  {
	      $$ = $2;
	  }
	;

arg_decls
	: { $$ = NULL; }
	| VOID { $$ = NULL; }
	| non_empty_arg_decls
	;

non_empty_arg_decls:
	  arg_decl
	| non_empty_arg_decls ',' arg_decl
	  {
	      $$ = new_node(',', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	;

arg_decl:
	  class_type decl
	  {
	      $$ = new_node(VARIABLE_NODE, 0);
	      $$->data.var = $2;
	  }
	| decl
	  {
	      $$ = new_node(VARIABLE_NODE, 0);
	      $$->data.var = $1;
	  }
	| DOTDOTDOT
	  {
	      $$ = new_node(DOTDOTDOT, 0);
	  }
	;

stats:
	  { $$ = NULL; }
	| stats stat
	  {
	      if ($1 == NULL) {
		  $$ = $2;
	      } else {
		  $$ = new_node(';', 2);
		  $$->arg[0] = $1;
		  $$->arg[1] = $2;
	      }
	  }
	;

stat:
	  RETURN ';'
	  {
	      $$ = new_node(RETURN, 0);
	  }
	| RETURN expression ';'
	  {
	      $$ = new_node(RETURN, 1);
	      $$->arg[0] = $2;
	  }
	| FOR '(' poss_expr ';' poss_expr ';' poss_expr ')' stat
	  {
	      $$ = new_node(FOR, 4);
	      $$->arg[0] = $3;
	      $$->arg[1] = $5;
	      $$->arg[2] = $7;
	      $$->arg[3] = $9;
	  }
	| WHILE '(' expression ')' stat
	  {
	      $$ = new_node(WHILE, 2);
	      $$->arg[0] = $3;
	      $$->arg[1] = $5;
	  }
	| DO stat WHILE '(' expression ')' ';'
	  {
	      $$ = new_node(DO, 2);
	      $$->arg[0] = $2;
	      $$->arg[1] = $5;
	  }
	| IF '(' expression ')' stat
	  {
	      $$ = new_node(IF, 2);
	      $$->arg[0] = $3;
	      $$->arg[1] = $5;
	  }
	| IF '(' expression ')' stat ELSE stat
	  {
	      $$ = new_node(IF, 3);
	      $$->arg[0] = $3;
	      $$->arg[1] = $5;
	      $$->arg[2] = $7;
	  }
	| SWITCH '(' expression ')' stat
	  {
	      $$ = new_node(SWITCH, 2);
	      $$->arg[0] = $3;
	      $$->arg[1] = $5;
	  }
	| CASE expression ':'
	  {
	      $$ = new_node(CASE, 1);
	      $$->arg[0] = $2;
	  }
	| DEFAULT ':'
	  {
	      $$ = new_node(DEFAULT, 0);
	  }
	| IDENT ':'
	  {
	      VARIABLE *var;
	      
	      var = get_ident($1, NAME_SPACE_LABEL);
	      if (var == NULL || !var->defined) {
		  if (var == NULL) {
		      var = new_ident($1, NAME_SPACE_LABEL, SCOPE_FUNCTION);
		  }
		  var->defined = 1;
		  $$ = new_node(CASE, 0);
		  $$->data.var = var;
	      } else {
		  yyerror("duplicate label \"%s\".", $1);
	      }
	  }
	| GOTO IDENT ';'
	  {
	      VARIABLE *var;
	      
	      var = get_ident($2, NAME_SPACE_LABEL);
	      if (var == NULL) {
		  var = new_ident($2, NAME_SPACE_LABEL, SCOPE_FUNCTION);
		  var->defined = 0;
	      }
	      $$ = new_node(GOTO, 0);
	      $$->data.var = var;
	  }
	| BREAK ';'
	  {
	      $$ = new_node(BREAK, 0);
	  }
	| CONTINUE ';'
	  {
	      $$ = new_node(CONTINUE, 0);
	  }
	| expression ';'
	  {
	      $$ = $1;
	  }
	| ';'
	  {
	      $$ = NULL;
	  }
	| '{' { scope_start(); } defs stats '}'
	  {
	      $$ = $4;
	      scope_end();
	  }
	;

poss_expr:
	  {
	      $$ = new_node(NUMBER, 0);
	      $$->data.value = 1;
	      $$->decl = malloc(sizeof(DECL));
	      $$->decl->decl_type = DECL_BASIC_TYPE;
	      $$->decl->data.type.type = INT;
	      $$->decl->data.type.is_signed = 1;
	      $$->decl->next = NULL;
	  }
	| expression
	  {
	      $$ = $1;
	  }
	;

expression:
	  expr_assign
	| expression ',' expr_assign
	  {
	      $$ = new_node(',', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	;

expr_assign:
	  expr_ternary
	| expr_ternary '=' expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '=' operator");
	      } else if ($1->decl->decl_type == DECL_POINTER) {
		  if (!same_declarations($1->decl, $3->decl)) {
		      yywarning("assignment from incompatible pointer type");
		  }
	      }
	      check_lhs($1);
	      $$ = new_node('=', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary PLUS_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '+=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(PLUS_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary MINUS_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '-=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(MINUS_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary TIMES_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '*=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(TIMES_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary DIV_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '/=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(DIV_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary MOD_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '%=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(MOD_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary AND_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '&=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(AND_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary XOR_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '^=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(XOR_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary OR_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '|=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(OR_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary SHIFT_LEFT_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '<<=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(SHIFT_LEFT_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	| expr_ternary SHIFT_RIGHT_ASSIGN expr_assign
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '>>=' operator");
	      }
	      check_lhs($1);
	      $$ = new_node(SHIFT_RIGHT_ASSIGN, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $3->decl;
	  }
	;

expr_ternary:
	  expr_log_or
	| expr_log_or '?' expr_ternary ':' expr_ternary
	  {
	      $$ = new_node('?', 3);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->arg[2] = $5;
	      $$->decl = $3->decl; /* XXX promote to $3 or $5 */
	  }
	;

expr_log_or:
	  expr_log_and
	| expr_log_or LOG_OR expr_log_and
	  {
	      if ($1->decl->decl_type == DECL_FUNCTION ||
		  $3->decl->decl_type == DECL_FUNCTION) {

		  yyerror("illegal operands to '||' operator");
	      }
	      $$ = new_node(LOG_OR, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_log_and:
	  expr_or
	| expr_log_and LOG_AND expr_or
	  {
	      if ($1->decl->decl_type == DECL_FUNCTION ||
		  $3->decl->decl_type == DECL_FUNCTION) {

		  yyerror("illegal operands to '&&' operator");
	      }
	      $$ = new_node(LOG_AND, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_or:
	  expr_xor
	| expr_or '|' expr_xor
	  {
	      if ($1->decl->decl_type != DECL_BASIC_TYPE ||
		  $3->decl->decl_type != DECL_BASIC_TYPE) {

		  yyerror("illegal operands to '|' operator");
	      }
	      $$ = new_node('|', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_xor:
	  expr_and
	| expr_xor '^' expr_and
	  {
	      if ($1->decl->decl_type != DECL_BASIC_TYPE ||
		  $3->decl->decl_type != DECL_BASIC_TYPE) {

		  yyerror("illegal operands to '^' operator");
	      }
	      $$ = new_node('^', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_and:
	  expr_equality
	| expr_and '&' expr_equality
	  {
	      if ($1->decl->decl_type != DECL_BASIC_TYPE ||
		  $3->decl->decl_type != DECL_BASIC_TYPE) {

		  yyerror("illegal operands to '&' operator");
	      }
	      $$ = new_node('&', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_equality:
	  expr_inequality
	| expr_equality EQUALS expr_inequality
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '==' operator");
	      }
	      $$ = new_node(EQUALS, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_equality NOT_EQUALS expr_inequality
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '!=' operator");
	      }
	      $$ = new_node(NOT_EQUALS, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_inequality:
	  expr_shift
	| expr_inequality '<' expr_shift
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '<' operator");
	      }
	      $$ = new_node('<', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_inequality '>' expr_shift
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '>' operator");
	      }
	      $$ = new_node('>', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_inequality LESS_EQUALS expr_shift
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '<=' operator");
	      }
	      $$ = new_node(LESS_EQUALS, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_inequality GREATER_EQUALS expr_shift
	  {
	      if ($1->decl->decl_type !=
		  $3->decl->decl_type) {

		  yyerror("illegal operands to '>=' operator");
	      }
	      $$ = new_node(GREATER_EQUALS, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_shift:
	  expr_sum
	| expr_shift SHIFT_LEFT expr_sum
	  {
	      if (($1->decl->decl_type != DECL_BASIC_TYPE) ||
		  ($3->decl->decl_type != DECL_BASIC_TYPE)) {
		  
		  yyerror("illegal operands to '<<' operator");
	      }
	      $$ = new_node(SHIFT_LEFT, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	| expr_shift SHIFT_RIGHT expr_sum
	  {
	      if (($1->decl->decl_type != DECL_BASIC_TYPE) ||
		  ($3->decl->decl_type != DECL_BASIC_TYPE)) {
		  
		  yyerror("illegal operands to '>>' operator");
	      }
	      $$ = new_node(SHIFT_RIGHT, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	;

expr_sum:
	  expr_factor
	| expr_sum '+' expr_factor
	  {
	      $$ = new_node('+', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;

	      if ($1->decl->decl_type == DECL_BASIC_TYPE &&
		   $3->decl->decl_type == DECL_BASIC_TYPE) {

		  $$->decl = $1->decl;
	      } else if ($1->decl->decl_type == DECL_POINTER &&
		   $3->decl->decl_type == DECL_BASIC_TYPE) {

		  $$->decl = $1->decl;
              } else if ($1->decl->decl_type == DECL_BASIC_TYPE &&
		   $3->decl->decl_type == DECL_POINTER) {

		  $$->decl = $3->decl;
	      } else {
		  yyerror("illegal operands to '+' operator");
	      }
	  }
	| expr_sum '-' expr_factor
	  {
	      $$ = new_node('-', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;

	      if ($1->decl->decl_type == DECL_BASIC_TYPE &&
		   $3->decl->decl_type == DECL_BASIC_TYPE) {

		  $$->decl = $1->decl;
	      } else if ($1->decl->decl_type == DECL_POINTER &&
		   $3->decl->decl_type == DECL_POINTER) {

		  $$->decl = malloc(sizeof(DECL));
		  $$->decl->decl_type = DECL_BASIC_TYPE;
		  $$->decl->data.type.type = INT;
		  $$->decl->data.type.is_signed = 0;
		  $$->decl->next = NULL;
	      } else {
		  yyerror("illegal operands to '+' operator");
	      }
	  }
	;

expr_factor:
	  expr_unary
	| expr_factor '*' expr_unary
	  {
	      if (($1->decl->decl_type != DECL_BASIC_TYPE) ||
		  ($3->decl->decl_type != DECL_BASIC_TYPE)) {
		  
		  yyerror("illegal operands to '*' operator");
	      }
	      $$ = new_node('*', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_factor '/' expr_unary
	  {
	      if (($1->decl->decl_type != DECL_BASIC_TYPE) ||
		  ($3->decl->decl_type != DECL_BASIC_TYPE)) {
		  
		  yyerror("illegal operands to '/' operator");
	      }
	      $$ = new_node('/', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	| expr_factor '%' expr_unary
	  {
	      if (($1->decl->decl_type != DECL_BASIC_TYPE) ||
		  ($3->decl->decl_type != DECL_BASIC_TYPE)) {
		  
		  yyerror("illegal operands to '%' operator");
	      }
	      $$ = new_node('%', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl;
	  }
	;

expr_unary:
	  expr_deref
	| expr_unary INCR %prec UNARY
	  {
	      check_lhs($1);
	      $$ = new_node(INCR, 1);
	      $$->data.detail = -1;
	      $$->arg[0] = $1;
	      $$->decl = $1->decl;
	  }
	| expr_unary DECR %prec UNARY
	  {
	      check_lhs($1);
	      $$ = new_node(DECR, 1);
	      $$->data.detail = -1;
	      $$->arg[0] = $1;
	      $$->decl = $1->decl;
	  }
	| INCR expr_unary %prec UNARY
	  {
	      check_lhs($2);
	      $$ = new_node(INCR, 1);
	      $$->data.detail = 1;
	      $$->arg[0] = $2;
	      $$->decl = $2->decl;
	  }
	| DECR expr_unary %prec UNARY
	  {
	      check_lhs($2);
	      $$ = new_node(DECR, 1);
	      $$->data.detail = 1;
	      $$->arg[0] = $2;
	      $$->decl = $2->decl;
	  }
	| '!' expr_unary %prec UNARY
	  {
	      $$ = new_node('!', 1);
	      $$->arg[0] = $2;
	      $$->decl = $2->decl;
	  }
	| '~' expr_unary %prec UNARY
	  {
	      $$ = new_node('~', 1);
	      $$->arg[0] = $2;
	      $$->decl = $2->decl;
	  }
	| '*' expr_unary %prec UNARY
	  {
	      if ($2->decl->decl_type != DECL_POINTER) {
		  yyerror("dereferenced expression is not a pointer");
	      }
	      $$ = new_node('*', 1);
	      $$->arg[0] = $2;
	      $$->decl = $2->decl->next;
	  }
	| '&' expr_unary %prec UNARY
	  {
	      if ($2->op == '*' && $2->numargs == 1) {
		  $$ = $2->arg[0];
		  delete_node($2);
	      } else {
		  check_lhs($2);
		  $$ = new_node('&', 1);
		  $$->arg[0] = $2;
		  $$->decl = malloc(sizeof(DECL));
		  $$->decl->decl_type = DECL_POINTER;
		  $$->decl->next = $2->decl;
	      }
	  }
	| '-' expr_unary %prec UNARY
	  {
	      if ($2->op == NUMBER) {
		  $2->data.value = -$2->data.value;
		  $$ = $2;
	      } else {
		  $$ = new_node('-', 1);
		  $$->arg[0] = $2;
		  $$->decl = $2->decl;
	      }
	  }
	| '+' expr_unary %prec UNARY
	  {
	      $$ = $2;
	  }
	| SIZEOF expr_unary %prec UNARY
	  {
	      /* XXX should also be able to take types */
	      $$ = new_node(SIZEOF, 1);
	      $$->arg[0] = $2;
	      $$->decl = malloc(sizeof(DECL));
	      $$->decl->decl_type = DECL_BASIC_TYPE;
	      $$->decl->data.type.type = INT;
	      $$->decl->data.type.is_signed = 0;
	      $$->decl->next = NULL;
	  }
	| '(' type { cur_type = $2; } cast_decl ')' expr_unary %prec UNARY
	  {
	      $$ = new_node(CAST, 1);
	      $$->arg[0] = $6;
	      $$->decl = $4->decl;
	  }
	;

expr_deref:
	  expr_item
	| expr_deref '[' expression ']'
	  {
	      int type1, type2;

	      type1 = $1->decl->decl_type;
	      type2 = $3->decl->decl_type;

	      if ((type1 == DECL_POINTER || type1 == DECL_ARRAY) ==
		  (type2 == DECL_POINTER || type2 == DECL_ARRAY)) {

		  yyerror("illegal array dereference");
	      }
	      if (type1 != DECL_BASIC_TYPE &&
		  type2 != DECL_BASIC_TYPE) {

		  yyerror("illegal array dereference");
	      }
	      $$ = new_node('*', 1);
	      $$->arg[0] = new_node('+', 2);
	      if (type1 == DECL_BASIC_TYPE) {
		  $$->decl = $3->decl->next;
		  $$->arg[0]->arg[0] = $3;
		  $$->arg[0]->arg[1] = $1;
	      } else {
		  $$->decl = $1->decl->next;
		  $$->arg[0]->arg[0] = $1;
		  $$->arg[0]->arg[1] = $3;
	      }
	  }
	| expr_item '(' func_params ')'
	  {
	      if ($1->decl->decl_type != DECL_FUNCTION) {
		  yyerror("illegal function call");
	      }

	      $$ = new_node('(', 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	      $$->decl = $1->decl->next;
	  }
	  /*
	| expr_deref DEREF IDENT
	  {
	      $$ = new_node(DEREF, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	| expr_deref DEREF_PTR IDENT
	  {
	      $$ = new_node(DEREF_PTR, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	  */
	;

expr_item:
	  IDENT
	  {
	      VARIABLE *var;

	      var = get_ident($1, NAME_SPACE_VARIABLE);

	      if (var == NULL) {
		  if (yychar == '(') {
		      yywarning("implicit declaration of function `%s'", $1);
		      var = new_ident($1, NAME_SPACE_VARIABLE, SCOPE_GLOBAL);
		      var->decl = malloc(sizeof(DECL));
		      var->decl->decl_type = DECL_FUNCTION;
		      var->decl->data.args = NULL;
		  } else {
		      yyerror("`%s' undeclared (first use this function)", $1);
		      if (!already_report_once) {
			  yyerror("(Each undeclared identifier is reported "
			      "only once");
			  yyerror("for each function it appears in.)");
			  already_report_once = 1;
		      }
		      var = new_ident($1, NAME_SPACE_VARIABLE, SCOPE_FUNCTION);
		      var->decl = malloc(sizeof(DECL));
		      var->decl->decl_type = DECL_BASIC_TYPE;
		      var->decl->data.type.type = INT;
		      var->decl->data.type.is_signed = 1;
		  }
	      }

	      $$ = new_node(IDENT, 0);
	      $$->data.var = var;
	      $$->decl = var->decl;
	      /* XXX Convert arrays to pointers */
	  }
	| NUMBER
	  {
	      $$ = new_node(NUMBER, 0);
	      $$->data.value = $1;
	      $$->decl = malloc(sizeof(DECL));
	      $$->decl->decl_type = DECL_BASIC_TYPE;
	      $$->decl->data.type.type = INT;
	      $$->decl->data.type.is_signed = 1;
	      $$->decl->next = NULL;
	  }
	| STRING
	  {
	      $$ = new_node(STRING, 0);
	      $$->data.address = register_string($1);
	      $$->decl = malloc(sizeof(DECL));
	      $$->decl->decl_type = DECL_POINTER;
	      $$->decl->next = malloc(sizeof(DECL));
	      $$->decl->next->decl_type = DECL_BASIC_TYPE;
	      $$->decl->next->data.type.type = CHAR;
	      $$->decl->next->data.type.is_signed = 0;
	      $$->decl->next->next = NULL;
	  }
	| '(' expression ')'
	  {
	      $$ = $2;
	  }
	;

func_params:
	  { $$ = NULL; }
	| non_empty_func_params
	;

non_empty_func_params:
	  expr_assign
	  {
	      $$ = $1;
	  }
	| non_empty_func_params ',' expr_assign
	  {
	      $$ = new_node(FUNCTION_PARAM, 2);
	      $$->arg[0] = $1;
	      $$->arg[1] = $3;
	  }
	;
%%

void yyerror(char *error, ...)
{
    va_list args;
    char buf[1024];
    extern char *yytext;

    fprintf(stderr, "%s:%d: ", filename, yylineno);

    if (strcmp(error, "parse error") == 0) {
	sprintf(buf, "parse error before `%s'", yytext);
	error = buf;
    }

    va_start(args, error);
    vfprintf(stderr, error, args);
    va_end(args);

    fprintf(stderr, "\n");

    error_return = 1;
}

void yywarning(char *warning, ...)
{
    va_list args;

    fprintf(stderr, "%s:%d: warning: ", filename, yylineno);

    va_start(args, warning);
    vfprintf(stderr, warning, args);
    va_end(args);

    fprintf(stderr, "\n");
}

int yywrap(void)
{
    return 1;
}

char *get_op_name(int op)
{
    return (char *)yytname[YYTRANSLATE(op)];
}

void get_include_path(char *exe_pathname, char *include_path)
{
    strcpy(include_path, exe_pathname);

    char *c = strrchr(include_path, '/');
    if (c == NULL) {
        strcpy(include_path, ".");
    } else {
        *c = '\0';
    }
    strcat(include_path, "/include");
}

void get_filename(char *filename, char *pathname)
{
    char *ch = strrchr(pathname, '/');
    if (ch == NULL) {
        strcpy(filename, pathname);
    } else {
        strcpy(filename, ch + 1);
    }
}

int main(int argc, char *argv[])
{
    int c, len;
    char cmd[1024];

    error_return = 0;

    if (argc == 1) {
	fprintf(stderr, "usage: zcc [-d] file.c\n");
	exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "d")) != EOF) {
	switch (c) {
	    case 'd':
		yydebug = 1;
		break;
	    default:
		fprintf(stderr, "Unknown command-line switch: '%c'\n", (char)c);
		exit(EXIT_FAILURE);
		break;
	}
    }

    if (optind == argc) {
	fprintf(stderr, "Must specify input source file.\n");
	exit(EXIT_FAILURE);
    }

    strcpy(pathname, argv[optind]);
    get_filename(filename, pathname);

    yyin = fopen(pathname, "r");
    if (yyin == NULL) {
	perror(pathname);
	exit(EXIT_FAILURE);
    }

    fclose(yyin);

    // Find the absolute path of include directory.
    char include_path[1024];
    get_include_path(argv[0], include_path);
        
    sprintf(cmd, "/usr/bin/cpp -nostdinc -undef -I%s %s", include_path, pathname);
    yyin = popen(cmd, "r");

    init_lex();

    // Make output filename. Same basename, ".s" extension in current directory.
    strcpy(outfilename, filename);
    len = strlen(outfilename);
    if (len > 2 && strcmp(outfilename + len - 2, ".c") == 0) {
	outfilename[len - 2] = '\0';
    }
    strcat(outfilename, ".s");

    scope_start();
    output_start(outfilename);

    check_newfile();
    yyparse();

    output_end();

    pclose(yyin);
    scope_end();

    if (unsupported == 1) {
	printf("One use of an unsupported operator.\n");
    } else if (unsupported > 1) {
	printf("%d uses of unsupported operators.\n", unsupported);
    }

    return error_return;
}
