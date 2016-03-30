/*
	HEADER:		CUG276;
	TITLE:		Z-80 Cross-Assembler (Portable);
	FILENAME:	AZ80.C;
	VERSION:	0.1;
	DATE:		08/27/1988;
	SEE-ALSO:	AZ80.H;
	AUTHORS:	William C. Colley III;

	UPDATE: Jan 2010 Herbert R. Johnson, compiles under lcc for windows.
*/

/*
		      Z-80 Cross-Assembler in Portable C

		Copyright (c) 1986-1988 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUNE 1988	Derived from my S-6 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "AZ80 FILE.ASM -L".  WCC3 per Alex Cameron.

	JAN 2010	Compiles under lcc for windows HRJ
				had to move all function prototypes to head of files
				had to use VOID for empty arguments of functions
				other small corrections all tagged with "hrj" comments

This file contains the main program and line assembly routines for the
assembler.  The main program parses the command line, feeds the source lines
to the line assembly routine, and sends the results to the listing and object
file output routines.  It also coordinates the activities of everything.  The
line assembly routines uses the expression analyzer and the lexical analyzer
to parse the source line and convert it into the object bytes that it
represents.
*/

/*  Get global goodies:  */

#include "az80.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* external routines HRJ */


    void lclose(void), lopen(char *), lputs(void);
    void hclose(void), hopen(char *), hputc(unsigned);
    void error(char), fatal_error(char *), warning(char *);
    void pops(char *), pushc(int), trash(void);
    void hseek(unsigned);
    void unlex(void);
    int isalph(char); /* was int isalph(int) HRJ */
    int popc(void);
    OPCODE *find_code(char *), *find_operator(char *);
	unsigned get_arg(void);
	TOKEN *lex(void);
	int newline(void);

/* these are local but used before defined HRJ */

    void asm_line(void);
    static void do_label(void),normal_op(void), pseudo_op(void);
    static void flush(void);
    static void grab_comma(void);
    static void insert_prebyte(unsigned **, unsigned);


/*  Define global mailboxes for all modules:				*/

char errcode, line[MAXLINE + 1], title[MAXLINE];
int pass = 0;
int eject, filesp, forwd, listhex;
unsigned address, bytes, errors, listleft, obj[MAXLINE], pagelen, pc;
FILE *filestk[FILES], *source;
TOKEN arg, token;

/*  Mainline routine.  This routine parses the command line, sets up	*/
/*  the assembler at the beginning of each pass, feeds the source text	*/
/*  to the line assembler, feeds the result to the listing and hex file	*/
/*  drivers, and cleans everything up at the end of the run.		*/

static int done, ifsp, off;

int main(argc,argv)
int argc;
char **argv;
{
    SCRATCH unsigned *o;

    printf("Z-80 Cross-Assembler (Portable) Ver 0.1\n");
    printf("Copyright (c) 1986-1988 William C. Colley, III\n\n");

    while (--argc > 0) {
	if (**++argv == '-') {
	    switch (toupper(*++*argv)) {
		case 'L':   if (!*++*argv) {
				if (!--argc) { warning(NOLST);  break; }
				else ++argv;
			    }
			    lopen(*argv);
			    break;

		case 'O':   if (!*++*argv) {
				if (!--argc) { warning(NOHEX);  break; }
				else ++argv;
			    }
			    hopen(*argv);
			    break;

		default:    warning(BADOPT);
	    }
	}
	else if (filestk[0]) warning(TWOASM);
	else if (!(filestk[0] = fopen(*argv,"r"))) fatal_error(ASMOPEN);
    }
    if (!filestk[0]) fatal_error(NOASM);

    while (++pass < 3) {
	fseek(source = filestk[0],0L,0);  done = off = FALSE;
	errors = filesp = ifsp = pagelen = pc = 0;  title[0] = '\0';
	while (!done) {
	    errcode = ' ';
	    if (newline()) {
		error('*');
		strcpy(line,"\tEND\n");
		done = eject = TRUE;  listhex = FALSE;
		bytes = 0;
	    }
	    else asm_line();
	    pc = word(pc + bytes);
	    if (pass == 2) {
		lputs();
		for (o = obj; bytes--; hputc(*o++));
	    }
	}
    }

    fclose(filestk[0]);  lclose();  hclose();

    if (errors) printf("%d Error(s)\n",errors);
    else printf("No Errors\n");

    return errors;
}

/*  Line assembly routine.  This routine gets the contents of the	*/
/*  argument field from the source file using the expression evaluator	*/
/*  and lexical analyzer.  It makes all validity checks on the		*/
/*  arguments validity, fills a buffer with the machine code bytes and	*/
/*  returns nothing.							*/

static char label[MAXLINE];
static int ifstack[IFDEPTH] = { ON };

static OPCODE *opcod;

void asm_line()
{
    SCRATCH char *p;
    SCRATCH int i;

    address = pc;  bytes = 0;  eject = forwd = listhex = FALSE;
    for (i = 0; i < BIGINST; obj[i++] = NOP);

    label[0] = '\0';
    if ((i = popc()) != ' ' && i != '\n') {
	if (isalph((char) i)) { /*hrj added char cast */
	    pushc(i);  pops(label);
	    for (p = label;  *(p + 1);  ++p);
	    if (*p == ':') *p = '\0';
	    if (find_operator(label)) { label[0] = '\0';  error('L'); }
	}
	else {
	    error('L');
	    while ((i = popc()) != ' ' && i != '\n');
	}
    }

    trash();  opcod = NULL;
    if ((i = popc()) != '\n') {
	if (!isalph(i)) error('S');
	else {
	    pushc(i);  pops(token.sval);
	    if (!(opcod = find_code(token.sval))) error('O');
	}
	if (!opcod) { listhex = TRUE;  bytes = BIGINST; }
    }

    if (opcod && opcod -> attr & ISIF) { if (label[0]) error('L'); }
    else if (off) { listhex = FALSE;  flush();  return; }

    if (!opcod) { do_label();  flush(); }
    else {
	listhex = TRUE;
	if (opcod -> attr & PSEUDO) pseudo_op();
	else normal_op();
	while ((i = popc()) != '\n') if (i != ' ') error('T');
    }
    source = filestk[filesp];
    return;
}

static void flush()
{
    while (popc() != '\n');
}

static void do_label()
{
    SCRATCH SYMBOL *l;
    SYMBOL *find_symbol(char *), *new_symbol(char *);

    if (label[0]) {
	listhex = TRUE;
	if (pass == 1) {
	    if (!((l = new_symbol(label)) -> attr)) {
		l -> attr = FORWD + VAL;
		l -> valu = pc;
	    }
	}
	else {
	    if ((l = find_symbol(label)) != 0) {
		l -> attr = VAL;
		if (l -> valu != pc) error('M');
	    }
	    else error('P');
	}
    }
}

static void normal_op()
{
    SCRATCH unsigned opcode, tmp;
    unsigned *o, *p;
    static unsigned IM_tbl[] = { 0x00, 0x10, 0x18 };

    unsigned expr(void);
    void do_label(void);
    TOKEN *lex(void);

    do_label();  opcode = opcod -> valu;  o = obj;
    if (opcode > 0xff) *o++ = high(opcode);
    *o = low(opcode);
    switch (opcod -> attr & OPTYPE) {
	case LD:
	    p = o;
	    switch (get_arg()) {
		case IX:
		case IY:	insert_prebyte(&o,arg.attr);

		case BC:
		case DE:
		case HL:
		case SP:	*o = 0x01 + ((arg.attr & 007) << 3);
				grab_comma();
				switch (get_arg()) {
				    case NUM_IND:   if (*o == 0x21) *o += 0x09;
						    else {
							*o += 0x4a;
							insert_prebyte(&o,
							    arg.attr);
						    }

				    case NUM:	    *++o = low(arg.valu);
						    *++o = high(arg.valu);
						    break;

				    case IX:
				    case IY:	    insert_prebyte(&o,
							arg.attr);

				    case HL:	    if (*o == 0x31) {
							*o = 0xf9;  break;
						    }

				    default:	    error(arg.attr < NUM ?
							'R' : 'S');
						    break;
				}
				break;

		case I:		*o++ = 0xed;  *o = 0x47;  goto do_ld_i;

		case R:		*o++ = 0xed;  *o = 0x4f;  goto do_ld_i;

		case BC_IND:	*o = 0x02;  goto do_ld_i;

		case DE_IND:	*o = 0x12;
do_ld_i:			grab_comma();
				if (get_arg() != A)
				    error(arg.attr < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	*++o = low(arg.valu);
				*++o = high(arg.valu);
				grab_comma();  *p = 0x02;
				switch (get_arg()) {
				    case A:	    *p = 0x32;  break;

				    case BC:
				    case DE:
				    case SP:	    *p = 0x43;

				    case IX:
				    case IY:	    *p += (arg.attr & 007)
							<< 3;
						    insert_prebyte(&o,
							arg.attr);
						    break;

				    case HL:	    *p = 0x22;  break;

				    default:	    error(arg.attr < NUM ?
							'R' : 'S');
						    break;
				}
				break;

		case A:		grab_comma();
				switch (get_arg()) {
				    case BC_IND:    *o = 0x0a;  break;

				    case DE_IND:    *o = 0x1a;  break;

				    case I:	    *o++ = 0xed;  *o = 0x57;
						    break;

				    case R:	    *o++ = 0xed;  *o = 0x5f;
						    break;

				    case NUM_IND:   *o++ = 0x3a;
						    *o++ = low(arg.valu);
						    *o = high(arg.valu);
						    break;

				    default:	    *o += 0x38;  goto do_ld_a;
				}
				break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  ++p;
				*++o = low(arg.valu);  arg.attr = HL_IND;

		default:	if (arg.attr <= A) {
				    *p += (arg.attr & 007) << 3;
				    grab_comma();  get_arg();
do_ld_a:			    if (arg.attr == NUM) {
					*p -= 0x3a;
					if (arg.valu > 0xff &&
					    arg.valu < 0xff80) {
					    *++o = 0;  error('V');
					}
					else *++o = low(arg.valu);
					break;
				    }
				    if (arg.attr <= A || arg.attr ==
					IX_IND || arg.attr == IY_IND) {
					if ((*p += arg.attr & 007) == 0x76) {
					    *p = 0x06;  error('R');  break;
					}
					if (arg.attr > A) {
					    insert_prebyte(&o,arg.attr);
					    *++o = low(arg.valu);
					}
					break;
				    }
				}
				error(arg.attr < NUM ? 'R' : 'S');  break;
	    }
	    break;

	case POP:
	    switch (get_arg()) {
		case IX:
		case IY:	insert_prebyte(&o,arg.attr);
				arg.attr = HL;

		default:	if (arg.attr < BC || arg.attr > AF)
				    error(arg.attr < NUM ? 'R' : 'S');
				else *o += (arg.attr & 007) << 3;
				break;
	    }
	    break;

	case EX:
	    tmp = get_arg();  grab_comma();
	    switch (tmp) {
		case SP_IND:	if (get_arg() == IX || arg.attr == IY)
				    insert_prebyte(&o,arg.attr);
				else if (arg.attr != HL)
				    error(arg.attr < NUM ? 'R' : 'S');
				break;

		case DE:	if (get_arg() == HL) *o = 0xeb;
				else error(arg.attr < NUM ? 'R' : 'S');
				break;

		case AF:	if (get_arg() == AF) {
				    *o = 0x08;  trash();
				    if ((tmp = popc()) == '\'') break;
				    pushc(tmp);  error('S');  break;
				}
				error(arg.attr < NUM ? 'R' : 'S');
				break;
	    }
	    break;

	case ADD:
	    if (get_arg() == IX || arg.attr == IY) {
		insert_prebyte(&o,arg.attr);
		tmp = arg.attr;  arg.attr = HL;
	    }
	    else tmp = HL;
	    goto do_adc;

	case ADC:
	    get_arg();  tmp = HL;
do_adc:	    if (arg.attr == HL) {
		switch (*o) {
		    case 0x80:	*o = 0x09;  break;

		    case 0x88:	*o++ = 0xed;  *o = 0x4a;  break;

		    case 0x98:	*o++ = 0xed;  *o = 0x42;  break;
		}
		grab_comma();
		if (get_arg() == BC || arg.attr == DE || arg.attr == SP ||
		    arg.attr == tmp) *o += (arg.attr & 007) << 3;
		else error(arg.attr < NUM ? 'R' : 'S');
		break;
	    }
	    if (arg.attr != A) { error('S');  break; }
	    grab_comma();

	case CP:
	    switch (get_arg()) {
		case NUM_IND:
		case NONE:	error('S');  break;

		case NUM:	*o++ += 0x46;
				if (arg.valu > 0xff && arg.valu < 0xff80) {
				    *o = 0;  error('V');
				}
				else *o = low(arg.valu);
				break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  *o++ += 0x06;
				*o = low(arg.valu);  break;

		default:	if (arg.attr > A) error('R');
				else *o += arg.attr & 007;
				break;
	    }
	    break;

	case DEC:
	    p = o;
	    switch (get_arg()) {
		case NUM:
		case NUM_IND:
		case NONE:	error('S');  break;

		case IX:
		case IY:	insert_prebyte(&o,arg.attr);

		case SP:
		case HL:
		case DE:
		case BC:	*o = (arg.attr & 007 ^ *o) << 3 ^ 0x23;
				break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);  ++p;
				*++o = arg.valu;  arg.attr = HL_IND;

		default:	if (arg.attr > A) error('R');
				else *p += (arg.attr & 007) << 3;
				break;
	    }
	    break;

	case BIT:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu > 7) error('V');
	    else *o += arg.valu << 3;
	    grab_comma();

	case RLC:
	    switch (get_arg()) {
		case NUM:
		case NUM_IND:
		case NONE:	error('S');  break;

		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr); *(o + 1) = *o;
				*o++ = low(arg.valu);  arg.attr = HL_IND;

		default:	if (arg.attr > A) error('R');
				else *o += arg.attr & 007;
				break;
	    }
	    break;


	case JR:
	    if (get_arg() == NUM) { *o -= 0x08;  goto do_djnz; }
	    if (arg.attr == C) arg.attr = CY;
	    if (arg.attr < NZ || arg.attr > CY) {
		*++o = 0xfe;  error('S');  break;
	    }
	    *o += (arg.attr & 007) << 3;  grab_comma();

	case DJNZ:
	    if (get_arg() != NUM) { *++o = 0xfe;  error('S');  break; }
do_djnz:    if ((tmp = arg.valu - (pc + 2)) > 0x7f && tmp < 0xff80) {
		*++o = 0xfe;  error('B');
	    }
	    else *++o = low(tmp);
	    break;

	case JP:
	    switch (get_arg()) {
		case IX_IND:
		case IY_IND:	insert_prebyte(&o,arg.attr);
				if (arg.valu) error('V');

		case HL_IND:	*o = 0xe9;  break;

		default:	goto do_call;
	    }
	    break;

	case CALL:
	    get_arg();
do_call:    if (arg.attr == NUM) *o += *o == 0xc4 ? 0x09 : 0x01;
	    else {
		if (arg.attr == C) arg.attr = CY;
		if (arg.attr < NZ || arg.attr > M) error('S');
		else *o += (arg.attr & 007) << 3;
		grab_comma();
		if (get_arg() != NUM) error('S');
	    }
	    *++o = low(arg.valu);  *++o = high(arg.valu);  break;

	case RET:
	    if (get_arg() == NONE) { *o += 0x09;  break; }
	    if (arg.attr == C) arg.attr = CY;
	    if (arg.attr < NZ || arg.attr > M) error('S');
	    else *o += (arg.attr & 007) << 3;
	    break;

	case IN:
	    tmp = get_arg();  grab_comma();
	    switch (get_arg()) {
		case C_IND:	*(o - 1) = 0xed;  *o = 0x40;
				if (tmp <= L || tmp == A)
				    *o += (tmp & 007) << 3;
				else error(tmp < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	if (arg.valu <= 0xff) {
				    *o = arg.valu;
				    if (tmp != A) error(tmp < NUM ? 'R' : 'S');
				}
				else error('V');
				break;

		case NUM:	error('S');  break;

		default:	error('R');  break;
	    }
	    break;

	case OUT:
	    switch (get_arg()) {
		case C_IND:	*(o - 1) = 0xed;  *o = 0x41;  grab_comma();
				if (get_arg() <= L || arg.attr == A)
				    *o += (arg.attr & 007) << 3;
				else error(arg.attr < NUM ? 'R' : 'S');
				break;

		case NUM_IND:	if (arg.valu <= 0xff) {
				    *o = arg.valu;  grab_comma();
				    if (get_arg() != A)
					error(arg.attr < NUM ? 'R' : 'S');
				}
				else error('V');
				break;

		case NUM:	error('S');  break;

		default:	error('R');  break;
	    }
	    break;

	case RST:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu & 0xffc7) error('V');
	    else *o |= arg.valu;
	    break;

	case IM:
	    if (get_arg() != NUM) error('S');
	    else if (arg.valu > 2) error('V');
	    else *o |= IM_tbl[arg.valu];

	case NO_ARGS:
	    break;
    }
    if ((lex() -> attr & TYPE) != EOL) error('T');
    bytes = (o - obj) + 1;  return;
}

static void grab_comma()
{

    if ((lex() -> attr & TYPE) != SEP) { error('S');  unlex(); }
}

static void insert_prebyte(optr,pb)
unsigned **optr;
unsigned pb;
{
    SCRATCH unsigned *q;

    for (q = ++*optr; q > obj ; --q) *q = *(q - 1);
    if (pb == IX || pb == IX_IND) *q = 0xdd;
    else if (pb == IY || pb == IY_IND) *q = 0xfd;
    else *q = 0xed;
    return;
}

static void pseudo_op()
{
    SCRATCH char *s;
    SCRATCH unsigned *o, u;
    SCRATCH SYMBOL *l;

    unsigned expr(void);
    SYMBOL *find_symbol(char *), *new_symbol(char *);

    o = obj;
    switch (opcod -> valu) {
	case DB:
	case DC:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) {
			    *o++ = 0;  ++bytes;
			}
			else if (token.attr == STR) {
			    trash();  pushc(u = popc());
			    if (u != ',' && u != '\n') goto do_byte;
			    for (s = token.sval; *s; *o++ = *s++) ++bytes;
			    lex();
			}
			else {
do_byte:		    unlex();
			    if ((u = expr()) > 0xff && u < 0xff80) {
				u = 0;  error('V');
			    }
			    *o++ = low(u);  ++bytes;
			}
		    } while ((token.attr & TYPE) == SEP);
		    if (bytes && opcod -> valu == DC) *(o - 1) |= 0x80;
		    break;

	case DS:    do_label();
		    u = word(pc + expr());
		    if (forwd) error('P');
		    else {
			pc = u;
			if (pass == 2) hseek(pc);
		    }
		    break;

	case DW:    do_label();
		    do {
			if ((lex() -> attr & TYPE) == SEP) u = 0;
			else { unlex();  u = expr(); }
			*o++ = low(u);  *o++ = high(u);
			bytes += 2;
		    } while ((token.attr & TYPE) == SEP);
		    break;

	case ELSE:  listhex = FALSE;
		    if (ifsp) off = (ifstack[ifsp] = -ifstack[ifsp]) != ON;
		    else error('I');
		    break;

	case END:   do_label();
		    if (filesp) { listhex = FALSE;  error('*'); }
		    else {
			done = eject = TRUE;
			if (pass == 2 && (lex() -> attr & TYPE) != EOL) {
			    unlex();  hseek(address = expr());
			}
			if (ifsp) error('I');
		    }
		    break;

	case ENDIF: listhex = FALSE;
		    if (ifsp) off = ifstack[--ifsp] != ON;
		    else error('I');
		    break;

	case EQU:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)) {
				l -> attr = FORWD + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if ((l = find_symbol(label)) != 0) {
				l -> attr = VAL;
				address = expr();
				if (forwd) error('P');
				if (l -> valu != address) error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;

	case IF:    if (++ifsp == IFDEPTH) fatal_error(IFOFLOW);
		    address = expr();
		    if (forwd) { error('P');  address = TRUE; }
		    if (off) { listhex = FALSE;  ifstack[ifsp] = 0; } /* was ZERO but not defined hrj*/
		    else {
			ifstack[ifsp] = address ? ON : OFF;
			if (!address) off = TRUE;
		    }
		    break;

	case INCL:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == STR) {
			if (++filesp == FILES) fatal_error(FLOFLOW);
			if (!(filestk[filesp] = fopen(token.sval,"r"))) {
			    --filesp;  error('V');
			}
		    }
		    else error('S');
		    break;

	case ORG:   u = expr();
		    if (forwd) error('P');
		    else {
			pc = address = u;
			if (pass == 2) hseek(pc);
		    }
		    do_label();
		    break;

	case PAGE:  listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) != EOL) {
			unlex();  pagelen = expr();
			if (pagelen > 0 && pagelen < 3) {
			    pagelen = 0;  error('V');
			}
		    }
		    eject = TRUE;
		    break;

	case TITLE: listhex = FALSE;  do_label();
		    if ((lex() -> attr & TYPE) == EOL) title[0] = '\0';
		    else if ((token.attr & TYPE) != STR) error('S');
		    else strcpy(title,token.sval);
		    break;

	case VAR:   if (label[0]) {
			if (pass == 1) {
			    if (!((l = new_symbol(label)) -> attr)
				|| (l -> attr & SOFT)) {
				l -> attr = FORWD + SOFT + VAL;
				address = expr();
				if (!forwd) l -> valu = address;
			    }
			}
			else {
			    if ((l = find_symbol(label)) != 0) {
				address = expr();
				if (forwd) error('P');
				else if (l -> attr & SOFT) {
				    l -> attr = SOFT + VAL;
				    l -> valu = address;
				}
				else error('M');
			    }
			    else error('P');
			}
		    }
		    else error('L');
		    break;
    }
    return;
}
