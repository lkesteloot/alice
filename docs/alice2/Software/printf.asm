
;
; int printf(char *format, ...) -- just like C
;
;   Only supports %d, %x, and %s
;
;   internally:
;     HL = pointer to format string
;     IX = pointer to output buffer
;     IY = pointer to next arg to be processed
;

_printf
	PUSH	IY
	LD	IY, 0
	ADD	IY, SP

	PUSH	AF
	PUSH	IX

	; IY points at IY on stack
	INC	IY
	INC	IY	; points at return address
	INC	IY
	INC	IY	; points at first argument

	LD	L, (IY + 0)	; format string into HL
	LD	H, (IY + 1)
	INC	IY
	INC	IY

	LD	IX, PRINTF_BUF

PRINTF_LOOP
	LD	A, (HL)
	CP	'%'
	JP	NZ, PRINTF_NOT_PERCENT

	INC	HL
	LD	A, (HL)
	CP	'd'
	JP	NZ, PRINTF_NOT_DECIMAL

	CALL	PRINTF_CONVERT_DECIMAL

	INC	HL
	JP	PRINTF_LOOP

PRINTF_NOT_DECIMAL
	CP	'x'
	JP	NZ, PRINTF_NOT_HEX

	CALL	PRINTF_CONVERT_HEX

	INC	HL
	JP	PRINTF_LOOP

PRINTF_NOT_HEX
	CP	's'
	JP	NZ, PRINTF_NOT_PERCENT

	CALL	PRINTF_CONVERT_STRING

	INC	HL
	JP	PRINTF_LOOP

PRINTF_NOT_PERCENT
	LD	(IX), A
	INC	IX
	INC	HL
	CP	0
	JP	NZ, PRINTF_LOOP

	LD	HL, PRINTF_BUF
	CALL	LCDPRINT
	LD	HL, 0

	POP	IX
	POP	AF
	POP	IY

	RET	

PRINTF_BUF	DS	48  ; 24 bytes on the display, pad to be sure

;
; PRINTF_CONVERT_DECIMAL
;
;   Input:
;       IY = pointer to int
;       IX = buffer to convert into
;   Output:
;       IY = incremented past int
;       IX = incremented past decimal version of int
;   Internally:
;       HL = int
;

PRINTF_CONVERT_DECIMAL
	PUSH	HL
	PUSH	AF
	PUSH	BC

	LD	L, (IY + 0)
	LD	H, (IY + 1)
	INC	IY
	INC	IY

	XOR	A		; reset carry and set A to 0
	LD	B, A		; BC = 0
	LD	C, A
	ADC	HL, BC		; HL = HL (set zero flag)
	JP	NZ, PCD_NOT_ZERO

	LD	A, '0'
	LD	(IX), A
	INC	IX

	JP	PCD_END

PCD_NOT_ZERO
	PUSH	IY
	PUSH	DE

	LD	A, 0		; D = 0 (printed flag)
	LD	D, A

	LD	IY, PCD_TABLE

PCD_LOOP
	LD	A, (IY)
	INC	IY		; does not affect condition bits
	CP	'$'		; end of table
	JP	Z, PCD_LOOP_END

	LD	C, (IY)		; get word to compare to
	INC	IY
	LD	B, (IY)
	INC	IY

	CP	'0'		; special case
	JP	NZ, PCD_NOT_ZERO_DIGIT

	LD	A, D		; printed flag
	CP	0
	JP	Z, PCD_LOOP	; before first digit, do not print zero

	; the number in BC represents the smallest number which HL
	; has to be to not print a zero right now
	AND	A		; clear carry
	PUSH	HL		; do not clobber it
	SBC	HL, BC		; HL -= BC
	POP	HL
	JP	NC, PCD_LOOP	; HL >= BC

	LD	A, '0'		; print zero
	LD	(IX), A
	INC	IX
	JP	PCD_LOOP

PCD_NOT_ZERO_DIGIT
	AND	A		; clear carry
	PUSH	HL		; do not clobber HL
	SBC	HL, BC		; HL -= BC
	POP	HL
	JP	C, PCD_LOOP	; HL < BC

	LD	(IX), A		; print digit
	INC	IX

	AND	A		; clear carry
	SBC	HL, BC		; HL -= BC

	LD	A, 1		; printed flag = 1
	LD	D, A

	JP	PCD_LOOP

PCD_LOOP_END
	POP	DE
	POP	IY

PCD_END
	POP	BC
	POP	AF
	POP	HL
	RET

PCD_TABLE
	DB	'6'
	DW	60000
	DB	'5'
	DW	50000
	DB	'4'
	DW	40000
	DB	'3'
	DW	30000
	DB	'2'
	DW	20000
	DB	'1'
	DW	10000
	DB	'0'
	DW	1000
	DB	'9'
	DW	9000
	DB	'8'
	DW	8000
	DB	'7'
	DW	7000
	DB	'6'
	DW	6000
	DB	'5'
	DW	5000
	DB	'4'
	DW	4000
	DB	'3'
	DW	3000
	DB	'2'
	DW	2000
	DB	'1'
	DW	1000
	DB	'0'
	DW	100
	DB	'9'
	DW	900
	DB	'8'
	DW	800
	DB	'7'
	DW	700
	DB	'6'
	DW	600
	DB	'5'
	DW	500
	DB	'4'
	DW	400
	DB	'3'
	DW	300
	DB	'2'
	DW	200
	DB	'1'
	DW	100
	DB	'0'
	DW	10
	DB	'9'
	DW	90
	DB	'8'
	DW	80
	DB	'7'
	DW	70
	DB	'6'
	DW	60
	DB	'5'
	DW	50
	DB	'4'
	DW	40
	DB	'3'
	DW	30
	DB	'2'
	DW	20
	DB	'1'
	DW	10
	DB	'0'
	DW	1
	DB	'9'
	DW	9
	DB	'8'
	DW	8
	DB	'7'
	DW	7
	DB	'6'
	DW	6
	DB	'5'
	DW	5
	DB	'4'
	DW	4
	DB	'3'
	DW	3
	DB	'2'
	DW	2
	DB	'1'
	DW	1
	DB	'$'

;
; PRINTF_CONVERT_HEX
;
;   Input:
;       IY = pointer to int
;       IX = buffer to convert into
;   Output:
;       IY = incremented past int
;       IX = incremented past hex version of int
;   Internally:
;       HL = int
;

PRINTF_CONVERT_HEX
	PUSH	HL

	LD	L, (IY + 0)
	LD	H, (IY + 1)
	INC	IY
	INC	IY

	CALL	PHEXWORD

	POP	HL
	RET

;
; PRINTF_CONVERT_STRING
;
;   Input:
;       IY = pointer to string pointer
;       IX = buffer to convert into
;   Output:
;       IY = incremented past pointer
;       IX = incremented past string
;

PRINTF_CONVERT_STRING
	PUSH	HL

	LD	L, (IY + 0)
	LD	H, (IY + 1)
	INC	IY
	INC	IY

	CALL	STRCPY	; increments IX for us

	POP	HL
	RET
