; Defined if we are running slow:
#undef SLOWCLOCK

; Defined to turn off printing:
#undef NOPRINT

; library.asm must be first!
#include "library.asm"

#include "stdio.asm"
#include "string.asm"

PIC_COMMAND	EQU	08000H
NUMKEY		EQU	08001H ; number of keys in keyboard buffer.
PICBYTE		EQU	08003H
SHIFT_STATUS	EQU	08004H
CTRL_STATUS	EQU	08005H
ALT_STATUS	EQU	08006H
UPKEY_FLAG	EQU	08007H
CURRENT_CMD	EQU	08008H
CURRENT_ADDR	EQU	08009H ; two bytes
CURRENT_LEN	EQU	0800BH
CURRENT_PTR	EQU	0800CH ; point into CURRENT_CMD
SHOULD_RUN	EQU	0800DH ; whether to jump to addr from serial.
TIMER_COUNTER   EQU     0800EH ; incremented each time we get a timer.
KEYBUF		EQU	08100H ; keyboard buffer.
INPUT_BUF	EQU	08200H
OUTPUT_BUF	EQU	08300H
GFX_BUF     	EQU	08400H ; graphics variables

#include "graphics.asm"

LSHIFT_KEY	EQU	012H
RSHIFT_KEY	EQU	059H
CTRL_KEY	EQU	014H
ALT_KEY		EQU	011H
UP_KEY		EQU	0F0H
EXT_KEY		EQU	0E0H
EXT2_KEY	EQU	0E1H

PIC_NON_CMD	EQU	000H    ; no command is pending
PIC_SER_CMD	EQU	010H    ; one serial byte follows
PIC_KBD_CMD	EQU	020H    ; one keyboard byte follows
PIC_TIM_CMD	EQU	030H    ; timer interrupt
PIC_SPI_CMD     EQU     040H    ; SPI bytes follow

PIC_SER_SEND    EQU     010H    ; send bytes to serial, lower bits are count, bytes follow
PIC_KBD_SEND    EQU     020H    ; placeholder for send-to-keyboard
PIC_CFG_SEND    EQU     030H    ; placeholder for send PIC configuration
PIC_SPI_WRITE   EQU     040H    ; write bytes to SPI, lower bits and next byte are count, bytes follow
PIC_SPI_READ    EQU     050H    ; read bytes from SPI, lower bits and next byte are count
PIC_SPI_CS      EQU     060H    ; lower 4 bits are the chip-select mask

CMD_SEND	EQU	001H
CMD_RUN		EQU	002H
CMD_MAX		EQU	003H	; one past max value of a command


;----------------------------------------

;
; Main ROM routine.
;

MAIN
        CALL    GFX_INIT
        CALL    GFX_TESTPAT
        ; CALL    GFX_CLEAR

        LD      A, 4
        CALL    GFX_GRAYBAR
        HALT

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
	LD	(TIMER_COUNTER), A

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
        LD      A, 5
        CALL    GFX_GRAYBAR

        LD      A, 10
        CALL    WAITSECS

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

        LD      A, 6
        CALL    GFX_GRAYBAR

	IN	A, (PIC)
        ADD     A, 7
        CALL    GFX_GRAYBAR

	LD	A, 020H		; End of Interrupt
	OUT	(I8259A), A

	POP	AF
	EI
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

BOOTSTR  	DB	"Alice II, v1.0", 0
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

	ORG	09000H

; #include "zcc_out.asm"
; #include "libc.asm"

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

	END
