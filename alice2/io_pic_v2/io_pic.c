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

void spi_config_for_lcd()
{
    // Set up SPI for NewHaven LCD
    // Set TMR2 to /16 pre, /5 comparator, source is Fosc/4,
    // output is /2 for SSP clock, yielding 19.53Khz
    // 20Mhz / 4 / 16 / 8 / 2 = 26Khz
    T2CONbits.T2CKPS = 0b11;    // 1:16 prescale
    // PR2 = 6;                    // TMR2 PR2 trigger on 6
    PR2 = 8;                    // TMR2 PR2 trigger on 8
    T2CONbits.TMR2ON = 1;       // enable timer 2

    // slave select for LCD
    TRISBbits.TRISB7 = 0;       // B7 is /SS for LCD - set to output
    PORTBbits.RB7 = 1;          // B7 is /SS for LCD - disable

    // SPI master mode
    SSPCON1bits.SSPEN = 0;      // Disable and reset SPI
    TRISCbits.TRISC5 = 0;       // SDO is output
    TRISCbits.TRISC3 = 0;       // SCK is output
    SSPCON1bits.CKP = 1;        // Clock idle high
    SSPCON1bits.SSPM = 0b0011;  // SPI Master, CK = TMR2 / 2
    SSPSTATbits.CKE = 0;        // Output valid by active(low) to idle(high)
    SSPSTATbits.SMP = 0;        // Sample at middle (unused on LCD)
    SSPCON1bits.SSPEN = 1;      // Enable SPI
}

void spi_enable_lcd()
{
    PORTBbits.RB7 = 0;          // B7 is /SS for LCD - enable
}

void spi_disable_lcd()
{
    PORTBbits.RB7 = 1;          // B7 is /SS for LCD - disable
}


void main()
{
    int i;
    int dummy;
    setup();

    pause(); // NewHaven LCD requires 100ms delay on power-up

    spi_config_for_lcd();
    spi_enable_lcd();

    // write to LCD 
    SSPBUF = 0xFE; while(!SSPSTATbits.BF); dummy = SSPBUF; // Command escape
    SSPBUF = 0x51; while(!SSPSTATbits.BF); dummy = SSPBUF; // Clear screen
    SSPBUF = 0xFE; while(!SSPSTATbits.BF); dummy = SSPBUF; // Command escape
    SSPBUF = 0x46; while(!SSPSTATbits.BF); dummy = SSPBUF; // Cursor Home
    pause();
    for(i = 0; i < sizeof(hello_LCD) - 1; i++) {
        SSPBUF = hello_LCD[i]; while(!SSPSTATbits.BF); dummy = SSPBUF;
    }

    spi_disable_lcd();

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
