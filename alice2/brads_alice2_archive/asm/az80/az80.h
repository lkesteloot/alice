/*
	HEADER:		CUG276;
	TITLE:		Z-80 Cross-Assembler (Portable);
	FILENAME:	AZ80.H;
	VERSION:	0.1;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Z-80 and NSC800 families of
			microprocessors.  The program is written in portable C
			rather than BDS C.  All assembler features are
			supported except relocation, linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Zilog, Z-80, NSC800;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program is written in as portable C as possible.
			A port to BDS C would be extremely difficult.  A port
			to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      Z-80 Cross-Assembler in Portable C

		Copyright (c) 1986-1988 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	JUNE 1987	Derived from my S-6 cross-assembler.  WCC3.

0.1	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "AZ80 FILE.ASM -L".  WCC3 per Alex Cameron.

	JAN 2010	HRJ fixes for lcc windows C compiler

This header file contains the global constants and data type definitions for
all modules of the cross-assembler.  This also seems a good place to put the
compilation and linkage instructions for the animal.  This list currently
includes the following compilers:

	    Compiler Name		Op. Sys.	Processor

	1)  Aztec C86			CP/M-86		8086, 8088
					MSDOS/PCDOS

	2)  AZTEC C II			CP/M-80		8080, Z-80

	3)  Computer Innovations C86	MSDOS/PCDOS	8086, 8088

	4)  Eco-C			CP/M-80		Z-80

	5)  Eco-C88			MSDOS/PCDOS	8086, 8088

	6)  HP C			HP-UX		68000

	7)  Lattice C			MSDOS/PCDOS	8086, 8088

	8)  Microsoft C			MSDOS/PCDOS	8086, 8088

	9)  QNIX C			QNIX		8086, 8088

Further additions will be made to the list as users feed the information to
me.  This particularly applies to UNIX and IBM-PC compilers.

Compile-assemble-link instructions for this program under various compilers
and operating systems:

    1)	Aztec C86:

	A)  Uncomment out the "#define AZTEC_C 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Assuming that all files are on drive A:, run the following
	    sequence of command lines:

		A>cc az80
		A>cc az80eval
		A>cc az80util
		A>ln az80.o az80eval.o az80util.o -lc
		A>era az80*.o

    2)  Aztec CII (version 1.06B):

	A)  Uncomment out the "#define AZTEC_C 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Assuming the C compiler is called "CC.COM" and all files are
	    on drive A:, run the following sequence of command lines:

		A>cc az80
		A>as -zap az80
		A>cc az80eval
		A>as -zap az80eval
		A>cc az80util
		A>as -zap az80util
		A>ln az80.o az80eval.o az80util.o -lc
		A>era az80*.o

    3)  Computer Innovations C86:

	A)  Uncomment out the "#define CI_C86 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Compile the files AZ80.C, AZ80EVAL.C, and AZ80UTIL.C.  Link
	    according to instructions that come with the compiler.

    4)  Eco-C (CP/M-80 version 3.10):

	A)  Uncomment out the "#define ECO_C 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Assuming all files are on drive A:, run the following sequence of
	    command lines:

		A>cp az80 -i -m
		A>cp az80eval -i -m
		A>cp az80util -i -m
		A>l80 az80,az80eval,az80util,az80/n/e
		A>era az80*.mac
		A>era az80*.rel

    5)  Eco-C88:

	A)  Uncomment out the "#define ECO_C 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Compile the files AZ80.C, AZ80EVAL.C, and AZ80UTIL.C.  Link
	    according to instructions that come with the compiler.

    6)  HP-UX (a UNIX look-alike running on an HP-9000 Series 200/500,
	68000-based machine):

	A)  Uncomment out the "#define HP_UX 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Run the following command line:

		. cc az80.c az80eval.c az80util.c

    7)  Lattice C:

	A)  Uncomment out the "#define LATTICE_C 1" line and comment out all
	    other compiler names in AZ80.H.

	B)  Compile the files AZ80.C, AZ80EVAL.C, and AZ80UTIL.C.  Link
	    according to instructions that come with the compiler.

    8)  Microsoft C (version 3.00):

	A)  Uncomment out the "#define MICROSOFT_C 1" line and comment out
	    all other compiler names in AZ80.H.

	B)  Run the following command line:

		C>cl az80.c az80eval.c az80util.c

    9)	QNIX C:

	A)  Uncomment out the "#define QNIX 1" line and comment out all other
	    compiler names in AZ80.H.

	B)  Run the following command line:

		. cc az80.c az80eval.c az80util.c

Note that, under CP/M-80, you can't re-execute a core image from a previous
assembly run with the "@.COM" trick.  This technique is incompatible with the
Aztec CII compiler, so I didn't bother to support it at all.
*/

#include <stdio.h>

/*  Comment out all but the line containing the name of your compiler:	*/

/* #define		AZTEC_C		1 HRJ */
/* #define	CI_C86		1					*/
/* #define	ECO_C		1					*/
/* #define	HP_UX		1					*/
/* #define	LATTICE_C	1					*/
/* #define	MICROSOFT_C	1					*/
/* #define	QNIX		1					*/

/*  Compiler dependencies:						*/

#ifdef	AZTEC_C
#define	getc(f)		agetc(f)
#define	putc(c,f)	aputc(c,f)
#endif

#ifndef	ECO_C
#define	FALSE		0
#define	TRUE		(!0)
#endif

#ifdef	LATTICE_C
#define	void		int
#endif

#ifdef	QNIX
#define	fprintf		tfprintf
#define	printf		tprintf
#endif

/*  On 8-bit machines, the static type is as efficient as the register	*/
/*  type and far more efficient than the auto type.  On larger machines	*/
/*  such as the 8086 family, this is not necessarily the case.  To	*/
/*  let you experiment to see what generates the fastest, smallest code	*/
/*  for your machine, I have declared internal scratch variables in	*/
/*  functions "SCRATCH int", "SCRATCH unsigned", etc.  A SCRATCH	*/
/*  variable is made static below, but you might want to try register	*/
/*  instead.								*/

#define	SCRATCH		static

/*  A slow, but portable way of cracking an unsigned into its various	*/
/*  component parts:							*/

#define	clamp(u)	((u) &= 0xffff)
#define	high(u)		(((u) >> 8) & 0xff)
#define	low(u)		((u) & 0xff)
#define	word(u)		((u) & 0xffff)

/*  The longest source line the assembler can hold without exploding:	*/

#define	MAXLINE		255

/*  The maximum number of source files that can be open simultaneously:	*/

#define	FILES		4

/*  The fatal error messages generated by the assembler:		*/

#define	ASMOPEN		"Source File Did Not Open"
#define	ASMREAD		"Error Reading Source File"
#define	DSKFULL		"Disk or Directory Full"
#define	FLOFLOW		"File Stack Overflow"
#define	HEXOPEN		"Object File Did Not Open"
#define	IFOFLOW		"If Stack Overflow"
#define	LSTOPEN		"Listing File Did Not Open"
#define	NOASM		"No Source File Specified"
#define	SYMBOLS		"Too Many Symbols"

/*  The warning messages generated by the assembler:			*/

#define	BADOPT		"Illegal Option Ignored"
#define	NOHEX		"-o Option Ignored -- No File Name"
#define	NOLST		"-l Option Ignored -- No File Name"
#define	TWOASM		"Extra Source File Ignored"
#define	TWOHEX		"Extra Object File Ignored"
#define	TWOLST		"Extra Listing File Ignored"

/*  Line assembler (AZ80.C) constants:					*/

#define	BIGINST		4		/*  longest instruction length	*/
#define	IFDEPTH		16		/*  maximum IF nesting level	*/
#define	NOP		0x00		/*  processor's NOP opcode	*/
#define	ON		1		/*  assembly turned on		*/
#define	OFF		-1		/*  assembly turned off		*/

/*  Line assembler (AZ80.C) opcode attribute word flag masks:		*/

#define	PSEUDO		0400	/*  is pseudo op			*/
#define	ISIF		0200	/*  is IF, ELSE, or ENDIF		*/

/*  Line assembler (AZ80.C) pseudo-op opcode token values:		*/

#define	DB		1
#define	DC		2
#define	DS		3
#define	DW		4
#define	ELSE		5
#define	END		6
#define	ENDIF		7
#define	EQU		8
#define	IF		9
#define	INCL		10
#define	ORG		11
#define	PAGE		12
#define	TITLE		13
#define	VAR		14

/*  Line assembler (AZ80.C) machine opcode attribute values:		*/

#define	OPTYPE		0177	/*  Opcode type:			*/
#define	LD		0022	/*	LD				*/
#define	POP		0021	/*	POP, PUSH			*/
#define	EX		0020	/*	EX				*/
#define	ADD		0017	/*	ADD				*/
#define	ADC		0016	/*	ADC, SBC			*/
#define	CP		0015	/*	AND, CP, OR, SUB, XOR		*/
#define	DEC		0014	/*	DEC, INC			*/
#define	BIT		0013	/*	BIT, RES, SET			*/
#define	RLC		0012	/*	RL, RLC, RR, RRC, SLA, SRA, SRL	*/
#define	JR		0011	/*	JR				*/
#define	DJNZ		0010	/*	DJNZ				*/
#define	JP		0007	/*	JP				*/
#define	CALL		0006	/*	CALL				*/
#define	RET		0005	/*	RET				*/
#define	IN		0004	/*	IN				*/
#define	OUT		0003	/*	OUT				*/
#define	RST		0002	/*	RST				*/
#define	IM		0001	/*	IM				*/
#define	NO_ARGS		0000	/*	CCF, CPD, CPDR, CPI, CPIR, etc.	*/

/*  Opcode argument field parser (AZ80EVAL.C) attribute values:		*/

#define	B		000	/*  B					*/
#define	C		001	/*  C					*/
#define	D		002	/*  D					*/
#define	E		003	/*  E					*/
#define	H		004	/*  H					*/
#define	L		005	/*  L					*/
#define	HL_IND		006	/*  (HL)				*/
#define	A		007	/*  A					*/
#define	NZ		010	/*  NZ					*/
#define	Z		011	/*  Z					*/
#define	NC		012	/*  NC					*/
#define	CY		013	/*  C becomes CY on JP, CALL, etc.	*/
#define	PO		014	/*  PO					*/
#define	PE		015	/*  PE					*/
#define	P		016	/*  P					*/
#define	M		017	/*  M					*/
#define	BC		020	/*  BC					*/
#define	DE		022	/*  DE					*/
#define	HL		024	/*  HL					*/
#define	AF		026	/*  AF					*/
#define	IX		034	/*  IX					*/
#define	IY		044	/*  IY					*/
#define	SP		046	/*  SP					*/
#define	BC_IND		050	/*  (BC)				*/
#define	DE_IND		052	/*  (DE)				*/
#define	IX_IND		056	/*  (IX+d)				*/
#define	IY_IND		066	/*  (IY+d)				*/
#define	SP_IND		076	/*  (SP)				*/
#define	I		070	/*  I					*/
#define	R		071	/*  R					*/
#define	C_IND		072	/*  (C)					*/
#define	NUM		073	/*  nn					*/
#define	NUM_IND		074	/*  (nn)				*/
#define	NONE		075	/*  nothing				*/

/*  Lexical analyzer (AZ80EVAL.C) token buffer and stream pointer:	*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char sval[MAXLINE + 1];
} TOKEN;

/*  Lexical analyzer (AZ80EVAL.C) token attribute values:		*/

#define	EOL		0	/*  end of line				*/
#define	SEP		1	/*  field separator			*/
#define	OPR		2	/*  operator				*/
#define	STR		3	/*  character string			*/
#define	VAL		4	/*  value				*/
#define	REG		5	/*  register tag			*/

/*  Lexical analyzer (AZ80EVAL.C) token attribute word flag masks:	*/

#define	BINARY		0x8000	/*  Operator:	is binary operator	*/
#define	UNARY		0x4000	/*		is unary operator	*/
#define	PREC		0x0f00	/*		precedence		*/

#define	FORWD		0x8000	/*  Value:	is forward referenced	*/
#define	SOFT		0x4000	/*		is redefinable		*/

#define	TYPE		0x000f	/*  All:	token type		*/

/*  Lexical analyzer (AZ80EVAL.C) operator token values (unlisted ones	*/
/*  use ASCII characters):						*/

#define	AND		0
#define	GE		1
#define	HIGH		2
#define	LE		3
#define	LOW		4
#define	MOD		5
#define	NE		6
#define	NOT		7
#define	OR		8
#define	SHR		9
#define	SHL		10
#define	XOR		11

/*  Lexical analyzer (AZ80EVAL.C) operator precedence values:		*/

#define	UOP1		0x0000	/*  unary +, unary -			*/
#define	MULT		0x0100	/*  *, /, MOD, SHL, SHR			*/
#define	ADDIT		0x0200	/*  binary +, binary -			*/
#define	RELAT		0x0300	/*  >, >=, =, <=, <, <>			*/
#define	UOP2		0x0400	/*  NOT					*/
#define	LOG1		0x0500	/*  AND					*/
#define	LOG2		0x0600	/*  OR, XOR				*/
#define	UOP3		0x0700	/*  HIGH, LOW				*/
#define	RPREN		0x0800	/*  )					*/
#define	LPREN		0x0900	/*  (					*/
#define	ENDEX		0x0a00	/*  end of expression			*/
#define	START		0x0b00	/*  beginning of expression		*/

/*  Utility package (AZ80UTIL.C) symbol table routines:			*/

struct _symbol {
    unsigned attr;
    unsigned valu;
    struct _symbol *left, *right;
    char sname[1];
};

typedef struct _symbol SYMBOL;

#define	SYMCOLS		4

/*  Utility package (AZ80UTIL.C) opcode/operator table routines:		*/

typedef struct {
    unsigned attr;
    unsigned valu;
    char oname[6];
} OPCODE;

/*  Utility package (AZ80UTIL.C) hex file output routines:		*/

#define	HEXSIZE		32
