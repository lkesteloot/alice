;****************************************************************************
; PIC16C72 Header
;       Copyright (c) 1995, microEngineering Labs
;****************************************************************************

					; *** Default Device Specifications
		DEVICE  3FFF3FFFh       ; All Fuse Bits On
		DEVICE  50040002h       ; 14-Bit, 2K Code, 0 EE, ID = ?

					; *** DEVICE Device Definitions
PIC16C72        equ     50040002h       ; 14-Bit, 2K Code, 0 EE, ID = ?

					; *** DEVICE Fuses Definitions
RC_OSC          equ     3FFC0003h       ; XX XXXX XXXX XX11
HS_OSC          equ     3FFC0002h       ; XX XXXX XXXX XX10
XT_OSC          equ     3FFC0001h       ; XX XXXX XXXX XX01
LP_OSC          equ     3FFC0000h       ; XX XXXX XXXX XX00
WDT_ON          equ     3FFB0004h       ; XX XXXX XXXX X1XX
WDT_OFF         equ     3FFB0000h       ; XX XXXX XXXX X0XX
PWRT_ON         equ     3FF70008h       ; XX XXXX XXXX 1XXX
PWRT_OFF        equ     3FF70000h       ; XX XXXX XXXX 0XXX
PROTECT_OFF     equ     3FCF0030h       ; XX XXXX XX11 XXXX
PROTECT_ON      equ     3FCF0000h       ; XX XXXX XX00 XXXX

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
PORTC           equ     07h             ; Port C Data
RC              equ     07h
PCLATH          equ     0Ah             ; Program Counter (MSB)
INTCON          equ     0Bh             ; Interrupt Control
PIR1            equ     0Ch             : Peripheral Interrupt Flags 1
TMR1L           equ     0Eh             ; Timer 1 Data (LSB)
TMR1H           equ     0Fh             ; Timer 1 Data (MSB)
T1CON           equ     10h             ; Timer 1 Control
TMR2            equ     11h             ; Timer 2 Data
T2CON           equ     12h             ; Timer 2 Control
SSPBUF          equ     13h             ; SSP Data
SSPCON          equ     14h             ; SSP Control
CCPR1L          equ     15h             ; CCP 1 Data (LSB)
CCPR1H          equ     16h             ; CCP 1 Data (MSB)
CCP1CON         equ     17h             ; CCP 1 Control
ADRES           equ     1Eh             ; A/D Result
ADCON0          equ     1Fh             ; A/D Control 0
OPTION          equ     81h             ; OPTION Register
TRISA           equ     85h             ; Port A Tristate Control
TRISB           equ     86h             ; Port B Tristate Control
TRISC           equ     87h             ; Port C Tristate Control
PIE1            equ     8Ch             ; Peripheral Interrupt Enables 1
PCON            equ     8Eh             ; Power Control
PR2             equ     92h             ; Timer 2 Period
SSPADD          equ     93h             ; SSP I2C Address
SSPSTAT         equ     94h             ; SSP Status Flags
ADCON1          equ     9Fh             ; A/D Control 1

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
TMR1IF          equ     PIR1.0          ; Timer 1 Interrupt Flag
TMR2IF          equ     PIR1.1          ; Timer 2 Interrupt Flag
CCP1IF          equ     PIR1.2          ; CCP 1 Interrupt Flag
SSPIF           equ     PIR1.3          ; SSP Interrupt Flag
ADIF            equ     PIR1.6          ; A/D Interrupt Flag

					; *** T1CON Bits
TMR1ON          equ     T1CON.0         ; Timer 1 Enable
TMR1CS          equ     T1CON.1         ; Timer 1 Clock Select
T1INSYNC        equ     T1CON.2         ; Timer 1 Sync Enable
T1OSCEN         equ     T1CON.3         ; Timer 1 Oscillator Enable
T1CKPS0         equ     T1CON.4         ; Timer 1 Prescaler Select (2 Bits)
T1CKPS1         equ     T1CON.5

					; *** T2CON Bits
T2CKPS0         equ     T2CON.0         ; Timer 2 Prescaler Select (2 Bits)
T2CKPS1         equ     T2CON.1
TMR2ON          equ     T2CON.2         ; Timer 2 Enable
TOUTPS0         equ     T2CON.3         ; Timer 2 Postscaler Select (4 Bits)
TOUTPS1         equ     T2CON.4
TOUTPS2         equ     T2CON.5
TOUTPS3         equ     T2CON.6

					; *** SSPCON Bits
SSPM0           equ     SSPCON.0        ; SSP Mode Select (4 Bits)
SSPM1           equ     SSPCON.1
SSPM2           equ     SSPCON.2
SSPM3           equ     SSPCON.3
CKP             equ     SSPCON.4        ; SSP Clock Polarity Select
SSPEN           equ     SSPCON.5        ; SSP Enable
SSPOV           equ     SSPCON.6        ; SSP Receive Overflow Flag
WCOL            equ     SSPCON.7        ; SSP Write Collision Detect

					; *** CCP1CON Bits
CCP1M0          equ     CCP1CON.0       ; CCP 1 Mode Select (4 Bits)
CCP1M1          equ     CCP1CON.1
CCP1M2          equ     CCP1CON.2
CCP1M3          equ     CCP1CON.3
CCP1Y           equ     CCP1CON.4       ; CCP 1 PWM Bit 0
CCP1X           equ     CCP1CON.5       ; CCP 1 PWM Bit 1

					; *** ADCON0 Bits
ADON            equ     ADCON0.0        ; A/D Power Control
GO_DONE         equ     ADCON0.2        ; A/D GO/DONE Bit
CHS0            equ     ADCON0.3        ; A/D Channel Select (3 Bits)
CHS1            equ     ADCON0.4
CHS2            equ     ADCON0.5
ADCS0           equ     ADCON0.6        ; A/D Clock Select (2 Bits)
ADCS1           equ     ADCON0.7

					; *** OPTION Bits
PS0             equ     OPTION.0        ; Prescaler Divisor Select (3 Bits)
PS1             equ     OPTION.1
PS2             equ     OPTION.2
PSA             equ     OPTION.3        ; Prescaler Assignment
T0SE            equ     OPTION.4        ; Timer0 Source Edge Select
T0CS            equ     OPTION.5        ; Timer0 Clock Source Select
INTEDG          equ     OPTION.6        ; RB0/INT Edge Select
RBPU            equ     OPTION.7        ; Port B Weak Pull-Up Enable

					; *** PIE1 Bits
TMR1IE          equ     PIE1.0          ; Timer 1 Interrupt Enable
TMR2IE          equ     PIE1.1          ; Timer 2 Interrupt Enable
CCP1IE          equ     PIE1.2          ; CCP 1 Interrupt Enable
SSPIE           equ     PIE1.3          ; SSP Interrupt Enable
ADIE            equ     PIE1.6          ; A/D Interrupt Enable
PSPIE           equ     PIE1.7          ; PSP Interrupt Enable

					; *** PCON Bits
BOR             equ     PCON.0          ; Brown-Out Reset Flag
POR             equ     PCON.1          ; Power-On Reset Flag

					; *** SSPSTAT Bits
BF              equ     SSPSTAT.0       ; SSP Buffer Full Flag
UA              equ     SSPSTAT.1       ; SSP I2C Update Address Flag
R_W             equ     SSPSTAT.2       ; SSP I2C Read/Write Status
S               equ     SSPSTAT.3       ; SSP I2C Start Flag
P               equ     SSPSTAT.4       ; SSP I2C Stop Flag
D_A             equ     SSPSTAT.5       ; SSP I2C Data/Address Flag

					; *** ADCON1 Bits
PCFG0           equ     ADCON1.0        ; Port A Configuration (3 Bits)
PCFG1           equ     ADCON1.1
PCFG2           equ     ADCON1.2
