/*
	HEADER:		CUG276;
	TITLE:		Z-80 Cross-Assembler (Portable);
	FILENAME:	AZ80EVAL.C;
	VERSION:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	AZ80.H;
	AUTHORS:	William C. Colley III;
*/

/*
		      Z-80 Cross-Assembler in Portable C

		Copyright (c) 1986-1988 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUNE 1988	Derived from my S6 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "AZ80 FILE.ASM -L".  WCC3 per Alex Cameron.

	JAN 2010	HRJ edits for lcc windows C compiler

This file contains the assembler's expression evaluator and lexical analyzer.
The lexical analyzer chops the input character stream up into discrete tokens
that are processed by the expression analyzer and the line assembler.  The
expression analyzer processes the token stream into unsigned results of
arithmetic expressions.
*/

/*  Get global goodies:  */

#include "az80.h"
#include <ctype.h> /* hrj */
#include <string.h>

/* local  prototypes HRJ*/
static unsigned eval(unsigned);
static void exp_error(char);
void unlex(void);
TOKEN *lex(void);
static void make_number(unsigned);
int popc(void);
void pushc(char);
int isalph(char);
static int isnum(char), ischar(char), ishex(char);
static int isalpnum(char c);

/* external prototypes HRJ*/
void error(char);
void pops(char *), trash(void);
OPCODE *find_operator(char *);
SYMBOL *find_symbol(char *);

void asm_line(void);
void lclose(void), lopen(char *), lputs(void);
void hclose(void), hopen(char *), hputc(unsigned);
void error(char), fatal_error(char *), warning(char *);
void hseek(unsigned);
void unlex(void);

/*  Get access to global mailboxes defined in AZ80.C:			*/

extern char line[];
extern int filesp, forwd, pass;
extern unsigned pc;
extern FILE *filestk[], *source;
extern TOKEN arg, token;

/*  Machine opcode argument field parsing routine.  The token stream	*/
/*  from the lexical analyzer is processed to extract addresses and	*/
/*  addressing mode information.  The resulting value is passed back	*/
/*  through the same type of token buffer that the lexical analyzer	*/
/*  uses.  In addition, the addressing mode information is returned as	*/
/*  the	function return value.						*/

static int bad;

unsigned get_arg(void)
{
    SCRATCH int c;
    SCRATCH unsigned a, u;

    a = NUM;  u = 0;  bad = FALSE;
    switch (lex() -> attr & TYPE) {
	case SEP:   exp_error('S');  a = NONE;  break;

	case EOL:   unlex();  a = NONE;  break;

	case REG:   a = token.valu;  break;

	case OPR:   if (token.valu == '(') {
			if ((lex() -> attr & TYPE) == REG) {
			    a = token.valu;  lex();
			    switch (a) {
				case C:	    a = C_IND;  break;

				case BC:
				case DE:
				case SP:    a += SP_IND - SP;  break;

				case HL:    a = HL_IND;  break;

				case IX:    a = IX_IND;  goto do_ix_ind;

				case IY:    a = IY_IND;
do_ix_ind:				    if ((token.attr & TYPE) == OPR &&
						(token.valu == '+' ||
						token.valu == '-')) {
						unlex();
						if ((u = eval(LPREN)) > 0x7f
						    && u < 0xff80)
						    exp_error('V');
					    }
					    break;

				default:    exp_error('R');  break;
			    }
			    if ((token.attr & TYPE) != OPR ||
				token.valu != ')') exp_error(')');
			    break;
			}
			else {
			    unlex();  u = eval(LPREN);
			    trash();  pushc(c = popc());
			    if (c == ',' || c == '\n') {
				a = NUM_IND;  break;
			    }
			    token.attr = VAL;  token.valu = u;
			}
		    }

	case VAL:
	case STR:   unlex();  u = eval(START);  unlex();  break;
    }
    arg.valu = bad ? 0 : u;  return arg.attr = a;
}

/*  Expression analysis routine.  The token stream from the lexical	*/
/*  analyzer is processed as an arithmetic expression and reduced to an	*/
/*  unsigned value.  If an error occurs during the evaluation, the	*/
/*  global flag	forwd is set to indicate to the line assembler that it	*/
/*  should not base certain decisions on the result of the evaluation.	*/

unsigned expr()
{
    SCRATCH unsigned u;

    bad = FALSE;
    u = eval(START);
    return bad ? 0 : u;
}

static unsigned eval(pre)
unsigned pre;
{
   register unsigned op, u, v;

   for (;;) {
      u = op = lex() -> valu;
      switch (token.attr & TYPE) {
	 case REG:   exp_error('S');  break;

	 case SEP:   if (pre != START) unlex();
	 case EOL:   exp_error('E');  return(u); /* hjr return had no value */

	 case OPR:   if (!(token.attr & UNARY)) { exp_error('E');  break; }
		     u = eval((op == '+' || op == '-') ?
			   (unsigned) UOP1 : token.attr & PREC);
		     switch (op) {
			case '-':   u = word(0-u);  break; /*hrj had word(-u) */

			case NOT:   u ^= 0xffff;  break;

			case HIGH:  u = high(u);  break;

			case LOW:   u = low(u);  break;
		     }

	 case VAL:
	 case STR:   for (;;) {
			op = lex() -> valu;
			switch (token.attr & TYPE) {
			   case REG:   exp_error('S');  break;

			   case SEP:   if (pre != START) unlex();
			   case EOL:   if (pre == LPREN) exp_error('(');
				       return u;

			   case STR:
			   case VAL:   exp_error('E');  break;

			   case OPR:   if (!(token.attr & BINARY)) {
					  exp_error('E');  break;
				       }
				       if ((token.attr & PREC) >= pre) {
					  unlex();  return u;
				       }
				       if (op != ')')
					  v = eval(token.attr & PREC);
				       switch (op) {
					  case '+':   u += v;  break;

					  case '-':   u -= v;  break;

					  case '*':   u *= v;  break;

					  case '/':   u /= v;  break;

					  case MOD:   u %= v;  break;

					  case AND:   u &= v;  break;

					  case OR:    u |= v;  break;

					  case XOR:   u ^= v;  break;

					  case '<':   u = u < v;  break;

					  case LE:    u = u <= v;  break;

					  case '=':   u = u == v;  break;

					  case GE:    u = u >= v;  break;

					  case '>':   u = u > v;  break;

					  case NE:    u = u != v;  break;

					  case SHL:   if (v > 15)
							 exp_error('E');
						      else u <<= v;
						      break;

					  case SHR:   if (v > 15)
							 exp_error('E');
						      else u >>= v;
						      break;

					  case ')':   if (pre == LPREN)
							 return u;
						      exp_error('(');
						      break;
				       }
				       clamp(u);
				       break;
			}
		     }
		     break;
      }
   }
}

static void exp_error(char c)
{
    forwd = bad = TRUE;  error(c);
}

/*  Lexical analyzer.  The source input character stream is chopped up	*/
/*  into its component parts and the pieces are evaluated.  Symbols are	*/
/*  looked up, operators are looked up, etc.  Everything gets reduced	*/
/*  to an attribute word, a numeric value, and (possibly) a string	*/
/*  value.								*/

static int oldt = FALSE;
static int quote = FALSE;

TOKEN *lex(void)
{
    SCRATCH char c, *p;
    SCRATCH unsigned b;
    SCRATCH OPCODE *o;
    SCRATCH SYMBOL *s;


    if (oldt) { oldt = FALSE;  return &token; }
    trash();
    if (isalph(c = popc())) { /* first char is alpha not digit HRJ*/
	    pushc(c);  pops(token.sval);
	    /* printf("isalph %c, %s -",c,token.sval);  HRJ debug*/
	    if (o = find_operator(token.sval)) {
	        token.attr = o -> attr;  token.valu = o -> valu;
	    }
		else {
	    	token.attr = VAL;
	    	if (!strcmp(token.sval,"$")) token.valu = pc;
			else if (token.sval[0] == '$') { /* HRJ $nnnn */
				/* printf("H$ "); hrj */
				token.sval[0] = '0';  b = 16; make_number(b);
			}
			else if (token.sval[0] == '%') { /* HRJ %nnnn */
				/* printf("H$ %s\n",token.sval);  hrj */
				token.sval[0] = '0';  b = 2; make_number(b);
			}
	    	else if (s = find_symbol(token.sval)) {
				token.valu = s -> valu;
				if (pass == 2 && s -> attr & FORWD) forwd = TRUE;
	    	}
	    	else { token.valu = 0;  exp_error('U'); }
		}
    }
    else if (isnum(c)) { /*value begins with digit 0-9 HRJ*/
		pushc(c);  pops(token.sval);
	 	if ((token.sval[0] == '0') && (token.sval[1] == 'x')) { /* HRJ 0xnnnn as hex value */
			/* printf("0x %s\n",token.sval);  /* hrj */
			token.sval[1] = '0';  b = 16; make_number(b);
		}
		else { /* ... or check for last char for radix HRJ */
			for (p = token.sval; p[1]; ++p);  /* look for null */
			switch (toupper(*p)) {
	    		case 'B':	b = 2;  break;

	    		case 'O':
	    		case 'Q':	b = 8;  break;

	    		default:	++p;
	    		case 'D':	b = 10;  break;

	    		case 'H':	b = 16;  break;
			}
			*p = '\0';  make_number(b);
		}
    }
    else switch (c) {
	case '(':   token.attr = UNARY + LPREN + OPR;
		    goto opr1;

	case ')':   token.attr = BINARY + RPREN + OPR;
		    goto opr1;

	case '+':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '-':   token.attr = BINARY + UNARY + ADDIT + OPR;
		    goto opr1;

	case '*':   token.attr = BINARY + UNARY + MULT + OPR;
		    goto opr1;

	case '/':   token.attr = BINARY + MULT + OPR;
opr1:		    token.valu = c;
		    break;

	case '<':   token.valu = c;
		    if ((c = popc()) == '=') token.valu = LE;
		    else if (c == '>') token.valu = NE;
		    else pushc(c);
		    goto opr2;

	case '=':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = LE;
		    else if (c == '>') token.valu = GE;
		    else pushc(c);
		    goto opr2;

	case '>':   token.valu = c;
		    if ((c = popc()) == '<') token.valu = NE;
		    else if (c == '=') token.valu = GE;
		    else pushc(c);
opr2:		    token.attr = BINARY + RELAT + OPR;
		    break;

	case '\'':
	case '"':   quote = TRUE;  token.attr = STR;
		    for (p = token.sval; (*p = popc()) != c; ++p)
			if (*p == '\n') { exp_error('"');  break; }
		    *p = '\0';  quote = FALSE;
		    if ((token.valu = token.sval[0]) && token.sval[1])
			token.valu = (token.valu << 8) + token.sval[1];
		    break;

	case ',':   token.attr = SEP;
		    break;

        case '\n':  token.attr = EOL;
		    break;
    }
	/* printf("lex s=%s, v=%u\n",token.sval, token.valu); hrj */
    return &token;
}

static void make_number(base)
unsigned base;
{
    SCRATCH char *p;
    SCRATCH unsigned d;

    token.attr = VAL;
    token.valu = 0;
    for (p = token.sval; *p; ++p) {
	d = toupper(*p) - (isnum(*p) ? '0' : 'A' - 10);
	token.valu = token.valu * base + d;
	if (!ishex(*p) || d >= base) { exp_error('D');  break; }
    }
    clamp(token.valu);
    return;
}

int isalph(char c)
{
    return (c >= '?' && c <= '~') || (c >= '#' && c <= '&') ||
	c == '!' || c == '.' || c == ':';
}

static int isnum(char c)
{
    return c >= '0' && c <= '9';
}

static int ishex(char c)
{
    return isnum(c) || ((c = toupper(c)) >= 'A' && c <= 'F');
}

static int isalpnum(char c) /* was isalnum(), already in ctype.h HRJ */
{
    return isalph(c) || isnum(c);
}

/*  Push back the current token into the input stream.  One level of	*/
/*  pushback is supported.						*/

void unlex(void)
{
    oldt = TRUE;
    return;
}

/*  Get an alphanumeric string into the string value part of the	*/
/*  current token.  Leading blank space is trashed.			*/

void pops(char *s)
{
    trash();
    for (; isalpnum(*s = popc()); ++s);
    pushc(*s);  *s = '\0';
    return;
}

/*  Trash blank space and push back the character following it.		*/

void trash(void)
{
    SCRATCH char c;

    while ((c = popc()) == ' ');
    pushc(c);
    return;
}

/*  Get character from input stream.  This routine does a number of	*/
/*  other things while it's passing back characters.  All control	*/
/*  characters except \t and \n are ignored.  \t is mapped into ' '.	*/
/*  Semicolon is mapped to \n.  In addition, a copy of all input is set	*/
/*  up in a line buffer for the benefit of the listing.			*/

static int oldc, eol;
static char *lptr;

int popc(void)
{
    SCRATCH int c;

    if (oldc) { c = oldc;  oldc = '\0';  return c; }
    if (eol) return '\n';
    for (;;) {
	if ((c = getc(source)) != EOF && (c &= 0377) == ';' && !quote) {
	    do *lptr++ = c;
	    while ((c = getc(source)) != EOF && (c &= 0377) != '\n');
	}
	if (c == EOF) c = '\n';
	if ((*lptr++ = c) >= ' ' && c <= '~') return c;
	if (c == '\n') { eol = TRUE;  *lptr = '\0';  return '\n'; }
	if (c == '\t') return quote ? '\t' : ' ';
    }
}

/*  Push character back onto input stream.  Only one level of push-back	*/
/*  supported.  \0 cannot be pushed back, but nobody would want to.	*/

void pushc(char c)
{
    oldc = c;
    return;
}

/*  Begin new line of source input.  This routine returns non-zero if	*/
/*  EOF	has been reached on the main source file, zero otherwise.	*/

int newline(void)
{

    oldc = '\0';  lptr = line;
    oldt = eol = FALSE;
    while (feof(source)) {
	if (ferror(source)) fatal_error(ASMREAD);
	if (filesp) {
	    fclose(source);
	    source = filestk[--filesp];
	}
	else return TRUE;
    }
    return FALSE;
}
