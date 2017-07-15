;****************************************************************************
; PIC16C71(A) Header
;       Copyright (c) 1995, microEngineering Labs
;****************************************************************************

					; *** Default Device Specifications
		DEVICE  3FFF3FFFh       ; All Fuse Bits On
		DEVICE  50020000h       ; 14-Bit, 1K Code, 0 EE, ID = 0

					; *** DEVICE Device Definitions
PIC16C71        equ     50020000h       ; 14-Bit, 1K Code, 0 EE, ID = 0

					; *** DEVICE Fuses Definitions
RC_OSC          equ     3FFC0003h       ; XX XXXX XXXX XX11
HS_OSC          equ     3FFC0002h       ; XX XXXX XXXX XX10
XT_OSC          equ     3FFC0001h       ; XX XXXX XXXX XX01
LP_OSC          equ     3FFC0000h       ; XX XXXX XXXX XX00
WDT_ON          equ     3FFB0004h       ; XX XXXX XXXX X1XX
WDT_OFF         equ     3FFB0000h       ; XX XXXX XXXX X0XX
PWRT_ON         equ     3FF70008h       ; XX XXXX XXXX 1XXX
PWRT_OFF        equ     3FF70000h       ; XX XXXX XXXX 0XXX
PROTECT_OFF     equ     3FEF0010h       ; XX XXXX XXX1 XXXX
PROTECT_ON      equ     3FEF0000h       ; XX XXXX XXX0 XXXX

					; *** Register Names
INDIRECT        equ     00h             ; Indirect Data
INDF            equ     00h             ; Indirect Data
RTCC            equ     01h             ; Real Time Clock/Counter
TMR0            equ     01h             ; Real Time Clock/Counter
PCL             equ     02h             ; Program Counter (LSB)
STATUS          equ     03h             ; Status Flags
FSR             equ     04h             ; File Select Register
PORTA           equ     05h             ; Port A Data
RA              equ     05h
PORTB           equ     06h             ; Port B Data
RB              equ     06h
ADCON0          equ     08h             ; A/D Control 0
ADRES           equ     09h             ; A/D Result
PCLATH          equ     0Ah             ; Program Counter (MSB)
INTCON          equ     0Bh             ; Interrupt Control
OPTION          equ     81h             ; OPTION Register
TRISA           equ     85h             ; Port A Tristate Control
TRISB           equ     86h             ; Port B Tristate Control
ADCON1          equ     88h             ; A/D Control 1

					; *** STATUS Bits
C               equ     STATUS.0        ; Carry
DC              equ     STATUS.1        ; Digit (Half) Carry
Z               equ     STATUS.2        ; Zero
PD              equ     STATUS.3        ; Power Down
TO              equ     STATUS.4        ; Time Out
RP0             equ     STATUS.5        ; Direct Data Page Select 0
RP1             equ     STATUS.6        ; Direct Data Page Select 1
IRP             equ     STATUS.7        ; Indirect Data Page Select

					; *** ADCON0 Bits
ADON            equ     ADCON0.0        ; A/D Power Control
ADIF            equ     ADCON0.1        ; A/D Interrupt Flag
GO_DONE         equ     ADCON0.2        ; A/D GO/DONE Bit
CHS0            equ     ADCON0.3        ; A/D Channel Select (2 Bits)
CHS1            equ     ADCON0.4
ADCS0           equ     ADCON0.6        ; A/D Clock Select (2 Bits)
ADCS1           equ     ADCON0.7

					; *** INTCON Bits
RBIF            equ     INTCON.0        ; PORTB[4..7] Change Interrupt Flag
INTF            equ     INTCON.1        ; RB0/INT Interrupt Flag
RTIF            equ     INTCON.2        ; RTCC Overflow Interrupt Flag
RBIE            equ     INTCON.3        ; PORTB[4..7] Change Interrupt Enable
INTE            equ     INTCON.4        ; RB0/INT Interrupt Enable
RTIE            equ     INTCON.5        ; RTCC Overflow Interrupt Enable
ADIE            equ     INTCON.6        ; A/D Interrupt Enable
GIE             equ     INTCON.7        ; Global Interrupt Enable

					; *** OPTION Bits
PS0             equ     OPTION.0        ; Prescaler Divisor Select (3 Bits)
PS1             equ     OPTION.1
PS2             equ     OPTION.2
PSA             equ     OPTION.3        ; Prescaler Assignment
T0SE            equ     OPTION.4        ; Timer0 Source Edge Select
T0CS            equ     OPTION.5        ; Timer0 Clock Source Select
INTEDG          equ     OPTION.6        ; RB0/INT Edge Select
RBPU            equ     OPTION.7        ; Port B Weak Pull-Up Enable

					; *** PCON Bits
BOR             equ     PCON.0          ; Brown-Out Reset Flag
POR             equ     PCON.1          ; Power-On Reset Flag

					; *** ADCON1 Bits
PCFG0           equ     ADCON1.0        ; Port A Configuration (2 Bits)
PCFG1           equ     ADCON1.1
