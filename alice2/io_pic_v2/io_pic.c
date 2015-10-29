#ifdef SDCC_MODE

#if PIC_LINE == pic18

#include <pic18fregs.h>

#pragma config OSC=EC           // External clock
#pragma config WDT=OFF          // Watchdog timer off

#else // FAMILY == pic16

// This will only work until I allocate a big buffer for SD card blocking

#include <pic16fregs.h>

#pragma config FOSC=HS          // External clock
#pragma config WDTE=OFF         // watchdog off

#endif // FAMILY

#else // ! SDCC_MODE - Microchip's C compiler

#include <xc.h>

#endif // SDCC_MODE

#ifndef ClrWdt
#define ClrWdt CLRWDT
#endif

#include <stdio.h>

void pause()
{
    int i;
    int j;
    for(j = 0; j < 8; j++)
        for (i=0;i<30000;i++) ClrWdt();
}

void setup()
{
    // For simulator; otherwise reports WDT exception
    ClrWdt();
#if PIC_LINE == pic18
    WDTCON = 0;
#endif

    // debug LEDs
    TRISA = 0x00;
}

char hello_LCD[] = "Hello LCD";

void try_lcd(int clock_idle_high, int output_valid_edge)
{
    int i;
    int dummy;
    // Set up SPI for NewHaven LCD
    // Set TMR2 to /16 pre, /8 comparator, source is Fosc/4,
    // output is /2 for SSP clock, yielding 19.53Khz
    // 20Mhz / 4 / 16 / 8 / 2 = 19.53125000000000000000
    T2CONbits.T2CKPS = 0b11;    // 1:16 prescale
    //PR2 = 8;                    // TMR2 PR2 trigger on 8
    PR2 = 32;                    // TMR2 PR2 trigger on 32
    T2CONbits.TMR2ON = 1;       // enable timer 2
    // SPI master mode
    TRISBbits.TRISB7 = 0;       // B7 is /SS for LCD - set to output
    PORTBbits.RB7 = 1;          // B7 is /SS for LCD - disable
    SSPCON1bits.SSPEN = 0;      // Clear SPI
    TRISCbits.TRISC5 = 0;       // SDO is output
    TRISCbits.TRISC3 = 0;       // SCK is output
    SSPCON1bits.CKP = clock_idle_high;        // Clock idle high
    SSPCON1bits.SSPM = 0b0011;  // SPI Master, CK = TMR2 / 2
    SSPSTATbits.CKE = output_valid_edge;        // Output valid by active(low) to idle(high) ??
    SSPSTATbits.SMP = 0;        // Sample at middle ??
    SSPCON1bits.SSPEN = 1;      // Enable SPI
    PORTBbits.RB7 = 0;          // B7 is /SS for LCD - enable
    // write to LCD 
    SSPBUF = 0xFE; while(!SSPSTATbits.BF); dummy = SSPBUF; // Command escape
    SSPBUF = 0x51; while(!SSPSTATbits.BF); dummy = SSPBUF; // Clear screen
    SSPBUF = 0xFE; while(!SSPSTATbits.BF); dummy = SSPBUF; // Command escape
    SSPBUF = 0x46; while(!SSPSTATbits.BF); dummy = SSPBUF; // Cursor Home
    for(i = 0; i < sizeof(hello_LCD); i++) {
        SSPBUF = hello_LCD[i]; while(!SSPSTATbits.BF); dummy = SSPBUF;
    }
    SSPBUF = '0' + clock_idle_high; while(!SSPSTATbits.BF); dummy = SSPBUF; // Cursor Home
    SSPBUF = '0' + output_valid_edge; while(!SSPSTATbits.BF); dummy = SSPBUF; // Cursor Home
    PORTBbits.RB7 = 1;          // B7 is /SS for LCD - disable
}

void main()
{
    int i;
    int dummy;
    setup();

    pause(); // NewHaven LCD requires 100ms delay on power-up

    try_lcd(0, 0);
    pause();
    pause();
    pause();
    try_lcd(0, 1);
    pause();
    pause();
    pause();
    try_lcd(1, 0);
    pause();
    pause();
    pause();
    try_lcd(1, 1);
    pause();
    pause();
    pause();

    // Set up USART
    // write to RS-232 "Hello Serial World" 

    // Set up SPI for SD card
    // read block 0

    // Set up SPI for LCD
    // write to LCD first 8 bytes in hex

    for(;;) {
        PORTA = 0x55;
        pause();

        PORTA = 0xaa;
        pause();
    }
}
