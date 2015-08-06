BANKREG	EQU	0ffH

#include "library.asm"

;
; Main program
;
MAIN
	LD	IX, RET1
	JP	LCDINIT
RET1

	LD	HL, HELLOSTR
	LD	IX, RET2
	JP 	LCDPRINT
RET2

	LD	D, 2		; 2 seconds wait
	LD	IX, RET3
;	JP	WAITSECS
RET3

OKAY0
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

	LD	HL, BADMMIO
	LD	IX, RET4
	JP	LCDPRINT
RET4
	HALT

OKAY1
	LD	HL, GOODMMIO
	LD	IX, RET4A
	JP	LCDPRINT
RET4A

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

	LD	HL, BADMEMLO
	LD	IX, RET5
	JP	LCDPRINT
RET5
	HALT

OKAY2
	LD	HL, GOODMEMLO
	LD	IX, RET6
	JP	LCDPRINT
RET6

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
	LD	HL, WROTE
	LD	IX, RET7
	JP	LCDPRINT
RET7

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
	LD	B, H
	LD	C, L
	LD	D, E

	LD	HL, BANKBAD
	LD	IX, RET8
	JP	PHEXBYTE

RET8
	LD	HL, BANKVAL
	LD	D, B
	LD	IX, RET9
	JP	PHEXBYTE
RET9
	LD	HL, BANKVAL+2
	LD	D, C
	LD	IX, RET10
	JP	PHEXBYTE

RET10
	LD	HL, BANKVAL
	LD	IX, RET11
	JP	LCDPRINT

RET11
	HALT

ALLCOOL
	LD	HL, ALLGOOD
	LD	IX, RET12
	JP	LCDPRINT

RET12
	HALT
	

;
; put the string value of D into the location at HL
; destroys IY, A, BC
;
PHEXBYTE
	LD	A, D
	AND	0fH
	CP	0aH
	JP	C, DIG1LESS
	ADD	A, "a"
	JP	DIG1DONE
DIG1LESS
	ADD	A, "0"
DIG1DONE
	LD	(HL), A
	INC	HL
	LD	A, D
	AND	0f0H
	SRL	A
	SRL	A
	SRL	A
	SRL	A
	CP	0aH
	JP	C, DIG2LESS
	ADD	A, "a"
	JP	DIG2DONE
DIG2LESS
	ADD	A, "0"
DIG2DONE
	LD	(HL), A
	JP	(IX)




HELLOSTR	DB	"Alice 2 is alive", 10
		DB	"ALIVE!!!", 10
		DB	"Memory test...", 10
		DB	"ROM is working", 10, 0

BADMMIO		DB	"MMIO kept CAFE!", 0
GOODMMIO	DB	"MMIO good", 10, 0
BADMEMLO	DB	"8000 lost CAFE!", 0
GOODMEMLO	DB	"8000 good", 10, 0
BANKBAD		DB	"nn failed,"
BANKVAL		DB	"nnnn", 0

ALLGOOD		DB	"Everything is", 10, "wonderful!", 0

WROTE		DB	"Wrote banks.", 10, 0

	END
