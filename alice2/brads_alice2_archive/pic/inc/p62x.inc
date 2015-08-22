;****************************************************************************
; PIC16C62X Family Header
;       Copyright (c) 1995, microEngineering Labs
;****************************************************************************

					; *** Default Device Specifications
		DEVICE  3FFF3FFFh       ; All Fuse Bits On
		DEVICE  50010002h       ; 14-Bit, 512 Code, 0 EE, ID = ?

					; *** DEVICE Device Definitions
PIC16C620       equ     50010002h       ; 14-Bit, 512 Code, 0 EE, ID = ?
PIC16C621       equ     50020002h       ; 14-Bit,  1K Code, 0 EE, ID = ?
PIC16C622       equ     50040002h       ; 14-Bit,  2K Code, 0 EE, ID = ?

					; *** DEVICE Fuses Definitions
RC_OSC          equ     3FFC0003h       ; XX XXXX XXXX XX11
HS_OSC          equ     3FFC0002h       ; XX XXXX XXXX XX10
XT_OSC          equ     3FFC0001h       ; XX XXXX XXXX XX01
LP_OSC          equ     3FFC0000h       ; XX XXXX XXXX XX00
WDT_ON          equ     3FFB0004h       ; XX XXXX XXXX X1XX
WDT_OFF         equ     3FFB0000h       ; XX XXXX XXXX X0XX
PWRT_ON         equ     3FF70008h       ; XX XXXX XXXX 1XXX
PWRT_OFF        equ     3FF70000h       ; XX XXXX XXXX 0XXX
BOD_ON          equ     3FBF0040h       ; XX XXXX X1XX XXXX
BOD_OFF         equ     3FBF0000h       ; XX XXXX X0XX XXXX
PROTECT_OFF     equ     00CF3F30h       ; 11 1111 XX11 XXXX
PROTECT_ON      equ     00CF0000h       ; 00 0000 XX00 XXXX

					; *** Register Names
INDIRECT        equ     00h             ; Indirect Data
INDF            equ     00h
TMR0            equ     01h             ; Real Time Clock/Counter
PCL             equ     02h             ; Program Counter (LSB)
STATUS          equ     03h             ; Status Flags
FSR             equ     04h             ; File Select Register
PORTA           equ     05h             ; Port A Data
RA              equ     05h
PORTB           equ     06h             ; Port B Data
RB              equ     06h
PCLATH          equ     0Ah             ; Program Counter (MSB)
INTCON          equ     0Bh             ; Interrupt Control
PIR1            equ     0Ch             : Peripheral Interrupt Flags 1
CMCON           equ     1Fh             ; Comparator Control
OPTION          equ     81h             ; OPTION Register
TRISA           equ     85h             ; Port A Tristate Control
TRISB           equ     86h             ; Port B Tristate Control
PIE1            equ     8Ch             ; Peripheral Interrupt Enables 1
PCON            equ     8Eh             ; Power Control
VRCON           equ     8Fh             ; Voltage Reference Control

					; *** STATUS Bits
C               equ     STATUS.0        ; Carry
DC              equ     STATUS.1        ; Digit (Half) Carry
Z               equ     STATUS.2        ; Zero
PD              equ     STATUS.3        ; Power Down
TO              equ     STATUS.4        ; Time Out
RP0             equ     STATUS.5        ; Direct Data Page Select 0
RP1             equ     STATUS.6        ; Direct Data Page Select 1
IRP             equ     STATUS.7        ; Indirect Data Page Select

					; *** INTCON Bits
RBIF            equ     INTCON.0        ; PORTB[4..7] Change Interrupt Flag
INTF            equ     INTCON.1        ; RB0/INT Interrupt Flag
RTIF            equ     INTCON.2        ; RTCC Overflow Interrupt Flag
RBIE            equ     INTCON.3        ; PORTB[4..7] Change Interrupt Enable
INTE            equ     INTCON.4        ; RB0/INT Interrupt Enable
RTIE            equ     INTCON.5        ; RTCC Overflow Interrupt Enable
PEIE            equ     INTCON.6        ; Peripheral Interrupt Enable
GIE             equ     INTCON.7        ; Global Interrupt Enable

					; *** PIR1 Bits
CMIF            equ     PIR1.6          ; Comparator Interrupt Flag

					; *** CMCON Bits
CM0             equ     CMCON.0         ; Comparator Mode (2 Bits)
CM1             equ     CMCON.1
CM2             equ     CMCON.2
CIS             equ     CMCON.3         ; Comparator Input Select
C1OUT           equ     CMCON.6         ; Comparator Output 1
C2OUT           equ     CMCON.7         ; Comparator Output 2

					; *** OPTION Bits
PS0             equ     OPTION.0        ; Prescaler Divisor Select (3 Bits)
PS1             equ     OPTION.1
PS2             equ     OPTION.2
PSA             equ     OPTION.3        ; Prescaler Assignment
RTE             equ     OPTION.4        ; RTCC Edge Select
RTS             equ     OPTION.5        ; RTCC Signal Select
INTEDG          equ     OPTION.6        ; RB0/INT Edge Select
RBPU            equ     OPTION.7        ; Port B Weak Pull-Up Enable

					; *** PIE1 Bits
CMIE            equ     PIE1.6          ; Comparator Interrupt Enable

					; *** PCON Bits
BO              equ     PCON.0          ; Brown-Out Flag
POR             equ     PCON.1          ; Power-On-Reset Flag

					; *** VRCON Bits
VR0             equ     VRCON.0         ; VRef Select (4 Bits)
VR1             equ     VRCON.1
VR2             equ     VRCON.2
VR3             equ     VRCON.3
VRR             equ     VRCON.5         ; VRef Range Select
VROE            equ     VRCON.6         ; VRef Output Enable
VREN            equ     VRCON.7         ; VRef Enable
