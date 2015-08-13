;
; Commonly used assembly routines
;

;
; Routines temporarily accept IX as the return address, while we do not
; have RAM available to stack return addresses.  This means if you
; want to call a routine and you have *already* been called, you need to
; save the return address, e.g. in IY.
;
; IX is an implicit parameter to all these functions, although not listed
; as an input.
;

;
; LCD Constants
;
LCDINST	EQU	002H		; LCD instruction I/O address
LCDDATA	EQU	003H		; LCD data I/O address
LCLEAR	EQU	001H		; LCD clear instruction
LRESET	EQU	038H		; LCD reset to normal instruction
LHALF1	EQU	080H		; LCD move cursor to char 1
LHALF2	EQU	0c0H		; LCD move cursor to char 9
NEWLINE	EQU	0aH		; a UNIX newline

	JP MAIN			; get past library routines

;
; LCD Init routine
; In: nothing
; Destroys: HL, B, C
;
LCDINIT
	LD	HL, LISTRING
	LD	B, LISTRLEN	; falls through into byte output loop

;
; LCD string bytes output routine
; In:
;     B    - length divided by two, (e.g. # of bytes to output, not
; 	    including the I/O addresses)
;     HL   - address of bytes
; Destroys: HL, B, C
;
OUTBYTES
	LD	C, (HL)
	INC	HL
	OUTI
	JR	NZ, OUTBYTES

	; Probably need a delay here at 1 MHz.
	; Are we supposed to check busy bit and wait until not busy?

	JP	(IX)		; i.e. return.

;
; Clear LCD display
; In: nothing
; Destroys: HL, B, C
;
LCDCLR
	LD	HL, LCSTRING
	LD	B, LCSTRLEN	; falls through into byte output loop
	JP	OUTBYTES

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
; Destroys: HL, A, B, C, D
;
LCDPRINT
	LD	A, LCLEAR
	OUT	(LCDINST), A		; do a clear

	LD	B, 250		; 250 cycles of wait
LCDPCLOOP
	DJNZ	LCDPCLOOP

	LD	C, LCDDATA
	LD	D, 9		; when 1->0, switch to 2nd display

LCDPLOOP
	DEC	D
	JP	NZ, LCDNOSWITCH

	LD	A, LHALF2	; set LCD to address 0xc0
	OUT	(LCDINST), A

	LD	B, 20		; 20 cycles of wait for write to LCD
LCDP2LOOP
	DJNZ	LCDP2LOOP	; fall through

LCDNOSWITCH

	LD	A, (HL)
	CP	0
	JP	Z, LCDPDONE

	CP	NEWLINE
	JP	NZ, LCDPDOIT

LCDPNL

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

	LD	A, LCLEAR
	OUT	(LCDINST), A	; do a clear

	LD	B, 250		; 250 cycles of wait
LCDPNLOOP
	DJNZ	LCDPNLOOP

	LD	A, LHALF1
	OUT	(LCDINST), A	; go to address 0x80

	LD	B, 20		; 20 cycles of wait for write
LCDP1LOOP
	DJNZ	LCDP1LOOP

	LD	D, 9		; reset half counter
	JP	LCDPNEXT	; continue with next character

LCDPDOIT
	OUT	(LCDDATA), A		; output that guy
	
	LD	B, 20		; 20 cycles of wait for write
LCDPWLOOP
	DJNZ	LCDPWLOOP

LCDPNEXT
	INC	HL		; next char
	JP	LCDPLOOP

	;
	; handle some kind of clear and delay characters.
	; how to handle more than 16 characters?
	;   I bet shift will not work.  How about just delaying like \n?
	;   for now, just run off end.
	; check busy bit and wait until not busy?
	;

LCDPDONE
	JP	(IX)		; i.e. return


;
; Wait for seconds routine
; In:	
;     A    - number of seconds to wait
; Destroys: A, B, C
;
WAITSECS
	RLCA		; multiply D by 2

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

	JP	(IX)		; i.e. return
