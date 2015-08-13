BANKREG	EQU	0ffH

#include "library.asm"

;
; Memory test program
;
MAIN
	LD	HL, HELLOSTR
	CALL 	LCDPRINT

	LD	A, 2		; 2 seconds wait
	CALL	WAITSECS

	; write 0 to 0x4000
	; write 0xCAFE to 0x4000
	; must not read back 0xCAFE
	LD	BC, 0H
	LD	(7FFEH), BC
	LD	BC, 0cafeH
	LD	(7FFEH), BC
	LD	HL, (7FFEH)
	SCF			; Set carry
	CCF			; Complement carry
	SBC	HL, BC
	JP	NZ, OKAY1

	LD	HL, MEMIS48K
	CALL	LCDPRINT
	HALT

OKAY1
	LD	HL, MEMIS32K
	CALL	LCDPRINT

	; write 0 to 0x8000
	; write 0xCAFE to 0x8000
	; must read back 0xCAFE
	LD	BC, 0H
	LD	(8000H), BC
	LD	BC, 0cafeH
	LD	(8000H), BC
	LD	HL, (8000H)
	SCF			; Set carry
	CCF			; Complement carry
	SBC	HL, BC
	JP	Z, OKAY2

	LD	HL, BADMEMLO
	CALL	LCDPRINT
	HALT

OKAY2
	LD	HL, GOODMEMLO
	CALL	LCDPRINT

	; for all banks
	;    write 0xBE<bank#> to 0xC000
	LD	D, 0beh
	LD	E, 0
	LD	C, BANKREG

BKLOOP1
	LD	A, E
	CP	16
	JP	Z, CHECKBANKS

	LD	HL, WRTBANK
	CALL	PHEXBYTE

	OUT	(C), E		; set memory bank register

	LD	IY, 0c000H
	LD	B, 16		; I think 16 might be enough.

BKLOOP1a
	LD	HL, WRTVAL
	CALL	PHEXWORD
	LD	HL, WRITING
	CALL	LCDPRINT
	LD	(IY+0), D
	LD	(IY+1), E

	INC	IY
	DJNZ	BKLOOP1a

	INC	E
	JP	BKLOOP1

CHECKBANKS
	LD	HL, CHECKING
	CALL	LCDPRINT

	; for all banks
	;    must read 0xBE<bank#>

	LD	D, 0beh
	LD	E, 0
	LD	C, BANKREG

BKLOOP2
	LD	A, E
	CP	16
	JP	Z, ALLCOOL

	LD	HL, RDBANK
	CALL	PHEXBYTE

	OUT	(C), E		; set memory bank register

	LD	IY, 0c000H
	LD	B, 16		; I think 16 might be enough.

BKLOOP2a
	LD	HL, RDVAL
	CALL	PHEXWORD
	LD	HL, READING
	CALL	LCDPRINT
	LD	H, (IY+0)
	LD	L, (IY+1)

	SCF			; Set carry
	CCF			; Complement carry
	SBC	HL, DE
	JP	NZ, BADBANK

	INC	IY
	DJNZ	BKLOOP2a

	INC	E
	JP	BKLOOP2

BADBANK
	; A already contains the bank number
	; HL already contains the bogus value 

	LD	IX, BANKBAD
	CALL	PHEXBYTE

	LD	IX, BANKVAL
	CALL	PHEXWORD

	LD	IX, BANKVAL
	CALL	LCDPRINT

	HALT

ALLCOOL
	LD	HL, ALLGOOD
	CALL	LCDPRINT

	HALT
	

HELLOSTR	DB	"Alice 2 is alive", 10
		DB	"ROM is working", 0

MEMIS32K	DB	"Mem in 32K mode", 10, 0
MEMIS48K	DB	"Mem in 48K mode", 10, 0

BADMEMLO	DB	"0x8000 lost CAFE!", 0
GOODMEMLO	DB	"fixed mem okay", 10
		DB	"Writing banks...", 10, 0

CHECKING	DB	"Reading banks...", 10, 0

WRITING		DB	"writing "
WRTBANK		DB	"nn:"
WRTVAL		DB	"nnnn", 0
READING		DB	"reading"
RDBANK		DB	"nn:"
RDVAL		DB	"nnnn", 0

BANKBAD		DB	"nn failed:"
BANKVAL		DB	"nnnn", 0

ALLGOOD		DB	"Memory Okay.", 10, 0


	END
