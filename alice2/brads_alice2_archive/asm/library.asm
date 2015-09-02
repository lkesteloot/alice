;
; Commonly used assembly routines
;

; 8259 Constants
I8259A	EQU	000H		; 8259 port 1
I8259B	EQU	001H		; 8259 port 2

PAUSE	EQU	0aH		; a UNIX newline
STACKTOP	EQU	00000H	; top of stack (wraps to 0ffff)

; PIC Constants
PIC	EQU	004H		; PIC I/O port

;
; Set up useful stuff, jump to main program
;
	LD	SP, STACKTOP
	JP 	MAIN		; get past library routines

;
; Catch reset if it happens and try to print it on the LCD
;

	ORG	0038H		; Catch a reset
	HALT

;
; Wait for seconds routine
; In:	
;     A    - number of seconds to wait
;
WAITSECS
	PUSH	AF
	PUSH	BC
	RLCA		; multiply A by 2

LCDP1SLP1
	LD	C, 0		; 256 * 256 cycles
LCDP1SLP2
	LD	B, 0		; 256 cycles
LCDP1SLP3
	DJNZ	LCDP1SLP3

	DEC	C
	JP	NZ, LCDP1SLP2

	DEC	A
	JP	NZ, LCDP1SLP1

	POP	BC
	POP	AF

	RET


;
; put the hex string representation of A into the location at IX
; IX becomes IX + 2
;
PHEXBYTE
	PUSH	AF
	PUSH	BC

	LD	B, A
	AND	0f0H
	SRL	A
	SRL	A
	SRL	A
	SRL	A
	CP	0aH
	JP	C, DIG1LESS
	ADD	A, "a" - 10
	JP	DIG1DONE
DIG1LESS
	ADD	A, "0"
DIG1DONE

	LD	(IX), A
	INC	IX
	LD	A, B
	AND	0fH
	CP	0aH
	JP	C, DIG2LESS
	ADD	A, "a" - 10
	JP	DIG2DONE
DIG2LESS
	ADD	A, "0"
DIG2DONE
	LD	(IX), A
	INC	IX

	POP	BC
	POP	AF
	RET

;
; put the hex string representation of HL into the location at IX
; IX becomes IX + 4
;
PHEXWORD

	PUSH	AF
	LD	A, H
	CALL	PHEXBYTE
	LD	A, L
	CALL	PHEXBYTE
	POP	AF
	RET
