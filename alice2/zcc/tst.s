	LD	A, 0
	LD	(SHOULD_RUN), A
	CALL	_main
	HALT	


	; 
	; Function "main".
	; 
_main
	PUSH	IY
	LD	IY, 0
	ADD	IY, SP
	LD	HL, -4
	ADD	HL, SP
	LD	SP, HL
	; Variable "i" is at location IY - 2
	; Variable "p" is at location IY - 4

	; (tst.c, 11) for (i = 0; i < 16384; i++) {
	LD	HL, 0
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	$6
$3
	; (tst.c, 12) p = (char *)i;
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	; (tst.c, 13) *p = i & 0xFF;
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 255
	POP	DE
	LD	A, H
	AND	D
	LD	H, A
	LD	A, L
	AND	E
	LD	L, A
	PUSH	HL
	LD	L, (IY + -4)
	LD	H, (IY + -3)
	PUSH	HL
	POP	IX
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
$4
	; (tst.c, 11) for (i = 0; i < 16384; i++) {
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
$6
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 16384
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, $7
	LD	HL, 0
$7
	LD	DE, 0
	ADD	HL, DE
	JP	NZ, $3
$5

	; (tst.c, 16) for (i = 0; i < 16384; i++) {
	LD	HL, 0
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	$11
$8
	; (tst.c, 17) p = (char *)i;
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	; (tst.c, 21) }
	; (tst.c, 18) if (*p != (i & 0xFF)) {
	LD	L, (IY + -4)
	LD	H, (IY + -3)
	PUSH	HL
	POP	IX
	LD	H, (IX + 1)
	LD	L, (IX)
	PUSH	HL
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 255
	POP	DE
	LD	A, H
	AND	D
	LD	H, A
	LD	A, L
	AND	E
	LD	L, A
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 0
	JP	Z, $13
	LD	HL, 1
$13
	LD	DE, 0
	ADD	HL, DE
	JP	Z, $12
	; (tst.c, 19) printf("Error at %d\nFix it now!\\\t\n", i);
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, $1
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP
	INC	SP
	INC	SP
$12
$9
	; (tst.c, 16) for (i = 0; i < 16384; i++) {
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
$11
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 16384
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, $14
	LD	HL, 0
$14
	LD	DE, 0
	ADD	HL, DE
	JP	NZ, $8
$10

	; (tst.c, 23) for (; i < 16384; i++) {
	LD	HL, 1
	JP	$18
$15
	; (tst.c, 24) p;
	LD	L, (IY + -4)
	LD	H, (IY + -3)
$16
	; (tst.c, 23) for (; i < 16384; i++) {
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
$18
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 16384
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, $19
	LD	HL, 0
$19
	LD	DE, 0
	ADD	HL, DE
	JP	NZ, $15
$17

	; (tst.c, 27) i = i * 5;
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	LD	D, H
	LD	E, L
	ADD	HL, HL
	ADD	HL, HL
	ADD	HL, DE
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	; (tst.c, 30) p = "This is\na long\tstring.  Dude.";
	LD	HL, $2
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	LD	SP, IY
	POP	IY

	RET	

_src	DW	0
_dst	DW	0
_buffer	DW	0
_format	DW	0

$2	DB	"This is", 10, "a long", 9, "string.  Dude.", 0
$1	DB	"Error at %d", 10, "Fix it now!", 92, 9, 10, 0

