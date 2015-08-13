	CALL	_main
	HALT	


	; 
	; Function "main".
	; 
_main
	PUSH	IY
	LD	IY, 0
	ADD	IY, SP
	LD	HL, -10
	ADD	HL, SP
	LD	SP, HL
	; Variable "foo" is at location IY - 10

#if 0
	LD	HL, 1
	PUSH	HL
	LD	BC, -10
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, 4
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
#endif

	LD	HL, 1
	JP	$6
$3
	LD	HL, $1
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP

	LD	BC, -10
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, 4
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	LD	H, (IX + 1)
	LD	L, (IX)
	PUSH	HL
	LD	BC, -10
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, 4
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	LD	H, (IX + 1)
	LD	L, (IX)
	PUSH	HL
	LD	HL, $2
	PUSH	HL
	CALL	_printf
	INC	SP
	INC	SP
	INC	SP
	INC	SP
	INC	SP
	INC	SP

	LD	BC, -10
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, 4
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	LD	H, (IX + 1)
	LD	L, (IX)
	ADD	HL, HL
	PUSH	HL
	LD	BC, -10
	PUSH	IY
	POP	HL
	ADD	HL, BC
	PUSH	HL
	LD	HL, 4
	POP	BC
	ADD	HL, BC
	PUSH	HL
	POP	IX
	POP	HL
	LD	(IX + 0), L
	LD	(IX + 1), H
$4
	LD	HL, 1
$6
	LD	HL, 1
	LD	DE, 0
	ADD	HL, DE
	JP	NZ, $3
$5

	LD	SP, IY
	POP	IY

	RET	


$2	DB	"Cool, eh? 0x%x %d", 10, 0
$1	DB	"This was compiled.", 10, 0

