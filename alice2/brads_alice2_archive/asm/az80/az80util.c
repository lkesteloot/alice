/*
	HEADER:		CUG276;
	TITLE:		Z-80 Cross-Assembler (Portable);
	FILENAME:	AZ80UTIL.C;
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

	JAN 2010	HRJ fixes for lcc windows C compiler

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "az80.h"
#include <string.h> /* HRJ */
#include <ctype.h>
// #include <malloc.h>
#include <string.h> // grantham
#include <stdio.h>
#include <stdlib.h>

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

/* char *calloc(); */

/*HRJ external declarations */

/*HRJ local declarations */

static OPCODE *bccsearch(OPCODE *, OPCODE *, char *); /* HRJ changed from bsearch() due to conflict */
static void list_sym(SYMBOL *);
static void check_page(void);
static void record(unsigned);
static void putb(unsigned);
static int ustrcmp(char *, char*);
void warning(char *);
void fatal_error(char *);


/*  Get access to global mailboxes defined in AZ80.C:			*/

extern char errcode, line[], title[];
extern int eject, listhex;
extern unsigned address, bytes, errors, listleft, obj[], pagelen;

/*  The symbol table is a binary tree of variable-length blocks drawn	*/
/*  from the heap with the calloc() function.  The root pointer lives	*/
/*  here:								*/

static SYMBOL *sroot = NULL;

/*  Add new symbol to symbol table.  Returns pointer to symbol even if	*/
/*  the symbol already exists.  If there's not enough memory to store	*/
/*  the new symbol, a fatal error occurs.				*/

SYMBOL *new_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL **p, *q;

    for (p = &sroot; (q = *p) && (i = strcmp(nam,q -> sname)); )
	p = i < 0 ? &(q -> left) : &(q -> right);
    if (!q) {
	if (!(*p = q = (SYMBOL *)calloc(1,sizeof(SYMBOL) + strlen(nam))))
	    fatal_error(SYMBOLS);
	strcpy(q -> sname,nam);
    }
    return q;
}

/*  Look up symbol in symbol table.  Returns pointer to symbol or NULL	*/
/*  if symbol not found.						*/

SYMBOL *find_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL *p;

    for (p = sroot; p && (i = strcmp(nam,p -> sname));
	p = i < 0 ? p -> left : p -> right);
    return p;
}

/*  Opcode table search routine.  This routine pats down the opcode	*/
/*  table for a given opcode and returns either a pointer to it or	*/
/*  NULL if the opcode doesn't exist.					*/

OPCODE *find_code(nam)
char *nam;
{

    static OPCODE opctbl[] = {
	{ ADC,			0x88,	"ADC"	},
	{ ADD,			0x80,	"ADD"	},
	{ CP,			0xa0,	"AND"	},
	{ BIT, 			0xcb40,	"BIT"	},
	{ CALL,			0xc4,	"CALL"	},
	{ NO_ARGS,		0x3f,	"CCF"	},
	{ PSEUDO + ISIF,	IF,	"COND"	},
	{ CP,			0xb8,	"CP"	},
	{ NO_ARGS,		0xeda9,	"CPD"	},
	{ NO_ARGS,		0xedb9,	"CPDR"	},
	{ NO_ARGS,		0xeda1,	"CPI"	},
	{ NO_ARGS,		0xedb1,	"CPIR"	},
	{ NO_ARGS,		0x2f,	"CPL"	},
	{ NO_ARGS,		0x27,	"DAA"	},
	{ PSEUDO,		DB,	"DB"	},
	{ PSEUDO,		DC,	"DC"	},
	{ DEC,			0x05,	"DEC"	},
	{ PSEUDO,		DB,	"DEFB"	},
	{ PSEUDO,		VAR,	"DEFL"	},
	{ PSEUDO,		DB,	"DEFM"	},
	{ PSEUDO,		DS,	"DEFS"	},
	{ PSEUDO,		DW,	"DEFW"	},
	{ NO_ARGS,		0xf3,	"DI"	},
	{ DJNZ,			0x10,	"DJNZ"	},
	{ PSEUDO,		DS,	"DS"	},
	{ PSEUDO,		DW,	"DW"	},
	{ NO_ARGS,		0xfb,	"EI"	},
	{ PSEUDO + ISIF,	ELSE,	"ELSE"	},
	{ PSEUDO,		END,	"END"	},
	{ PSEUDO + ISIF,	ENDIF,	"ENDC"	},
	{ PSEUDO + ISIF,	ENDIF,	"ENDIF"	},
	{ PSEUDO,		EQU,	"EQU"	},
	{ EX,			0xe3,	"EX"	},
	{ NO_ARGS,		0xd9,	"EXX"	},
	{ NO_ARGS,		0x76,	"HALT"	},
	{ PSEUDO + ISIF,	IF,	"IF"	},
	{ IM,			0xed46,	"IM"	},
	{ IN,			0xdb00,	"IN"	},
	{ DEC,			0x04,	"INC"	},
	{ PSEUDO,		INCL,	"INCL"	},
	{ NO_ARGS,		0xedaa,	"IND"	},
	{ NO_ARGS,		0xedba,	"INDR"	},
	{ NO_ARGS,		0xeda2,	"INI"	},
	{ NO_ARGS,		0xedb2,	"INIR"	},
	{ JP,			0xc2,	"JP"	},
	{ JR,			0x20,	"JR"	},
	{ LD,			0x40,	"LD"	},
	{ NO_ARGS,		0xeda8,	"LDD"	},
	{ NO_ARGS,		0xedb8,	"LDDR"	},
	{ NO_ARGS,		0xeda0,	"LDI"	},
	{ NO_ARGS,		0xedb0,	"LDIR"	},
	{ NO_ARGS,		0xed44,	"NEG"	},
	{ NO_ARGS,		0x00,	"NOP"	},
	{ CP,			0xb0,	"OR"	},
	{ PSEUDO,		ORG,	"ORG"	},
	{ NO_ARGS,		0xedbb,	"OTDR"	},
	{ NO_ARGS,		0xedb3,	"OTIR"	},
	{ OUT,			0xd300,	"OUT"	},
	{ NO_ARGS,		0xedab,	"OUTD"	},
	{ NO_ARGS,		0xeda3,	"OUTI"	},
	{ PSEUDO,		PAGE,	"PAGE"	},
	{ POP,			0xc1,	"POP"	},
	{ POP,			0xc5,	"PUSH"	},
	{ BIT,			0xcb80,	"RES"	},
	{ RET,			0xc0,	"RET"	},
	{ NO_ARGS,		0xed4d,	"RETI"	},
	{ NO_ARGS,		0xed45,	"RETN"	},
	{ RLC,			0xcb10,	"RL"	},
	{ NO_ARGS,		0x17,	"RLA"	},
	{ RLC,			0xcb00,	"RLC"	},
	{ NO_ARGS,		0x07,	"RLCA"	},
	{ NO_ARGS,		0xed6f,	"RLD"	},
	{ RLC,			0xcb18,	"RR"	},
	{ NO_ARGS,		0x1f,	"RRA"	},
	{ RLC,			0xcb08,	"RRC"	},
	{ NO_ARGS,		0x0f,	"RRCA"	},
	{ NO_ARGS,		0xed67,	"RRD"	},
	{ RST,			0xc7,	"RST"	},
	{ ADC,			0x98,	"SBC"	},
	{ NO_ARGS,		0x37,	"SCF"	},
	{ BIT,			0xcbc0,	"SET"	},
	{ RLC,			0xcb20,	"SLA"	},
	{ RLC,			0xcb28,	"SRA"	},
	{ RLC,			0xcb38,	"SRL"	},
	{ CP,			0x90,	"SUB"	},
	{ PSEUDO,		TITLE,	"TITLE"	},
	{ PSEUDO,		VAR,	"VAR"	},
	{ CP,			0xa8,	"XOR"	}
    };

    return bccsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}

/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/

OPCODE *find_operator(nam)
char *nam;
{

    static OPCODE oprtbl[] = {
	{ REG,				A,		"A"	},
	{ REG,				AF,		"AF"	},
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ REG,				B,		"B"	},
	{ REG,				BC,		"BC"	},
	{ REG,				C,		"C"	},
	{ REG,				D,		"D"	},
	{ REG,				DE,		"DE"	},
	{ REG,				E,		"E"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ REG,				H,		"H"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ REG,				HL,		"HL"	},
	{ REG,				I,		"I"	},
	{ REG,				IX,		"IX"	},
	{ REG,				IY,		"IY"	},
	{ REG,				L,		"L"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ REG,				M,		"M"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ REG,				NC,		"NC"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ REG,				NZ,		"NZ"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ REG,				P,		"P"	},
	{ REG,				PE,		"PE"	},
	{ REG,				PO,		"PO"	},
	{ REG,				R,		"R"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ REG,				SP,		"SP"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	},
	{ REG,				Z,		"Z"	}
    };

    return bccsearch(oprtbl,oprtbl + (sizeof(oprtbl) / sizeof(OPCODE)),nam);
}

static OPCODE *bccsearch(lo,hi,nam)
OPCODE *lo, *hi;
char *nam;
{
    SCRATCH int i;
    SCRATCH OPCODE *chk;

    for (;;) {
	chk = lo + (hi - lo) / 2;
	if (!(i = ustrcmp(chk -> oname,nam))) return chk;
	if (chk == lo) return NULL;
	if (i < 0) lo = chk;
	else hi = chk;
    }
}

static int ustrcmp(s,t)
char *s, *t;
{
    SCRATCH int i;

    while (!(i = toupper(*s++) - toupper(*t)) && *t++);
    return i;
}

/*  Buffer storage for line listing routine.  This allows the listing	*/
/*  output routines to do all operations without the main routine	*/
/*  having to fool with it.						*/

static FILE *list = NULL;

/*  Listing file open routine.  If a listing file is already open, a	*/
/*  warning occurs.  If the listing file doesn't open correctly, a	*/
/*  fatal error occurs.  If no listing file is open, all calls to	*/
/*  lputs() and lclose() have no effect.				*/

void lopen(nam)
char *nam;
{
    if (list) warning(TWOLST);
    else if (!(list = fopen(nam,"w"))) fatal_error(LSTOPEN);
    return;
}

/*  Listing file line output routine.  This routine processes the	*/
/*  source line saved by popc() and the output of the line assembler in	*/
/*  buffer obj into a line of the listing.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void lputs(void)
{
    SCRATCH int i, j;
    SCRATCH unsigned *o;

    if (list) {
	i = bytes;  o = obj;
	do {
	    fprintf(list,"%c  ",errcode);
	    if (listhex) {
		fprintf(list,"%04x  ",address);
		for (j = 4; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list," %02x",*o++); }
		    else fprintf(list,"   ");
		}
	    }
	    else fprintf(list,"%18s","");
	    fprintf(list,"   %s",line);  strcpy(line,"\n");
	    check_page();
	    if (ferror(list)) fatal_error(DSKFULL);
	} while (listhex && i);
    }
    return;
}

/*  Listing file close routine.  The symbol table is appended to the	*/
/*  listing in alphabetic order by symbol name, and the listing file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

static int col = 0;

void lclose(void)
{

    if (list) {
	if (sroot) {
	    list_sym(sroot);
	    if (col) fprintf(list,"\n");
	}
	fprintf(list,"\f");
	if (ferror(list) || fclose(list) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void list_sym(sp)
SYMBOL *sp;
{
    if (sp) {
	list_sym(sp -> left);
	fprintf(list,"%04x  %-10s",sp -> valu,sp -> sname);
        col = (col + 1) % SYMCOLS;
	if (col != 0) fprintf(list,"    ");
	else {
	    fprintf(list,"\n");
	    if (sp -> right) check_page();
	}
	list_sym(sp -> right);
    }
    return;
}

static void check_page()
{
    if (pagelen && !--listleft) eject = TRUE;
    if (eject) {
	eject = FALSE;  listleft = pagelen;  fprintf(list,"\f");
	if (title[0]) { listleft -= 2;  fprintf(list,"%s\n\n",title); }
    }
    return;
}

/*  Buffer storage for hex output file.  This allows the hex file	*/
/*  output routines to do all of the required buffering and record	*/
/*  forming without the	main routine having to fool with it.		*/

static FILE *hex = NULL;
static unsigned cnt = 0;
static unsigned addr = 0;
static unsigned sum = 0;
static unsigned buf[HEXSIZE];

/*  Hex file open routine.  If a hex file is already open, a warning	*/
/*  occurs.  If the hex file doesn't open correctly, a fatal error	*/
/*  occurs.  If no hex file is open, all calls to hputc(), hseek(), and	*/
/*  hclose() have no effect.						*/

void hopen(nam)
char *nam;
{

    if (hex) warning(TWOHEX);
    else if (!(hex = fopen(nam,"w"))) fatal_error(HEXOPEN);
    return;
}

/*  Hex file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void hputc(c)
unsigned c;
{

    if (hex) {
	buf[cnt++] = c;
	if (cnt == HEXSIZE) record(0);
    }
    return;
}

/*  Hex file address set routine.  The specified address becomes the	*/
/*  load address of the next record.  If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.								*/

void hseek(a)
unsigned a;
{

    if (hex) {
	if (cnt) record(0);
	addr = a;
    }
    return;
}

/*  Hex file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void hclose(void)
{

    if (hex) {
	if (cnt) record(0);
	record(1);
	if (fclose(hex) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void record(typ)
unsigned typ;
{
    SCRATCH unsigned i;

    putc(':',hex);  putb(cnt);  putb(high(addr));
    putb(low(addr));  putb(typ);
    for (i = 0; i < cnt; ++i) putb(buf[i]);
    putb(low(-sum));  putc('\n',hex);  /* was (-sum) HRJ*/

    addr += cnt;  cnt = 0;

    if (ferror(hex)) fatal_error(DSKFULL);
    return;
}

static void putb(b)
unsigned b;
{
    static char digit[] = "0123456789ABCDEF";

    putc(digit[b >> 4],hex);  putc(digit[b & 0x0f],hex);
    sum += b;  return;
}

/*  Error handler routine.  If the current error code is non-blank,	*/
/*  the error code is filled in and the	number of lines with errors	*/
/*  is adjusted.							*/

void error(code)
char code;
{
    if (errcode == ' ') { errcode = code;  ++errors; }
    return;
}

/*  Fatal error handler routine.  A message gets printed on the stderr	*/
/*  device, and the program bombs.					*/

void fatal_error(msg)
char *msg;
{
    printf("Fatal Error -- %s\n",msg);
    exit(-1);
}

/*  Non-fatal error handler routine.  A message gets printed on the	*/
/*  stderr device, and the routine returns.				*/

void warning(msg)
char *msg;
{
    printf("Warning -- %s\n",msg);
    return;
}
