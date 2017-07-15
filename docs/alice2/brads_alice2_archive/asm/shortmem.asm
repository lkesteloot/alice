BANKREG	EQU	0ffH

;
; Main program
;
MAIN

	; write 0 to 0x4000
	; write 0xCAFE to 0x4000
	; must not read back 0xCAFE
	LD	BC, 0H
	LD	(4000H), BC
	LD	BC, 0cafeH
	LD	(4000H), BC
	LD	HL, (4000H)
	SBC	HL, BC
	JP	NZ, OKAY1

	HALT

OKAY1

	; write 0 to 0x8000
	; write 0xCAFE to 0x8000
	; must read back 0xCAFE
	LD	BC, 0H
	LD	(8000H), BC
	LD	BC, 0cafeH
	LD	(8000H), BC
	LD	HL, (8000H)
	SBC	HL, BC
	JP	Z, OKAY2

	HALT

OKAY2

	; for all banks
	;    write 0xBE<bank#> to 0xC000
	LD	D, 0beh
	LD	E, 0
	LD	C, BANKREG

BKLOOP1
	LD	A, E
	CP	16
	JP	Z, CHECKBANKS

	OUT	(C), E

	LD	(0c000H), DE

	INC	E
	JP	BKLOOP1

CHECKBANKS

	; for all banks
	;    must read 0xBE<bank#>

	LD	D, 0beh
	LD	E, 0
	LD	C, BANKREG

BKLOOP2
	LD	A, E
	CP	16
	JP	Z, ALLCOOL

	OUT	(C), E

	LD	HL, (0c000H)

	SCF			; Set carry
	CCF			; Complement carry
	SBC	HL, DE
	JP	NZ, BADBANK

	INC	E
	JP	BKLOOP2

BADBANK
	HALT

ALLCOOL
	HALT
	
	END
