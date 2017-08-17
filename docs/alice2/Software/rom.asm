#undef SLOWCLOCK   ; Defined if we are running slow
#undef NOPRINT     ; Defined to turn off printing

; library.asm must be first!
#include "library.asm"

#include "stdio.asm"
#include "string.asm"

PIC_COMMAND	EQU	08000H
NUMKEY		EQU	08001H
PICBYTE		EQU	08003H
SHIFT_STATUS	EQU	08004H
CTRL_STATUS	EQU	08005H
ALT_STATUS	EQU	08006H
UPKEY_FLAG	EQU	08007H
CURRENT_CMD	EQU	08008H
CURRENT_ADDR	EQU	08009H ; two bytes
CURRENT_LEN	EQU	0800BH
CURRENT_PTR	EQU	0800CH ; point into CURRENT_CMD
SHOULD_RUN	EQU	0800DH
KEYBUF		EQU	0A000H
INPUT_BUF	EQU	0C000H
OUTPUT_BUF	EQU	0C040H

LSHIFT_KEY	EQU	012H
RSHIFT_KEY	EQU	059H
CTRL_KEY	EQU	014H
ALT_KEY		EQU	011H
UP_KEY		EQU	0F0H
EXT_KEY		EQU	0E0H
EXT2_KEY	EQU	0E1H

PIC_NON_CMD	EQU	000H
PIC_SER_CMD	EQU	001H
PIC_KBD_CMD	EQU	002H
PIC_TIM_CMD	EQU	003H

CMD_SEND	EQU	001H
CMD_RUN		EQU	002H
CMD_MAX		EQU	003H	; one past max value of a command


;----------------------------------------

;
; Memory test program
;

MAIN
	LD	HL, INTROSTR
	CALL 	LCDPRINT

;
; Setup the 8259
;

	LD	A, 016H    ; F = 1, S = 1
	OUT	(I8259A), A

	LD	A, 03FH	   ; Table is at 03F00H
	OUT	(I8259B), A

	LD	A, 0F0H    ; Enable lower 4 IRQs
	OUT	(I8259B), A

;
; Initialize our variables
;

	LD	A, 0
	LD	(PIC_COMMAND), A
	LD	(NUMKEY), A
	LD	(SHIFT_STATUS), A
	LD	(CTRL_STATUS), A
	LD	(ALT_STATUS), A
	LD	(UPKEY_FLAG), A
	LD	(CURRENT_PTR), A
	LD	(SHOULD_RUN), A

;
; Read a byte from the pic in case it triggered
; an interrupt before we were ready (keyboard
; reset).  This resets the PIC interrupt line.
;
; The command read routine throws away commands it does not understand,
; so it is likely we will get the keyboard init status byte here but the
; command read routine will chuck it and we will catch up on the next byte.
;

	IN	A, (PIC)

;
; Setup interrupt mode 0
;

	IM	0
	EI

;
; Go, go, go!
;

PRINTAGAIN
	LD	HL, PLEASESTR
	CALL	LCDPRINT

	LD	HL, INPUT_BUF
	CALL	GETS

	LD	HL, HELLOSTR
	LD	IX, OUTPUT_BUF
	CALL	STRCPY

	LD	HL, INPUT_BUF
	CALL	STRCPY

	LD	(IX), PAUSE
	INC	IX
	LD	(IX), 0

	LD	HL, OUTPUT_BUF
	CALL	LCDPRINT

	JP	PRINTAGAIN

;----------------------------------------

ISR0	; PIC interrupt

	;
	; The PIC sends a command byte followed by an
	; optional data byte.
	;
	;   Command byte: PIC_SER_CMD
	;   Data byte: serial byte in ASCII
	;
	;   Command byte: PIC_KBD_CMD
	;   Data byte: keyboard byte as scan code
	;
	;   Command byte: PIC_TIM_CMD
	;   Data byte: none
	;
	; Current command is stored in PIC_COMMAND, which is
	; equal to PIC_NON_CMD if we are waiting for a command.
	;

	PUSH	AF

	IN	A, (PIC)
	LD	(PICBYTE), A

; START TEST
#if 0
	PUSH	IX
	PUSH	HL
	PUSH	BC

	LD	IX, OUTPUT_BUF
	CALL	PHEXBYTE	; Hex A into IX

	LD	HL, KEYBUF
	LD	A, (NUMKEY)
	LD	C, A
	LD	B, 0
	ADD	HL, BC		; HL = pointer into buffer
	INC	A
	LD	(NUMKEY), A

	LD	A, (OUTPUT_BUF)	; Key in A
	LD	(HL), A		; Stick our letter in buffer

	LD	HL, KEYBUF
	LD	A, (NUMKEY)
	LD	C, A
	LD	B, 0
	ADD	HL, BC		; HL = pointer into buffer
	INC	A
	LD	(NUMKEY), A

	LD	A, (OUTPUT_BUF+1); Key in A
	LD	(HL), A		; Stick our letter in buffer

	POP	BC
	POP	HL
	POP	IX

	JP	ISR0_END
#endif
; END TEST

	LD	A, (PIC_COMMAND)
	CP	PIC_NON_CMD	; Cmd not read yet
	JP	Z, ISR0_IS_COMMAND

	CP	PIC_SER_CMD
	JP	Z, ISR0_IS_SERIAL

	CP	PIC_KBD_CMD
	JP	Z, ISR0_IS_KEYBOARD

	JP	ISR0_RESET	; Whoa, bad byte

ISR0_IS_COMMAND
	LD	A, (PICBYTE)
	CP	PIC_TIM_CMD	; special case for timer cause no data
	JP	Z, ISR0_IS_TIMER

	LD	(PIC_COMMAND), A
	JP	ISR0_END

ISR0_IS_TIMER
	; do something
	JP	ISR0_RESET

ISR0_IS_SERIAL
	CALL	HANDLE_SERIAL
	JP	ISR0_RESET

ISR0_IS_KEYBOARD
	CALL	HANDLE_KEYBOARD
	JP	ISR0_RESET

ISR0_RESET
	LD	A, PIC_NON_CMD
	LD	(PIC_COMMAND), A
	; FALLTHROUGH

ISR0_END
	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

HANDLE_SERIAL  ; trashes A
	PUSH	HL
	PUSH	BC

	; we get downloaded programs through serial.  handle
	; that first.  we get one command byte, then two
	; address bytes, then a length byte.  if the command
	; is CMD_SEND, then this header is followed by "length"
	; bytes to stick into "address".  if the command is
	; CMD_RUN, then the length is zero and SHOULD_RUN is
	; set to 1, which causes the input routine to jump
	; to the address in CURRENT_ADDR.

	LD	A, (CURRENT_PTR)
	CP	3
	JP	NZ, GET_CMD_HEADER

	; just got last byte of header
	LD	A, (CURRENT_CMD)
	CP	CMD_RUN
	JP	NZ, GET_CMD_HEADER

	; ignore length and run
	LD	A, 1
	LD	(SHOULD_RUN), A
	JP	END_HANDLE_SERIAL

GET_CMD_HEADER
	LD	A, (CURRENT_PTR)
	CP	4
	JP	Z, TRANSFER_BYTE

	; check if it is not a transfer
	CP	0
	JP	NZ, TRANSFER_HEADER

	LD	A, (PICBYTE)
	CP	CMD_MAX
	JP	P, SIMULATE_KEYBOARD ; >= CMD_MAX

TRANSFER_HEADER
	; transfer into the header
	LD	HL, CURRENT_CMD
	LD	A, (CURRENT_PTR)
	LD	C, A
	LD	B, 0
	ADD	HL, BC
	LD	A, (PICBYTE)
	LD	(HL), A
	INC	C
	LD	A, C
	LD	(CURRENT_PTR), A

	JP	END_HANDLE_SERIAL

TRANSFER_BYTE
	; transfer into RAM
	LD	HL, (CURRENT_ADDR)
	LD	A, (PICBYTE)
	LD	(HL), A
	INC	HL
	LD	(CURRENT_ADDR), HL
	LD	A, (CURRENT_LEN)
	DEC	A
	LD	(CURRENT_LEN), A
	JP	NZ, END_HANDLE_SERIAL

	LD	(CURRENT_PTR), A

	JP	END_HANDLE_SERIAL

SIMULATE_KEYBOARD
	LD	HL, KEYBUF
	LD	A, (NUMKEY)
	LD	C, A
	LD	B, 0
	ADD	HL, BC		; HL = pointer into buffer
	INC	A
	LD	(NUMKEY), A

	LD	A, (PICBYTE)	; Key in A
	LD	(HL), A		; Stick our letter in buffer

END_HANDLE_SERIAL

	POP	BC
	POP	HL
	RET

;------------------

HANDLE_KEYBOARD
	PUSH	HL
	PUSH	BC

	LD	A, (PICBYTE)	; Get the byte

	CP	UP_KEY		; Key-up is preceded by UP_KEY
	JP	NZ, NOT_UP_KEY

	LD	A, 1
	LD	(UPKEY_FLAG), A
	JP	END_HANDLE_KEYBOARD

NOT_UP_KEY
	CP	EXT_KEY		; Extended keys preceded by EXT_KEY
	JP	Z, DID_KEY	; For now just ignore

	CP	EXT2_KEY	; Break preceded by EXT2_KEY
	JP	Z, DID_KEY	; For now just ignore

	BIT	7, A		; See if high bit is on
	JP	NZ, DID_KEY	; Ignore if >= 128

	CP	LSHIFT_KEY	; See if it is a shift key
	JP	Z, IS_SHIFT
	CP	RSHIFT_KEY
	JP	NZ, NOT_SHIFT
IS_SHIFT
	LD	A, (UPKEY_FLAG)
	XOR	1
	LD	(SHIFT_STATUS), A
	JP	DID_KEY

NOT_SHIFT
	CP	CTRL_KEY	; See if it is a control key
	JP	NZ, NOT_CTRL

	LD	A, (UPKEY_FLAG)
	XOR	1
	LD	(CTRL_STATUS), A
	JP	DID_KEY

NOT_CTRL
	CP	ALT_KEY		; See if it is an alt key
	JP	NZ, NOT_ALT

	LD	A, (UPKEY_FLAG)
	XOR	1
	LD	(ALT_STATUS), A
	JP	DID_KEY

NOT_ALT
	LD	L, A		; HL = A
	LD	H, 0

	LD	A, (UPKEY_FLAG) ; Ignore all other key ups
	CP	1
	JP	Z, DID_KEY

	ADD	HL, HL		; HL *= 4
	ADD	HL, HL

	LD	B, 0
	LD	A, (SHIFT_STATUS)
	LD	C, A
	ADD	HL, BC

	LD	A, (CTRL_STATUS)
	LD	C, A
	ADD	HL, BC
	ADD	HL, BC

	LD	A, (ALT_STATUS)
	LD	C, A
	ADD	HL, BC
	ADD	HL, BC
	ADD	HL, BC

	LD	BC, KEY_XLAT	; Translation table
	ADD	HL, BC

	LD	A, (HL)		; Get actual key
	PUSH	AF

	LD	HL, KEYBUF
	LD	A, (NUMKEY)
	LD	C, A
	LD	B, 0
	ADD	HL, BC		; HL = pointer into buffer
	INC	A
	LD	(NUMKEY), A

	POP	AF		; Key in A
	LD	(HL), A		; Stick our ASCII letter in there


DID_KEY
	; Reset "up" flag
	LD	A, 0
	LD	(UPKEY_FLAG), A

END_HANDLE_KEYBOARD

	POP	BC
	POP	HL
	RET

;------------------

ISR1
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR2
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR3
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR4
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR5
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR6
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;------------------

ISR7
	PUSH	AF

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
	RET

;----------------------------------------

INTROSTR	DB	"PIC16C64 test", 10, 0

PLEASESTR	DB	"Your name?", 10, 0
HELLOSTR	DB	"Hello ", 0

KEY_XLAT 	; Normal, shift, ctrl, alt
	DB	 '?', '?', '?', '?'
	DB	 '9', '9', '9', '9'  ; F9
	DB	 '?', '?', '?', '?'
	DB	 '5', '5', '5', '5'  ; F5
	DB	 '3', '3', '3', '3'  ; F3
	DB	 '1', '1', '1', '1'  ; F1
	DB	 '2', '2', '2', '2'  ; F2
	DB	 '1', '1', '1', '1'  ; F12
	DB	 '?', '?', '?', '?'
	DB	 '1', '1', '1', '1'  ; F10
	DB	 '8', '8', '8', '8'  ; F8
	DB	 '6', '6', '6', '6'  ; F6
	DB	 '4', '4', '4', '4'  ; F4
	DB	   9,   9,   9,   9  ; Tab
	DB	 '`', '~', '`', '`'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'  ; Alt
	DB	 '?', '?', '?', '?'  ; Left shift
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'  ; Control
	DB	 'q', 'Q',  17,  17
	DB	 '1', '!', '1', '1'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 'z', 'Z',  26,  26
	DB	 's', 'S',  19,  19
	DB	 'a', 'A',   1,   1
	DB	 'w', 'W',  23,  23
	DB	 '2', '@', '2', '2'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 'c', 'C',   3,   3
	DB	 'x', 'X',  24,  24
	DB	 'd', 'D',   4,   4
	DB	 'e', 'E',   5,   5
	DB	 '4', '$', '4', '4'
	DB	 '3', '#', '3', '3'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 ' ', ' ', ' ', ' ' 
	DB	 'v', 'V',  22,  22
	DB	 'f', 'F',   6,   6
	DB	 't', 'T',  20,  20
	DB	 'r', 'R',  18,  18
	DB	 '5', '%', '5', '5'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 'n', 'N',  14,  14
	DB	 'b', 'B',   2,   2
	DB	 'h', 'H',   8,   8
	DB	 'g', 'G',   7,   7
	DB	 'y', 'Y',  25,  25
	DB	 '6', '^', '6', '6'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 'm', 'M',  13,  13
	DB	 'j', 'J',  10,  10
	DB	 'u', 'U',  21,  21
	DB	 '7', '&', '7', '7'
	DB	 '8', '*', '8', '8'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 ',', '<', ',', ','
	DB	 'k', 'K',  11,  11
	DB	 'i', 'I',   9,   9
	DB	 'o', 'O',  15,  15
	DB	 '0', ')', '0', '0'
	DB	 '9', '(', '9', '9'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '.', '>', '.', '.'
	DB	 '/', '?', '/', '/'
	DB	 'l', 'L',  12,  12
	DB	 ';', ':', ';', ';'
	DB	 'p', 'P',  16,  16
	DB	 '-', '_', '-', '-'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	  39, '"',  39,  39  ; Single quote
	DB	 '?', '?', '?', '?'
	DB	 '[', '{', '[', '['
	DB	 '=', '+', '=', '='
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'  ; Caps lock
	DB	 '?', '?', '?', '?'  ; Right shift
	DB	  10,  10,  10,  10  ; Enter
	DB	 ']', '}', ']', ']'
	DB	 '?', '?', '?', '?'
	DB	  92, '|',  92,  92  ; Backslash
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	   8,   8,   8,   8  ; Backspace
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '1', '1', '1', '1'  ; Numeric 1
	DB	 '?', '?', '?', '?'
	DB	 '4', '4', '4', '4'  ; Numeric 4
	DB	 '7', '7', '7', '7'  ; Numeric 7
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '?', '?', '?', '?'
	DB	 '0', '0', '0', '0'  ; Numeric 0
	DB	 '.', '.', '.', '.'  ; Numeric .
	DB	 '2', '2', '2', '2'  ; Numeric 2
	DB	 '5', '5', '5', '5'  ; Numeric 5
	DB	 '6', '6', '6', '6'  ; Numeric 6
	DB	 '8', '8', '8', '8'  ; Numeric 8
	DB	  27,  27,  27,  27  ; Escape
	DB	 '?', '?', '?', '?'  ; Num lock
	DB	 '1', '1', '1', '1'  ; F11
	DB	 '+', '+', '+', '+'  ; Numeric +
	DB	 '3', '3', '3', '3'  ; Numeric 3
	DB	 '-', '-', '-', '-'  ; Numeric -
	DB	 '*', '*', '*', '*'  ; Numeric *
	DB	 '9', '9', '9', '9'  ; Numeric 9
	DB	 '?', '?', '?', '?'  ; Scroll lock
	DB	 '?', '?', '?', '?'

	ORG	03F00H
INTTABLE

	; IRQ0
	JP	ISR0
	DB	0

	; IRQ1
	JP	ISR1
	DB	0

	; IRQ2
	JP	ISR2
	DB	0

	; IRQ3
	JP	ISR3
	DB	0

	; IRQ4
	JP	ISR4
	DB	0

	; IRQ5
	JP	ISR5
	DB	0

	; IRQ6
	JP	ISR6
	DB	0

	; IRQ7
	JP	ISR7
	DB	0

	ORG	04000H

#if 0

; start tmp code
	ORG	06000H  /* J UMP TO HERE */

#if 0
;this code writes 0-255 in 0x4000 through 0x40ff continuously

	DI
	LD	A, 0
	LD	HL, 04000H
TEST_WRITE_AGAIN
	LD	(HL), A
	INC	A
	INC	L
	JP	TEST_WRITE_AGAIN
#endif


	LD	HL, 07000H
	LD	A, 0
TEST_AGAIN
        PUSH	HL
	LD	(HL), A
	LD	A, (HL)
	LD	IX, OUTPUT_BUF
	CALL	PHEXBYTE
	LD	(IX), PAUSE
	INC	IX
	LD	(IX), 0
	LD	HL, OUTPUT_BUF
	CALL	LCDPRINT

#if 0
	LD	HL, 04000H
	LD	(HL), 0FFH
	LD	A, (HL)
	LD	IX, OUTPUT_BUF
	CALL	PHEXBYTE
	LD	(IX), PAUSE
	INC	IX
	LD	(IX), 0
	LD	HL, OUTPUT_BUF
	CALL	LCDPRINT
#endif
	POP	HL
	INC	HL
	INC	A

	JP	TEST_AGAIN

; end tmp code
#endif

#if 1
#include "image.asm"
#endif

#if 1
	ORG	09000H   /* JUMP TO HERE */
#endif

#if 0
/* fill the screen */
	LD	HL, 16384
	LD	A, 088H

LOOP_AGAIN_YES
	LD	(HL), A
	INC	HL
	JP	LOOP_AGAIN_YES
	DJNZ	LOOP_AGAIN_YES
#endif

#if 0
#include "zcc_out.asm"
#include "libc.asm"
#endif

LOOP_FOREVER
	JP	LOOP_FOREVER

	END
