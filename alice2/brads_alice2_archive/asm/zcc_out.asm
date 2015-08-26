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

	; (tst.c, 106) for (i = 200; i >= 100; i--) {
	LD	HL, 200
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
	; (tst.c, 108) p = (char *)(0x4000 + i*22);
	LD	HL, 16384
	PUSH	HL
	LD	L, (IY + -6)
	LD	H, (IY + -5)
	LD	DE, 0
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

	; (tst.c, 110) for (j = 0; j < 22; j++) {
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
	; (tst.c, 111) p[j] = 0xff;
	LD	HL, 255
	PUSH	HL
	LD	L, (IY + -2)
	LD	H, (IY + -1)
	PUSH	HL
	LD	L, (IY + -4)
	LD	H, (IY + -3)
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	POP	HL
	LD	(IX + 0), L
zcc$6
	; (tst.c, 110) for (j = 0; j < 22; j++) {
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
	LD	HL, 22
	POP	DE
	OR	A ; Clear carry flag
	SBC	HL, DE ; HL = HL - DE - C
	LD	HL, 0
	JP	M, zcc$9
	JP	Z, zcc$9
	LD	HL, 1
zcc$9
	OR	A
	LD	DE, 0
	SBC	HL, DE
	JP	NZ, zcc$5
zcc$7
zcc$2
	; (tst.c, 106) for (i = 200; i >= 100; i--) {
	PUSH	IY
	POP	IX
	LD	BC, -6
	ADD	IX, BC
	LD	L, (IX + 0)
	LD	H, (IX + 1)
	DEC	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
zcc$4
	LD	L, (IY + -6)
	LD	H, (IY + -5)
	PUSH	HL
	LD	HL, 100
	POP	DE
	OR	A ; Clear carry flag
	SBC	HL, DE ; HL = HL - DE - C
	LD	HL, 1
	JP	M, zcc$10
	JP	Z, zcc$10
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


