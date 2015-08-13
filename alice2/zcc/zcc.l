
%{

#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "zcc.h"
#include "y.tab.h"

YYSTYPE yylval;
static char *string_buf = NULL;
static int string_buf_len = 0;

// Forward declaration.
static int input(void);
static void yyunput(int c, char *buf_ptr);

void init_lex(void)
{
    yylineno = 0;

    string_buf_len = 16;
    string_buf = malloc(string_buf_len + 2);
}

static char *extend_string_buf(char *p)
{
    int offset;

    offset = p - string_buf;

    string_buf_len *= 2;
    string_buf = realloc(string_buf, string_buf_len + 2);

    return string_buf + offset;
}

static int get_esc_char(int c)
{
    int val;

    switch (c) {
	case 'a':
	    val = '\a';
	    break;
	case 'b':
	    val = '\b';
	    break;
	case 'f':
	    val = '\f';
	    break;
	case 'n':
	    val = '\n';
	    break;
	case 'r':
	    val = '\r';
	    break;
	case 't':
	    val = '\t';
	    break;
	case 'v':
	    val = '\v';
	    break;
	case '\\':
	    val = '\\';
	    break;
	case '?':
	    val = '?';
	    break;
	case '\'':
	    val = '\'';
	    break;
	case '"':
	    val = '"';
	    break;
	case '\n':
	    yylineno++;
	    val = -1;
	    break;
	case 'x':
	case 'X':
	    val = 0;
	    c = input();
	    if (isxdigit(c)) {
		val = c - '0';
		if (val > 9) {
		    val -= 7;
		}
		if (val > 15) {
		    val -= 32;
		}
		c = input();
		if (isxdigit(c)) {
		    val = val * 16 + c - '0';
		    if (val > 9) {
			val -= 7;
		    }
		    if (val > 15) {
			val -= 32;
		    }
		    c = input();
		}
	    }
	    unput(c);
	    break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	    val = c - '0';
	    c = input();
	    if (c >= '0' && c < '8') {
		val = val * 8 + c - '0';
		c = input();
		if (c >= '0' && c < '8') {
		    val = val * 8 + c - '0';
		    c = input();
		}
	    }
	    unput(c);
	    break;
	default:
	    yywarning("unknown escape sequence `\\%c'", c);
	    val = c;
	    break;
    }

    return val;
}

/*
 * check_newfile() is identical to check_newline() except it's called
 * at the beginning of parsing and Lex hasn't initialized yet, so we
 * can't call input() or unput().  Instead we just call getc() and
 * ungetc().  I wish there were a better way to do this.
 */

void check_newfile(void)
{
    int c;
    char *s;

    yylineno++;
    c = getc(yyin);
    if (c == '#') {
	do {
	    c = getc(yyin);
	} while (isspace(c));
	yylineno = 0;
	while (isdigit(c)) {
	    yylineno = yylineno * 10 + c - '0';
	    c = getc(yyin);
	}
	while (isspace(c)) {
	    c = getc(yyin);
	}
	if (c == '"') {
	    c = getc(yyin);
	}
	s = filename;
	while (c != '"' && c != '\n') {
	    *s++ = c;
	    c = getc(yyin);
	}
	*s = '\0';
	while (c != '\n') {
	    c = getc(yyin);
	}
    } else {
	ungetc(c, yyin);
    }
}

void check_newline(void)
{
    int c;
    char *s;

    yylineno++;
    c = input();
    if (c == '#') {
	do {
	    c = input();
	} while (isspace(c));
	yylineno = 0;
	while (isdigit(c)) {
	    yylineno = yylineno * 10 + c - '0';
	    c = input();
	}
	while (isspace(c)) {
	    c = input();
	}
	if (c == '"') {
	    c = input();
	}
	s = filename;
	while (c != '"' && c != '\n') {
	    *s++ = c;
	    c = input();
	}
	*s = '\0';
	while (c != '\n') {
	    c = input();
	}
    } else {
	unput(c);
    }
}

%}

%%

[ \t]		;
\n		check_newline();

int		return INT;
char		return CHAR;
signed		return SIGNED;
unsigned	return UNSIGNED;
void		return VOID;

static		return STATIC;
extern		return EXTERN;
volatile	return VOLATILE;
auto		return AUTO;
register	return REGISTER;

return		return RETURN;
for		return FOR;
while		return WHILE;
do		return DO;
if		return IF;
else		return ELSE;
break		return BREAK;
continue	return CONTINUE;
goto		return GOTO;
switch		return SWITCH;
default		return DEFAULT;
case		return CASE;

"+="		return PLUS_ASSIGN;
"-="		return MINUS_ASSIGN;
"*="		return TIMES_ASSIGN;
"/="		return DIV_ASSIGN;
"%="		return MOD_ASSIGN;
"&="		return AND_ASSIGN;
"^="		return XOR_ASSIGN;
"|="		return OR_ASSIGN;
"<<="		return SHIFT_LEFT_ASSIGN;
">>="		return SHIFT_RIGHT_ASSIGN;

"||"		return LOG_OR;
"&&"		return LOG_AND;

"=="		return EQUALS;
"!="		return NOT_EQUALS;

"<="		return LESS_EQUALS;
">="		return GREATER_EQUALS;

"<<"		return SHIFT_LEFT;
">>"		return SHIFT_RIGHT;

"++"		return INCR;
"--"		return DECR;
"sizeof"	return SIZEOF;

"..."		return DOTDOTDOT;
"."		return DEREF;
"->"		return DEREF_PTR;

[a-zA-Z_][a-zA-Z0-9_]*	{
			    strcpy(yylval.ident, yytext);
			    return IDENT;
			}

[1-9][0-9]*[lL]?	{
			    yylval.number = strtol(yytext, NULL, 10);
			    return NUMBER;
			}

0[xX][0-9a-fA-F][0-9a-fA-F]*[lL]? {
			    yylval.number = strtol(yytext + 2, NULL, 16);
			    return NUMBER;
			}

0[0-7]*[lL]?		{
			    yylval.number = strtol(yytext, NULL, 8);
			    return NUMBER;
			}

'			{
			    int c = input();
			    char *p;

			    p = string_buf;
			    while (c != '\'' && c != 0) {
				if (c == '\\') {
				    c = input();
				    if (c == 0) {
					break;
				    }
				    c = get_esc_char(c);
				    if (c == -1) {
					goto skip_character_char;
				    }
				}
				if (p == string_buf + string_buf_len) {
				    p = extend_string_buf(p);
				}
				*p++ = c;
			    skip_character_char:
				c = input();
			    }
			    *p = '\0';
			    if (c == 0) {
				yyerror("unterminated character constant");
			    } else if (string_buf[0] == '\0') {
				yyerror("empty character constant");
			    } else if (string_buf[1] != '\0') {
				yywarning("multi-character character constant");
			    }
			    yylval.number = string_buf[0];
			    return NUMBER;
			}

["]			{
			    int c = input();
			    char *p;

			    p = string_buf;
			    while (c != '"' && c != 0) {
				if (c == '\\') {
				    c = input();
				    if (c == 0) {
					break;
				    }
				    c = get_esc_char(c);
				    if (c == -1) {
					goto skip_character_str;
				    }
				}
				if (p == string_buf + string_buf_len) {
				    p = extend_string_buf(p);
				}
				*p++ = c;
			    skip_character_str:
				c = input();
			    }
			    *p = '\0';
			    if (c == 0) {
				yyerror("unterminated string constant");
			    }
			    yylval.string = string_buf;
			    return STRING;
			}

.			return yytext[0];
