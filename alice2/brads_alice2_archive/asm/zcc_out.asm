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
	LD	HL, -30
	ADD	HL, SP
	LD	SP, HL
	; Variable "buffer" is at location IY - 30

	; (tst.c, 66) }
zcc$3
	; (tst.c, 62) while (1) {
	LD	HL, 1
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	Z, zcc$4
	; (tst.c, 63) printf("What's your name?\n");
	LD	HL, zcc$1
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP

	; (tst.c, 64) gets(buffer);
	LD	BC, -30
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	CALL	_gets
	INC	SP
	INC	SP

	; (tst.c, 65) printf("Hey there %s\n", buffer);
	LD	BC, -30
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

