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
	LD	HL, -6
	ADD	HL, SP
	LD	SP, HL
	; Variable "p" is at location IY - 2
	; Variable "j" is at location IY - 4
	; Variable "i" is at location IY - 6

	; (tst.c, 109) for (i = 56; i < 56 + 170; i++) {
	LD	HL, 56
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -6
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	zcc$4
zcc$1
	; (tst.c, 110) p = (char *)(16396 + i*44);  /* 352/8 */
	LD	HL, 16396
	PUSH	HL
	LD	L, (IY + -6)
	LD	H, (IY + -5)
	LD	DE, 0
	ADD	HL, HL
	ADD	HL, HL
	EX	DE, HL
	ADD	HL, DE
	EX	DE, HL
	ADD	HL, HL
	EX	DE, HL
	ADD	HL, DE
	EX	DE, HL
	ADD	HL, HL
	ADD	HL, HL
	ADD	HL, DE
	POP	BC
	ADD	HL, BC
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	; (tst.c, 111) for (j = 0; j < 25; j++) {
	LD	HL, 0
	PUSH	HL
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
	JP	zcc$8
zcc$5
	; (tst.c, 112) *p = 0xff;
	LD	HL, 255
	PUSH	HL
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	POP	IX
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H

	; (tst.c, 113) p++;
	PUSH	IY
	POP	IX
	LD	BC, -2
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$6
	; (tst.c, 111) for (j = 0; j < 25; j++) {
	PUSH	IY
	POP	IX
	LD	BC, -4
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$8
	LD	L, (IY + -4)
	LD	H, (IY + -3)
	PUSH	HL
	LD	HL, 25
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, zcc$9
	LD	HL, 0
zcc$9
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	NZ, zcc$5
zcc$7
zcc$2
	; (tst.c, 109) for (i = 56; i < 56 + 170; i++) {
	PUSH	IY
	POP	IX
	LD	BC, -6
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	INC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$4
	LD	L, (IY + -6)
	LD	H, (IY + -5)
	PUSH	HL
	LD	HL, 56
	PUSH	HL
	LD	HL, 170
	POP	BC
	ADD	HL, BC
	POP	DE
	SCF	
	CCF	
	SBC	HL, DE
	LD	HL, 1
	JP	P, zcc$10
	LD	HL, 0
zcc$10
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	NZ, zcc$1
zcc$3

	LD	SP, IY
	POP	IY

	RET	

_src	DW	0
_dst	DW	0
_buffer	DW	0
_format	DW	0


