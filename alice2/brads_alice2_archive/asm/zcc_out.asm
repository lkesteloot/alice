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
	LD	HL, -34
	ADD	HL, SP
	LD	SP, HL
	; Variable "j" is at location IY - 2
	; Variable "i" is at location IY - 4
	; Variable "buffer" is at location IY - 34

	; (tst.c, 72) }
zcc$3
	; (tst.c, 63) while (1) {
	LD	HL, 1
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	Z, zcc$4
	; (tst.c, 64) printf("What's your name?\n");
	LD	HL, zcc$1
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP

	; (tst.c, 65) gets(buffer);
	LD	BC, -34
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	CALL	_gets
	INC	SP
	INC	SP

	; (tst.c, 66) printf("Hey there %s\n", buffer);
	LD	BC, -34
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, zcc$2
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP
	INC	SP
	INC	SP

	; (tst.c, 67) for (j = 0; j < 8; j++) {
	LD	HL, 0
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	zcc$8
zcc$5
	; (tst.c, 68) for (i = 0; i < 10000; i++) {
	LD	HL, 0
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	zcc$12
zcc$9
zcc$10
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$12
	LD	L, (IY + -4)
	LD	H, (IY + -3)
	PUSH	HL
	LD	HL, 10000
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, zcc$13
	LD	HL, 0
zcc$13
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	NZ, zcc$9
zcc$11
zcc$6
	; (tst.c, 67) for (j = 0; j < 8; j++) {
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$8
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	HL, 8
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, zcc$14
	LD	HL, 0
zcc$14
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	NZ, zcc$5
zcc$7
	JP	zcc$3
zcc$4

	LD	SP, IY
	POP	IY

	RET	

_src	DW	0
_dst	DW	0
_buffer	DW	0
_format	DW	0

zcc$2	DB	"Hey there %s", 10, 0
zcc$1	DB	"What's your name?", 10, 0

