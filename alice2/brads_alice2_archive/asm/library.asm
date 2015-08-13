;
; Commonly used assembly routines
;

; 8259 Constants
I8259A	EQU	000H		; 8259 port 1
I8259B	EQU	001H		; 8259 port 2

;
; LCD Constants
;
LCDINST	EQU	002H		; LCD instruction I/O port
LCDDATA	EQU	003H		; LCD data I/O port
LCLEAR	EQU	001H		; LCD clear instruction
LRESET	EQU	038H		; LCD reset to normal instruction
LHALF1	EQU	080H		; LCD move cursor to char 1
LHALF2	EQU	0c0H		; LCD move cursor to char 9
PAUSE	EQU	0aH		; a UNIX newline
STACKTOP	EQU	00000H	; top of stack (wraps to 0ffff)

; PIC Constants
PIC	EQU	004H		; PIC I/O port

;
; Set up useful stuff, jump to main program
;
	LD	SP, STACKTOP
	CALL	LCDINIT
	JP 	MAIN		; get past library routines

;
; Catch reset if it happens and try to print it on the LCD
;

	ORG	0038H		; Catch a reset
	LD	HL, GOTRESETSTR
	CALL	LCDPRINT
	HALT

GOTRESETSTR	DB	"Got RST 38H", PAUSE, 0


;
; LCD Init routine
; In: nothing
;
LCDINIT
#ifdef NOPRINT
	RET
#endif

	PUSH	BC
	PUSH	HL

	LD	HL, LISTRING
	LD	B, LISTRLEN

	JP	OUTBLOOP

;
; LCD string bytes output routine
; In:
;     B    - length divided by two, (e.g. # of bytes to output, not
; 	    including the I/O addresses)
;     HL   - address of bytes
;
OUTBYTES
	PUSH	BC
	PUSH	HL

OUTBLOOP
	LD	C, (HL)
	INC	HL
	OUTI
	JR	NZ, OUTBLOOP

	; Probably need a delay here at 1 MHz.
	; Are we supposed to check busy bit and wait until not busy?

	POP	HL
	POP	BC

	RET

;
; Clear LCD display
; In: nothing
;
LCDCLR
	PUSH	BC
	PUSH	HL

	LD	HL, LCSTRING
	LD	B, LCSTRLEN	; falls through into byte output loop
	JP	OUTBLOOP

;
; LCD Initialization string
;
LISTRLEN	EQU	10
LISTRING
	DB	LCDINST, LRESET
	DB	LCDINST, LRESET
	DB	LCDINST, 006H
	DB	LCDINST, 00eH
	DB	LCDINST, LCLEAR
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1

LCSTRLEN	EQU	5
LCSTRING
	DB	LCDINST, LCLEAR
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1
	DB	LCDINST, LHALF1

;
; LCD String output routine
; In:
;     HL   - Address of null-terminated string
;
LCDPRINT
#ifdef NOPRINT
	RET
#endif

	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL

	LD	A, LCLEAR
	OUT	(LCDINST), A		; do a clear

#ifndef SLOWCLOCK
	LD	B, 250		; 250 cycles of wait
LCDPCLOOP
	DJNZ	LCDPCLOOP
#endif /* SLOWCLOCK */

	LD	C, LCDDATA
	LD	D, 25		; when 1->0, switch to 2nd display

LCDPLOOP
	DEC	D
	JP	NZ, LCDNOSWITCH

	LD	A, LHALF2	; set LCD to address 0xc0
	OUT	(LCDINST), A

#ifndef SLOWCLOCK
	LD	B, 20		; 20 cycles of wait for write to LCD
LCDP2LOOP
	DJNZ	LCDP2LOOP	; fall through
#endif /* SLOWCLOCK */

LCDNOSWITCH

	LD	A, (HL)
	CP	0
	JP	Z, LCDPDONE

	CP	PAUSE
	JP	NZ, LCDPDOIT

;	LD	A, 0dh		; return
;	OUT	(PIC), A

#ifndef SLOWCLOCK
	; wait for two seconds here!
	LD	D, 2		; 2 * 256 * 256 cycles is close to 1 sec
LCDP1SL1
	LD	A, 0		; 256 * 256 cycles
LCDP1SL2
	LD	B, 0		; 256 cycles
LCDP1SL3
	DJNZ	LCDP1SL3

	DEC	A
	JP	NZ, LCDP1SL2

	DEC	D
	JP	NZ, LCDP1SL1
#endif /* SLOWCLOCK */

	LD	A, LCLEAR
	OUT	(LCDINST), A	; do a clear

;	LD	A, 0ah		; linefeed
;	OUT	(PIC), A

#ifndef SLOWCLOCK
	LD	B, 250		; 250 cycles of wait
LCDPNLOOP
	DJNZ	LCDPNLOOP
#endif /* SLOWCLOCK */

	LD	A, LHALF1
	OUT	(LCDINST), A	; go to address 0x80

#ifndef SLOWCLOCK
	LD	B, 20		; 20 cycles of wait for write
LCDP1LOOP
	DJNZ	LCDP1LOOP
#endif /* SLOWCLOCK */

	LD	D, 9		; reset half counter
	JP	LCDPNEXT	; continue with next character

LCDPDOIT
	OUT	(LCDDATA), A	; output that guy
;	OUT	(PIC), A

#ifndef SLOWCLOCK
	LD	B, 20		; 20 cycles of wait for write
LCDPWLOOP
	DJNZ	LCDPWLOOP
#endif /* SLOWCLOCK */

LCDPNEXT
	INC	HL		; next char
	JP	LCDPLOOP

LCDPDONE

	POP	HL
	POP	DE
	POP	BC
	POP	AF

	RET


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
