// 'C' source line config statements

#define _XTAL_FREQ 40000000

#include <xc.h>

#if defined(__18F452)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = ECIO       // Oscillator Selection bits (EC oscillator w/ OSC2 configured as RA6)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 20        // Brown-out Reset Voltage bits (VBOR set to 2.0V)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)

// CONFIG3H
#pragma config CCP2MUX = OFF     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)

// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP = OFF         // Low Voltage ICSP Enable bit (Low Voltage ICSP enabled)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000200-001FFFh) not code protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot Block (000000-0001FFh) not code protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000200-001FFFh) not write protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0001FFh) not write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000200-001FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from Table Reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0001FFh) not protected from Table Reads executed in other blocks)

#elif defined(__18F4620)


// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = ECIO6    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled and controlled by software (SBOREN is enabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#else

#error Chip type not handled in code 

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XSTR(x) STR(x)
#define STR(x) # x

void delay_100ms(unsigned char count)
{
    unsigned char i;
    for(i = 0; i < count * 10; i++) 
        __delay_ms(10);
}

void panic()
{
    for(;;) {
        PORTA = 0x05;
        delay_100ms(5);

        PORTA = 0x0a;
        delay_100ms(5);
    }
}

void bell()
{
    putch('\a');
}

void setup()
{
    SWDTEN = 0;

    // debug LEDs
    TRISA = 0xF0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC0 = 0;
    LATCbits.LC0 = 0;

    // /RESET control
    TRISBbits.TRISB3 = 1;       // B3 is /RESET to Z-80 - set to input (disabled)
    LATBbits.LB3 = 0;           // When B3 is enabled, it will be 0

    // /INT control
    TRISAbits.TRISA6 = 1;       // A6 is /INT to Z-80 - set to input (disabled)
    LATAbits.LA6 = 0;           // When A6 is enabled, it will be 0
}

void master_reset_start()
{
    LATBbits.LB3 = 0;           // When B3 is enabled, it will be 0
    TRISBbits.TRISB3 = 0;       // B3 is /RESET to Z-80 - set to output
    __delay_ms(1);
}

void master_reset_finish()
{
    TRISBbits.TRISB3 = 1;       // B3 is /RESET to Z-80 - set to output
}

void master_interrupt_start()
{
    LATAbits.LA6 = 0;           // When A6 is enabled, it will be 0
    TRISAbits.TRISA6 = 0;       // A6 is /RESET to Z-80 - set to output
    __delay_ms(1);
}

void master_interrupt_finish()
{
    TRISAbits.TRISA6 = 1;       // B3 is /RESET to Z-80 - set to output
}

int isprint(unsigned char a)
{
    return (a >= ' ') && (a <= '~');
}


unsigned char console_overflowed = 0;
unsigned char keyboard_overflowed = 0;

enum debug_levels {
    DEBUG_SILENT = 0,
    DEBUG_ERRORS,
    DEBUG_WARNINGS,
    DEBUG_EVENTS,
    DEBUG_DATA,
    DEBUG_ALL,
};
int debug = DEBUG_WARNINGS;

//----------------------------------------------------------------------------
// Monitor input queue

#define MON_QUEUE_LENGTH 32
volatile unsigned char mon_queue[MON_QUEUE_LENGTH];
volatile short mon_queue_next_head = 0;
volatile short mon_queue_tail = 0;

// Caller must ensure only one call can proceed at a time; currently
// called through isfull() and isempty(), so must protect those
unsigned short mon_queue_length()
{
    return (mon_queue_next_head + MON_QUEUE_LENGTH - mon_queue_tail) % MON_QUEUE_LENGTH;
}

// Caller must ensure only one call to isfull() or isempty() can
// proceed at a time.

// Currently called from main() or intr(), so must wrap with di(), ei()
unsigned char mon_queue_isfull()
{
    return mon_queue_length() == MON_QUEUE_LENGTH - 1;
}

// Currently called from main(), so, wrap with di(), ei()
unsigned char mon_queue_isempty()
{
    return mon_queue_length() == 0;
}

// Caller must ensure only one call can proceed at a time ; currently
// called from main() and intr(), so wrap with di(), ei()
void mon_enqueue(unsigned char d)
{
    mon_queue[mon_queue_next_head] = d;
    mon_queue_next_head = (mon_queue_next_head + 1) % MON_QUEUE_LENGTH;
}

// Caller must ensure only one call can proceed at a time; currently
// called only from main() so already protected
unsigned char mon_dequeue()
{
    unsigned char d = mon_queue[mon_queue_tail];
    mon_queue_tail = (mon_queue_tail + 1) % MON_QUEUE_LENGTH;
    return d;
}

// Call this from ISR, so skip di/ei
void monitor_enqueue_key_unsafe(unsigned char d)
{
    if(!mon_queue_isfull()) {
        mon_enqueue(d);
    }
}


//----------------------------------------------------------------------------
// Console input queue

#define CON_QUEUE_LENGTH 64
volatile unsigned char con_queue[CON_QUEUE_LENGTH];
volatile short con_queue_next_head = 0;
volatile short con_queue_tail = 0;

// Caller must ensure only one call can proceed at a time; currently
// called through isfull() and isempty(), so must protect those
unsigned short con_queue_length()
{
    return (con_queue_next_head + CON_QUEUE_LENGTH - con_queue_tail) % CON_QUEUE_LENGTH;
}

// Caller must ensure only one call to isfull() or isempty() can
// proceed at a time.

// Currently called from main() or intr(), so must wrap with di(), ei()
unsigned char con_queue_isfull()
{
    return con_queue_length() == CON_QUEUE_LENGTH - 1;
}

// Currently called from main(), so, wrap with di(), ei()
unsigned char con_queue_isempty()
{
    return con_queue_length() == 0;
}

// Caller must ensure only one call can proceed at a time ; currently
// called from main() and intr(), so wrap with di(), ei()
void con_enqueue(unsigned char d)
{
    con_queue[con_queue_next_head] = d;
    con_queue_next_head = (con_queue_next_head + 1) % CON_QUEUE_LENGTH;
}

// Caller must ensure only one call can proceed at a time; currently
// called only from main() so already protected
unsigned char con_dequeue()
{
    unsigned char d = con_queue[con_queue_tail];
    con_queue_tail = (con_queue_tail + 1) % CON_QUEUE_LENGTH;
    return d;
}

void console_enqueue_key(unsigned char d)
{
    unsigned char full;
    di();
    full = con_queue_isfull();
    ei();
    if(full) {
        console_overflowed = 1;
    } else {
        di();
        con_enqueue(d);
        ei();
    }
}

// Call this from ISR, so skip di/ei
void console_enqueue_key_unsafe(unsigned char d)
{
    unsigned char full;
    full = con_queue_isfull();
    if(full) {
        console_overflowed = 1;
    } else {
        con_enqueue(d);
    }
}


//----------------------------------------------------------------------------
// AT and PS/2 Keyboard processing

// Keyboard I/O constants
#define KBD_BIT_COUNT 11

volatile short kbd_bits = 0;
volatile unsigned short kbd_data = 0;
volatile char up_key_flag = 0;
volatile char kbd_shift_status = 0;
volatile char kbd_alt_status = 0;
volatile char kbd_ctrl_status = 0;

volatile unsigned char dump_keyboard_data = 0;

#define LSHIFT_KEY 0x12
#define RSHIFT_KEY 0x59
#define CTRL_KEY 0x14
#define ALT_KEY 0x11
#define UP_KEY 0xF0
#define EXT_KEY 0xE0
#define EXT2_KEY 0xE1

#define KBD_QUEUE_LENGTH 16
volatile unsigned char kbd_queue[KBD_QUEUE_LENGTH];
volatile unsigned char kbd_queue_next_head = 0;
volatile unsigned char kbd_queue_tail = 0;

// Caller must ensure only one call can proceed at a time; currently
// called through isfull() and isempty(), so must protect those
unsigned short kbd_queue_length()
{
    return (kbd_queue_next_head + KBD_QUEUE_LENGTH - kbd_queue_tail) % KBD_QUEUE_LENGTH;
}

// Caller must ensure only one call to isfull() or isempty() can
// proceed at a time.

// Currently called only from interrupt routine so already protected
unsigned char kbd_queue_isfull()
{
    return kbd_queue_length() == KBD_QUEUE_LENGTH - 1;
}

// Currently called from main() so must wrap with di(), ei()
unsigned char kbd_queue_isempty()
{
    return kbd_queue_length() == 0;
}

// Caller must ensure only one call can proceed at a time ; currently
// typically called only from interrupt routine so already protected
void kbd_enqueue(unsigned char d)
{
    kbd_queue[kbd_queue_next_head] = d;
    kbd_queue_next_head = (kbd_queue_next_head + 1) % KBD_QUEUE_LENGTH;
}

// Caller must ensure only one call can proceed at a time; currently
// called only from main() so already protected
unsigned char kbd_dequeue()
{
    unsigned char d = kbd_queue[kbd_queue_tail];
    kbd_queue_tail = (kbd_queue_tail + 1) % KBD_QUEUE_LENGTH;
    return d;
}

// Normal, shift, ctrl, alt
const unsigned char kbd_table[] = {
   '?', '?', '?', '?',
   '9', '9', '9', '9',
   '?', '?', '?', '?',
   '5', '5', '5', '5',
   '3', '3', '3', '3',
   '1', '1', '1', '1',
   '2', '2', '2', '2',
   '1', '1', '1', '1',
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '8', '8', '8', '8',
   '6', '6', '6', '6',
   '4', '4', '4', '4',
   9,   9,   9,   9,
   '`', '~', '`', '`',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'q', 'Q',  17,  17,
   '1', '!', '1', '1',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'z', 'Z',  26,  26,
   's', 'S',  19,  19,
   'a', 'A',   1,   1,
   'w', 'W',  23,  23,
   '2', '@', '2', '2',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'c', 'C',   3,   3,
   'x', 'X',  24,  24,
   'd', 'D',   4,   4,
   'e', 'E',   5,   5,
   '4', '$', '4', '4',
   '3', '#', '3', '3',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   ' ', ' ', ' ', ' ' ,
   'v', 'V',  22,  22,
   'f', 'F',   6,   6,
   't', 'T',  20,  20,
   'r', 'R',  18,  18,
   '5', '%', '5', '5',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'n', 'N',  14,  14,
   'b', 'B',   2,   2,
   'h', 'H',   8,   8,
   'g', 'G',   7,   7,
   'y', 'Y',  25,  25,
   '6', '^', '6', '6',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   'm', 'M',  13,  13,
   'j', 'J',  10,  10,
   'u', 'U',  21,  21,
   '7', '&', '7', '7',
   '8', '*', '8', '8',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   ',', '<', ',', ',',
   'k', 'K',  11,  11,
   'i', 'I',   9,   9,
   'o', 'O',  15,  15,
   '0', ')', '0', '0',
   '9', '(', '9', '9',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '.', '>', '.', '.',
   '/', '?', '/', '/',
   'l', 'L',  12,  12,
   ';', ':', ';', ';',
   'p', 'P',  16,  16,
   '-', '_', '-', '-',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   39, '"',  39,  39,
   '?', '?', '?', '?',
   '[', '{', '[', '[',
   '=', '+', '=', '=',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   13,  13,  13,  13,
   ']', '}', ']', ']',
   '?', '?', '?', '?',
   92, '|',  92,  92,
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   8,   8,   8,   8,
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '?', '?', '?', '?',
   '4', '4', '4', '4',
   '7', '7', '7', '7',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
   '0', '0', '0', '0',
   '.', '.', '.', '.',
   '2', '2', '2', '2',
   '5', '5', '5', '5',
   '6', '6', '6', '6',
   '8', '8', '8', '8',
   27,  27,  27,  27,
   '?', '?', '?', '?',
   '1', '1', '1', '1',
   '+', '+', '+', '+',
   '3', '3', '3', '3',
   '-', '-', '-', '-',
   '*', '*', '*', '*',
   '9', '9', '9', '9',
   '?', '?', '?', '?',
   '?', '?', '?', '?',
};

unsigned char kbd_lookup(int shift, int alt, int ctrl, unsigned char byte)
{
    int which = 0;
    if(shift) which = 1;
    else if(ctrl) which = 2;
    else if(alt) which = 3;
    return kbd_table[byte * 4 + which];
}

void kbd_process_byte(unsigned char kbd_byte)
{
    if(kbd_byte == UP_KEY) {
        up_key_flag = 1;
        if(dump_keyboard_data) 
            printf("keyboard key up\n");
    } else {
        switch(kbd_byte) {
            case LSHIFT_KEY:
            case RSHIFT_KEY:
                kbd_shift_status = !up_key_flag;
                if(dump_keyboard_data) 
                    printf("shift status is now %d\n", kbd_shift_status);
                break;
            case ALT_KEY:
                kbd_alt_status = !up_key_flag;
                if(dump_keyboard_data) 
                    printf("alt status is now %d\n", kbd_alt_status);
                break;
            case CTRL_KEY:
                kbd_ctrl_status = !up_key_flag;
                if(dump_keyboard_data) 
                    printf("ctrl status is now %d\n", kbd_ctrl_status);
                break;
            default:
                if(!up_key_flag)
                    if(!(kbd_byte & 0x80)) {
                        unsigned char c = kbd_lookup(kbd_shift_status, kbd_alt_status, kbd_ctrl_status, kbd_byte);
                        if(dump_keyboard_data) {
                            printf("keyboard ASCII: %02X", c);
                            if(isprint(c))
                                printf("(%c)\n", c);
                            else 
                                putch('\n');
                        }
                        console_enqueue_key(c);
                    }
                break;
        }
        up_key_flag = 0;
    }
}

void setup_keyboard()
{
    INTEDG0 = 1;
    INT0IF = 0;
    INT0IE = 1; // enable interrupts on INT0/RB0
}


/*--------------------------------------------------------------------------*/
/* USART - serial comms ----------------------------------------------------*/

#if defined(__18F452)

const int baud_rate_code = 21; // 115200 baud at 40 MHz, BRGH=1

#elif defined(__18F4620)

#if _XTAL_FREQ == 40000000

const int baud_rate_code = 86; // 115200 baud at 40 MHz, BRG16=1, BRGH=1

#elif _XTAL_FREQ == 32000000

const int baud_rate_code = 68; // 115200 baud at 32 MHz, BRG16=1, BRGH=1

#else /* Other frequency */

#error Crystal frequency not handled in code

#endif /* XTAL_FREQ */

#endif /* CHIP type */

void setup_serial()
{
    // clear
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    PIR1bits.RCIF = 0;

    TRISCbits.TRISC6 = 0;       // TX is output
    TRISCbits.TRISC7 = 1;       // RX is input

#if defined(__18F452)

    BRGH = 1;
    SPBRG = baud_rate_code;

#elif defined(__18F4620)

    BRG16 = 1;
    BRGH = 1;
    SPBRG = baud_rate_code & 0xff;
    SPBRGH = (baud_rate_code >> 8) & 0xff;

#endif /* CHIP type */

    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 0;
    TXSTAbits.TXEN = 1;

    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 1;
}

void send_serial(unsigned char b)
{
    CLRWDT(); // XXX Why?  SWDTEN and WDTEN are 0!
    while(!PIR1bits.TXIF);
    TXREG = b;
}


//----------------------------------------------------------------------------
// Alice 3 Bus

volatile unsigned char serial_is_monitor;
volatile unsigned char pic_monitor_latch;

#define PIC_POLL_AGAIN 0x00
#define PIC_SUCCESS 0x01
#define PIC_READY 0x01
#define PIC_FAILURE 0xFF
#define PIC_NOT_READY 0xFF

#define PIC_CMD_NONE  0x00
#define PIC_CMD_MIN  0x01
#define PIC_CMD_READ  0x01
#define PIC_CMD_WRITE 0x02
#define PIC_CMD_CONST 0x03
#define PIC_CMD_CONIN 0x04
#define PIC_CMD_SEROUT 0x05
#define PIC_CMD_READ_SUM 0x06
#define PIC_CMD_WRITE_SUM 0x07
#define PIC_CMD_MAX 0x07

volatile unsigned char command_request; /* set this after reading all the bytes */
volatile unsigned char command_bytes[1 + 5 + 128 + 2]; // largest is status + write + sector + 16-bit checksum
volatile unsigned char command_length;

volatile unsigned char response_bytes[1 + 128 + 2]; // largest is status + sector + 16-bit checksum
volatile unsigned char response_staging_length;
volatile unsigned char response_length;
volatile unsigned char response_index;
volatile unsigned char response_waiting;

void response_start()
{
    response_staging_length = 0;
}

void response_append(unsigned char r)
{
    if(response_staging_length >= sizeof(response_bytes)) {
        printf("PANIC: overflowed response buffer\n");
        panic();
    }
    response_bytes[response_staging_length++] = r;
}

// Caller must protect with di() / ei()
void response_finish()
{
    response_index = 0;
    response_length = response_staging_length;
    response_waiting = 1;
}

// Element 0 is 1 here to force stoppage on receiving a bad command
const unsigned char PIC_command_lengths[8] = {1, 6, 134, 1, 1, 2, 6, 136};

void command_clear()
{
    di();
    command_request = PIC_CMD_NONE;
    command_length = 0;
    ei();
}

void response_clear()
{
    di();
    response_length = 0;
    response_index = 0;
    response_waiting = 0;
    ei();
}

void setup_slave_port()
{
    // ADCON has to be set for RE2:RE0 to be inputs
#if defined(__18F4620)
    ADCON1bits.PCFG3 = 1;              
#endif
    ADCON1bits.PCFG2 = 1;               
    ADCON1bits.PCFG1 = 1;
    ADCON1bits.PCFG0 = 1;

    TRISEbits.TRISE0 = 1;		// E0,/RD is input
    TRISEbits.TRISE1 = 1;		// E1,/WR is input
    TRISEbits.TRISE2 = 1;		// E2,/CS is input

    LATD = 0;                           // clear PORTD

    TRISEbits.PSPMODE = 1;		// port D is PSP

                                        // read PORTD and maybe clear IBF
    command_bytes[sizeof(command_bytes) - 1] = PORTD;

    PIR1bits.PSPIF = 0;                 // clear the interrupt flag for PSP
    PIE1bits.PSPIE = 1;                 // enable interrupts on PSP
}

void enable_interrupts()
{
    INTCONbits.PEIE = 1; // enable peripheral interrupts
    INTCONbits.GIE = 1; // enable interrupts
    ei(); // XXX does this just do the above?
}


/*--------------------------------------------------------------------------*/
/* SPI commands ------------------------------------------------------------*/

unsigned char spi_exchange(unsigned char b)
{
    SSPBUF = b;
    while(!SSPSTATbits.BF);
    CLRWDT(); // XXX Why?  SWDTEN and WDTEN are 0!
    return SSPBUF;
}


/*--------------------------------------------------------------------------*/
/* SD-card-specific SPI commands -------------------------------------------*/

// Postcondition: SPI configured for SD, SS high (false)
void spi_config_for_sd()
{
    // Set up SPI for SD card
    // Set TMR2 to /16 pre, /1 comparator, source is Fosc/4,
    // 40Mhz / 4 / 16 / 1 / 2 = 312.5KHz

    T2CONbits.T2CKPS = 0b11;    // 1:16 prescale
    PR2 = 1;                    // TMR2 PR2 trigger on 8
    T2CONbits.TMR2ON = 1;       // enable timer 2

    // slave select for SD
    TRISBbits.TRISB4 = 0;       // B4 is /SS for SD - set to output
    PORTBbits.RB4 = 1;          // B4 is /SS for SD - disable

    // SD power control

    LATBbits.LB2 = 0;           // turn power off
    TRISBbits.TRISB2 = 1;       // B2 is power to SD card - set to input (disabled)

    // SPI master mode
    SSPCON1bits.SSPEN = 0;      // Disable and reset SPI
    TRISCbits.TRISC5 = 0;       // SDO is output
    TRISCbits.TRISC4 = 1;       // SDI is input
    TRISCbits.TRISC3 = 0;       // SCK is output
    SSPCON1bits.CKP = 0;        // Clock idle low
    SSPCON1bits.SSPM = 0b0011;  // SPI Master, CK = TMR2 / 2
    SSPSTATbits.CKE = 1;        // Output valid by active(low) to idle(high)
    SSPSTATbits.SMP = 1;        // Sample at middle
    SSPCON1bits.SSPEN = 1;      // Enable SPI
}

void sdcard_power_on()
{
    TRISBbits.TRISB2 = 0;       // B2 is power to SD card - set to output
    LATBbits.LB2 = 0;           // turn power off
}

void sdcard_power_off()
{
    TRISBbits.TRISB2 = 1;       // B2 is power to SD card - set to input (disabled)
}

void sdcard_reset()
{
    sdcard_power_off();
    delay_100ms(10);
    sdcard_power_on();
    delay_100ms(20);
}

void spi_enable_sd()
{
    PORTBbits.RB4 = 0;          // B4 is /SS for SD - enable
}

void spi_disable_sd()
{
    PORTBbits.RB4 = 1;          // B4 is /SS for SD - disable
}


/*--------------------------------------------------------------------------*/
/* define putch() so sprintf goes to the LCD screen ------------------------*/

void putch(unsigned char byte)
{
    send_serial(byte);
    CLRWDT(); // XXX Why?  SWDTEN and WDTEN are 0!
}


/*--------------------------------------------------------------------------*/
/* SD card -----------------------------------------------------------------*/

#define SDCARD_SUCCESS 1

unsigned char crc7_add_byte(unsigned char data, unsigned char previous_crc)
{
    unsigned char crc = previous_crc;
    int b;

    for (b = 0; b < 8; b++) {
        unsigned int crcbit = crc & 0x40;

        if ((data & 0x80UL) != 0)
            crcbit ^= 0x40;

        data <<= 1;
        crc <<= 1;

        if (crcbit != 0)
            crc ^= 0x09;
    }

    return crc;
}

unsigned char crc7_generate_bytes(unsigned char *b, int count)
{
    unsigned char crc = 0;
    int i;

    for(i = 0; i < count; i++)
        crc = crc7_add_byte(b[i], crc);

    return crc;
}

/* Linux 2.6.32 crc-itu-t.c */
/** CRC table for the CRC ITU-T V.41 0x0x1021 (x^16 + x^12 + x^15 + 1) */
const unsigned short crc_itu_t_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned short crc_itu_t_byte(unsigned short crc, const unsigned char data)
{
    return (crc << 8) ^ crc_itu_t_table[((crc >> 8) ^ data) & 0xff];
}

/**
 * crc_itu_t - Compute the CRC-ITU-T for the data buffer
 *
 * @crc:     previous CRC value
 * @buffer:  data pointer
 * @len:     number of bytes in the buffer
 *
 * Returns the updated CRC value
 */
unsigned short crc_itu_t(unsigned short crc, const unsigned char *buffer, size_t len)
{
    while (len--)
        crc = crc_itu_t_byte(crc, *buffer++);
    return crc;
}


int timeout_count = 32000;

#define BLOCK_SIZE 512

unsigned char block_buffer[BLOCK_SIZE];

// cribbed somewhat from http://elm-chan.org/docs/mmc/mmc_e.html
enum sdcard_command {
    CMD0 = 0,    // init; go to idle state
    CMD8 = 8,    // send interface condition
    CMD17 = 17,  // read single block
    CMD24 = 24,  // write single block
    CMD55 = 55,  // prefix command for application command
    ACMD41 = 41, // application command to send operating condition
};
unsigned char sdcard_response_IDLE = 0x01;
unsigned char sdcard_response_SUCCESS = 0x00;
unsigned char sdcard_response_DATA_ACCEPTED = 0xE5;
unsigned char sdcard_token_17_18_24 = 0xFE;

void spi_bulk(unsigned char *buffer, unsigned int nlen)
{
    int i;

    for(i = 0; i < nlen; i++) {
        buffer[i] = spi_exchange(buffer[i]);
    }
}

void spi_writen(unsigned char *buffer, unsigned int nlen)
{
    int i;
    unsigned char dummy;

    for(i = 0; i < nlen; i++) {
        dummy = spi_exchange(buffer[i]);
    }
}

void spi_readn(unsigned char *buffer, unsigned int nlen)
{
    int i;

    for(i = 0; i < nlen; i++) {
        buffer[i] = spi_exchange(0xff);
    }
}

// response length must include initial R1, so 1 for CMD0
int send_sdcard_command(enum sdcard_command command, unsigned long parameter, unsigned char *response, int response_length)
{
    int count;
    unsigned char command_buffer[6];

    command_buffer[0] = 0x40 | command;
    command_buffer[1] = (parameter >> 24) & 0xff;
    command_buffer[2] = (parameter >> 16) & 0xff;
    command_buffer[3] = (parameter >> 8) & 0xff;
    command_buffer[4] = (parameter >> 0) & 0xff;
    command_buffer[5] = ((crc7_generate_bytes(command_buffer, 5) & 0x7f) << 1) | 0x01;

    if(debug >= DEBUG_DATA) printf("command constructed: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    spi_bulk(command_buffer, sizeof(command_buffer));
    if(debug >= DEBUG_ALL) printf("returned in buffer: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    count = 0;
    do {
        if(count > timeout_count) {
            printf("send_sdcard_command: timed out waiting on response\n");
            return 0;
        }
        spi_readn(response, 1);
        if(debug >= DEBUG_ALL) printf("response 0x%02X\n", response[0]);
        count++;
    } while(response[0] & 0x80);

    if(response_length > 1) {
        spi_readn(response + 1, response_length - 1);
    }

    return 1;
}

// precondition: SD card CS is high (disabled)
// postcondition: SD card CS is low (enabled)
int sdcard_init()
{
    unsigned char response[8];
    unsigned long OCR;
    int count;
    unsigned int u;

    /* CS false, 80 clk pulses (read 10 bytes) */
    unsigned char buffer[10];
    for(u = 0; u < sizeof(buffer); u++)
        buffer[u] = 0xff;

    spi_bulk(buffer, sizeof(buffer));

    spi_enable_sd();
    /* interface init */
    if(!send_sdcard_command(CMD0, 0, response, 1))
        return 0;
    if(response[0] != sdcard_response_IDLE) {
        printf("sdcard_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }

    /* check voltage */
    if(!send_sdcard_command(CMD8, 0x000001AA, response, 5))
        return 0;
    if(response[0] != sdcard_response_IDLE) {
        printf("sdcard_init: failed to get OCR, response was 0x%02X\n", response[0]);
        return 0;
    }
    OCR = (((unsigned long)response[1]) << 24) | (((unsigned long)response[2]) << 16) | (((unsigned long)response[3]) << 8) | (((unsigned long)response[4]) << 0);
    printf("sdcard_init: OCR response is 0x%08lX\n", OCR);

    // should get CSD, CID, print information about them

    // Ask the card to initialize itself, and wait for it to get out of idle mode.
    count = 0; 
    do {
        if(count > timeout_count) {
            printf("sdcard_init: timed out waiting on transition to ACMD41\n");
            return 0;
        }
        /* leading command to the ACMD41 command */
        if(!send_sdcard_command(CMD55, 0x00000000, response, 1))
            return 0;
        if(response[0] != sdcard_response_IDLE) {
            printf("sdcard_init: not in IDLE mode for CMD55, response was 0x%02X\n", response[0]);
            return 0;
        }
        /* start initialization process, set HCS (high-capacity) */
        if(!send_sdcard_command(ACMD41, 0x40000000, response, 1))
            return 0;
        count++;
    } while(response[0] != sdcard_response_SUCCESS);
    if(debug >= DEBUG_ALL) printf("returned from ACMD41: %02X\n", response[0]);

    return 1;
}

void dump_more_spi_bytes(const char *why)
{
    unsigned char response[8];
    spi_readn(response, sizeof(response));
    printf("trailing %s: %02X %02X %02X %02X %02X %02X %02X %02X\n", why,
        response[0], response[1], response[2], response[3],
        response[4], response[5], response[6], response[7]);
}

/* precondition: SDcard CS is low (active) */
int sdcard_readblock(unsigned int blocknum, unsigned char *block)
{
    int count;
    unsigned char response[8];

    // Send read block command.
    response[0] = 0xff;
    if(!send_sdcard_command(CMD17, blocknum, response, 1))
        return 0;
    if(response[0] != sdcard_response_SUCCESS) {
        printf("sdcard_readblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait for the data token.
    count = 0;
    do {
        if(count > timeout_count) {
            printf("sdcard_readblock: timed out waiting for data token\n");
            return 0;
        }
        spi_readn(response, 1);
        if(debug >= DEBUG_ALL) printf("readblock response 0x%02X\n", response[0]);
        count++;
    } while(response[0] != sdcard_token_17_18_24);

    // Read data.
    spi_readn(block, BLOCK_SIZE);

    // Read CRC
    spi_readn(response, 2);
    if(debug >= DEBUG_DATA) printf("CRC is 0x%02X%02X\n", response[0], response[1]);

    unsigned short crc_theirs = response[0] * 256 + response[1];

    // calculate our version of CRC and compare
    unsigned short crc_ours = crc_itu_t(0, block, BLOCK_SIZE);

    if(crc_theirs != crc_ours) {
        printf("CRC mismatch (theirs %04X versus ours %04X, reporting failure)\n", crc_theirs, crc_ours);
        return 0;
    } else {
        if(debug >= DEBUG_DATA) printf("CRC matches\n");
    }

    // Wait for DO to go high. I don't think we need to do this for block reads,
    // but I don't think it'll hurt.
    count = 0;
    do {
        if(count > timeout_count) {
            printf("sdcard_readblock: timed out waiting on completion\n");
            return 0;
        }
        spi_readn(response, 1);
        if(debug >= DEBUG_ALL) printf("readblock response 0x%02X\n", response[0]);
        count++;
    } while(response[0] != 0xFF);

    if(debug >= DEBUG_ALL) dump_more_spi_bytes("read completion");

    return 1;
}

/* precondition: SDcard CS is low (active) */
int sdcard_writeblock(unsigned int blocknum, unsigned char *block)
{
    int count;
    unsigned int u;
    unsigned char response[8];

    // Send write block command.
    if(!send_sdcard_command(CMD24, blocknum, response, 1))
        return 0;
    if(response[0] != sdcard_response_SUCCESS) {
        printf("sdcard_writeblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }
    // XXX - elm-chan.org says I should be waiting >= 1byte here

    // Data token.
    response[0] = sdcard_token_17_18_24;
    spi_writen(response, 1);

    // Send data.
    spi_writen(block, BLOCK_SIZE);

    // junk CRC
    response[0] = 0xff;
    response[1] = 0xff;
    spi_writen(response, 2);

    // Get DATA_ACCEPTED response from WRITE
    spi_readn(response, 1);
    if(debug >= DEBUG_DATA) printf("writeblock response 0x%02X\n", response[0]);
    if(response[0] != sdcard_response_DATA_ACCEPTED) {
        printf("sdcard_writeblock: failed to respond with DATA_ACCEPTED, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait while busy (DO = low).
    count = 0;
    do {
        if(count > timeout_count) {
            printf("sdcard_writeblock: timed out waiting on completion\n");
            return 0;
        }
        spi_readn(response, 1);
        if(debug >= DEBUG_ALL) printf("writeblock completion 0x%02X\n", response[0]);
        count++;
    } while(response[0] != 0xFF);
    if(debug >= DEBUG_DATA) printf("looped %d times waiting on write to complete.\n", count);

    if(debug >= DEBUG_ALL) dump_more_spi_bytes("write completion");

    return 1;
}

void dump_buffer_hex(int indent, unsigned char *data, int size)
{
    int address = 0;
    int i;

    while(size > 0) {
        int howmany = (size < 16) ? size : 16;

        printf("%*s0x%04X: ", indent, "", address);
        for(i = 0; i < howmany; i++)
            printf("%02X ", data[i]);
        printf("\n");

        printf("%*s        ", indent, "");
        for(i = 0; i < howmany; i++)
            printf(" %c ", isprint(data[i]) ? data[i] : '.');
        printf("\n");

        size -= howmany;
        data += howmany;
        address += howmany;
    }
}

void test_sd_card()
{
#if 0 // XXX original_block cannot be allocated on 18F452 with all other stuff
    unsigned int u;
    int i;
    int success;
    int block_number;
    static unsigned char original_block[BLOCK_SIZE];

    // test SD card
    block_number = 0;

    for(;;) {
        printf("block %d\n", block_number);
        // printf("Reading a block\n");
        if(!sdcard_readblock(block_number, original_block)) {
            panic();
        }
        // printf("Original block:\n");
        // dump_buffer_hex(4, original_block, BLOCK_SIZE);

        for(i = 0; i < BLOCK_SIZE; i++)
            block_buffer[i] = (original_block[i] + 0x55) % 256;

        if(!sdcard_writeblock(block_number, block_buffer)) {
            printf("PANIC: Failed writeblock\n");
            panic();
        }
        // printf("Wrote junk block\n");

        for(i = 0; i < BLOCK_SIZE; i++)
            block_buffer[i] = 0;
        if(!sdcard_readblock(block_number, block_buffer)) {
            printf("PANIC: Failed readblock\n");
            panic();
        }

        success = 1;
        for(i = 0; i < BLOCK_SIZE; i++)
            if(block_buffer[i] != (original_block[i] + 0x55) % 256)
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of junk to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                block_buffer[0], block_buffer[1], block_buffer[2], block_buffer[3]);
        } else {
            printf("Verified junk block was written\n");
        }

        if(!sdcard_writeblock(block_number, original_block)) {
            printf("PANIC: Failed writeblock\n");
            panic();
        }
        // printf("Wrote original block\n");

        if(!sdcard_readblock(block_number, block_buffer)) {
            printf("PANIC: Failed readblock\n");
            panic();
        }

        success = 1;
        for(i = 0; i < BLOCK_SIZE; i++)
            if(original_block[i] != block_buffer[i])
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of original to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                block_buffer[0], block_buffer[1], block_buffer[2], block_buffer[3]);
        } else {
            printf("Verified original block was written\n");
        }
        block_number ++;
        if(block_number % 2 == 0)
            PORTA = 0x0f;
        else
            PORTA = 0x00;
    }
#endif
}

int previous_block = 0xffff;

#define sectors_per_block 4
#define sectors_per_track 64
#define tracks_per_disk 1024
#define sector_size 128
/* disk is 8MB, so 16384 512-byte blocks per disk */
#define blocks_per_disk 16384

volatile unsigned char pspif_but_rd_asserted_count = 0;

void interrupt intr()
{
    if(PIR1bits.PSPIF) {

        // http://ww1.microchip.com/downloads/en/DeviceDoc/31010a.pdf
        // PSPIF means /RD or /WR transitioned.  So read or write or both

        if(TRISEbits.IBF) {
            // IBF set when master has written to PSP and PIC has not yet read

            // XXX command_length++ could overflow command_bytes[]
            command_bytes[command_length++] = PORTD;
            // reading PORTD clears IBF
        }
        
        if(response_length > 0 && !TRISEbits.OBF) {

            if(PORTEbits.RE0 == 0 && PORTEbits.RE1 == 0) {

                pspif_but_rd_asserted_count++;

            } else {

                LATCbits.LC0 = 1; // debug LED: 1 if read

                if(response_index < response_length) {

                    LATD = response_bytes[response_index];
                    // writing LATD sets OBF
                    response_index++;

                } else  {

                    response_index = 0;
                    response_length = 0;
                    response_waiting = 0;
                    // writing LATD sets OBF
                    LATD = 0;
                }

                LATCbits.LC0 = 0; // debug LED
            }
        }

        // We cleared PSP last in old ASM code
        // XXX - probably PSPIF = (RD || WR), so must clear RD and WR
        // interrupts before clearing PSPIF...
        PIR1bits.PSPIF = 0; // clear PSP interrupt
    }

    if(RCIF) {
        monitor_enqueue_key_unsafe(RCREG); // reading RCREG clears RCIF
    }

    if(INT0IF) {
        kbd_data = (kbd_data >> 1) | (PORTBbits.RB1 << 10);
        INT0IF = 0;
        kbd_bits++;
        if(kbd_bits == KBD_BIT_COUNT) {
            if(kbd_queue_isfull()) {
                keyboard_overflowed = 1;
            } else {
                kbd_enqueue((kbd_data >> 1) & 0xff);
            }
            kbd_data = 0;
            kbd_bits = 0;
        }
    }
}

char local_command[80];
unsigned char local_command_length = 0;

void usage()
{
    printf("help - this help message\n");
    printf("debug N - set debug level\n");
    printf("buffers - print summary of command and response buffers\n");
    printf("reset - reset Z80 and clear communication buffers\n");
    printf("int - send /INT to Z80\n");
    printf("sdreset - reset SD\n");
    printf("dumpkbd - toggle dumping keyboard\n");
    printf("clear - clear command and response buffer\n");
    printf("pass - pass monitor keys to Z80\n");
    printf("version - print firmware build version\n");
    printf("read N - read and dump block\n");
    printf("panic - force panic\n");
}

#define PIC_FIRMWARE_VERSION_STRING XSTR(PIC_FIRMWARE_VERSION)

void process_local_key(unsigned char c)
{
    if(c == '\r' || c == '\n') {
        putch('\n');
        local_command[local_command_length] = 0;

        if((strcmp(local_command, "help") == 0) ||
           (strcmp(local_command, "h") == 0) ||
           (strcmp(local_command, "?") == 0)) {

            usage();

        } else if(strcmp(local_command, "sdreset") == 0) {

            printf("Resetting SD card...\n");

            sdcard_reset();
            if(!sdcard_init()) {
                printf("PANIC: failed to start access to SD card as SPI\n");
                panic();
            }

        } else if(strcmp(local_command, "dumpkbd") == 0) {

            dump_keyboard_data = !dump_keyboard_data;
            if(dump_keyboard_data)
                printf("Dumping keyboard data...\n");
            else
                printf("Not dumping keyboard data...\n");

        } else if(strcmp(local_command, "panic") == 0) {

            panic();

        } else if(strcmp(local_command, "reset") == 0) {

            master_reset_start();
            printf("Resetting Z-80 and communication buffers...\n");
            response_clear();
            command_clear();
            master_reset_finish();

        } else if(strcmp(local_command, "int") == 0) {

            master_interrupt_start();
            printf("Interupting Z-80...");
            putch('\n');
            master_interrupt_finish();

        } else if(strcmp(local_command, "clear") == 0) {

            response_clear();
            command_clear();
            printf("Command and response data cleared\n");

        } else if(strcmp(local_command, "pass") == 0) {

            serial_is_monitor = 0;
            printf("Press CTRL-A then CTRL-B to return to monitor.\n");

        } else if(strcmp(local_command, "buffers") == 0) {

            printf("Command request: 0x%02X\n", command_request);
            printf("Command length: %d bytes\n", command_length);
            if(command_length > 0) {
                printf("Command buffer:\n");
                dump_buffer_hex(4, command_bytes, command_length);
            }

            printf("Response length: %d bytes\n", response_length);
            printf("Response next byte to put on bus: %d\n", response_index);
            if(response_length > 0) {
                printf("Response buffer:\n");
                dump_buffer_hex(4, response_bytes, response_length);
            }

        } else if(strcmp(local_command, "version") == 0) {

            printf("%s\n", PIC_FIRMWARE_VERSION_STRING);

        } else if(strncmp(local_command, "debug ", 6) == 0) {

            unsigned char *p = local_command + 5;
            while(*p == ' ')
                p++;
            debug = strtol(p, NULL, 0);
            printf("debug set to %d\n", debug);

        } else if(strncmp(local_command, "read ", 5) == 0) {

            unsigned char *p = local_command + 4;
            unsigned int block_number;
            while(*p == ' ')
                p++;
            block_number = strtol(p, NULL, 0);
            if(sdcard_readblock(block_number, block_buffer)) {
                dump_buffer_hex(4, block_buffer, BLOCK_SIZE);
            }

        } else {

            printf("Unknown command.\n");
            usage();
        }

        printf("* ");
        local_command_length = 0;

    } else {

        if(c == 127 || c == '\b') {
            if(local_command_length > 0) {
                putch('\b');
                putch(' ');
                putch('\b');
                local_command_length--;
            } else {
                bell();
            }
        } else {
            if(local_command_length < sizeof(local_command) - 1) {
                putch(c);
                local_command[local_command_length++] = c;
            } else {
                bell();
            }
        }
    }
}

void main()
{
    unsigned char bad_read_count = 0;
    unsigned char tmp = 0;
    unsigned char was_response_waiting = 0;
    unsigned char isempty;
    unsigned int u;
    int i;
    int success;
    int block_number;

    PORTAbits.RA0 = 1; // LEDs *... means we got here

    serial_is_monitor = 1;
    pic_monitor_latch = 0;

    setup();

    setup_serial(); // transmit and receive but global interrupts disabled

    printf("Alice 3 I/O PIC firmware, %s\n", PIC_FIRMWARE_VERSION_STRING);

    spi_config_for_sd();

    // Toggle SD power in case PIC was warm reset
    sdcard_reset();

    if(!sdcard_init()) {
        printf("PANIC: failed to start access to SD card as SPI\n");
        panic();
    }
    printf("SD Card interface is initialized for SPI\n");

    if(0) test_sd_card();

    setup_slave_port();

    setup_keyboard();

    enable_interrupts();

    printf("* ");

    for(;;) {

        unsigned char c;

        di();
        isempty = mon_queue_isempty();
        ei();

        if(!isempty) {
            di();
            c = mon_dequeue();
            ei();
            if(serial_is_monitor)
                process_local_key(c);
            else {
                if(pic_monitor_latch == 0 && c == 1)
                    pic_monitor_latch = 1;
                else if(pic_monitor_latch != 0 && c == 2) {
                    pic_monitor_latch = 0;
                    serial_is_monitor = 1;
                    printf("Serial input returned to monitor\n");
                } else {
                    pic_monitor_latch = 0;
                    console_enqueue_key(c);
                }
            }
        }

        if(command_length > 0) {
            unsigned char command_byte = command_bytes[0];

            PORTCbits.RC2 = 1; // debug LED: RC2 on means receipt in progress

            if((command_byte < PIC_CMD_MIN) || (command_byte > PIC_CMD_MAX)) {

                if(debug >= DEBUG_ERRORS) printf("ERROR: Unknown PIC command 0x%02X received\n", command_byte);
                command_clear();

            } else {

                if(command_length == PIC_command_lengths[command_byte]) {
                    command_request = command_byte;
                    PORTCbits.RC2 = 0; // debug LED: RC2 off means receipt completed
                }
            }
        }

        CLRWDT(); // XXX Why?  SWDTEN and WDTEN are 0!
        PORTAbits.RA1 = 1; // LEDs **.. means we got here
        if(command_request != PIC_CMD_NONE) {
            PORTAbits.RA2 = 1; // LEDs ***. means we got here
            response_start();
            switch(command_request) {
                case PIC_CMD_READ:
                case PIC_CMD_READ_SUM:
                {
                    unsigned int disk = command_bytes[1];
                    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
                    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
                    unsigned int block_number = disk * blocks_per_disk + (track * sectors_per_track + sector) / sectors_per_block;
                    unsigned int sector_byte_offset = 128 * ((track * sectors_per_track + sector) % sectors_per_block);
                    PORTAbits.RA3 = 1; // LEDs **** means we got here

                    if(debug >= DEBUG_EVENTS) printf("read disk %d, sector %d, track %d -> block %d, offset %d\n", disk, sector, track, block_number, sector_byte_offset);

                    if(disk > 3) { 
                        if(debug >= DEBUG_WARNINGS) printf("asked for disk out of range\n");
                        response_append(PIC_FAILURE);
                        break;
                    }

                    if(previous_block == block_number) {
                        if(debug >= DEBUG_DATA) printf("Block already in cache.\n");
                    } else {
                        if(!sdcard_readblock(block_number, block_buffer)) {
                            if(debug >= DEBUG_WARNINGS) printf("some kind of block read failure\n");
                            response_append(PIC_FAILURE);
                            break;
                        }
                        if(debug >= DEBUG_DATA) printf("New cached block\n");
                        if(debug >= DEBUG_ALL) dump_buffer_hex(4, block_buffer, BLOCK_SIZE);
                        previous_block = block_number;
                    }

                    response_append(PIC_SUCCESS);

                    for(u = 0; u < sector_size; u++)
                        response_append(block_buffer[sector_byte_offset + u]);

                    if(command_request == PIC_CMD_READ_SUM) {
                        unsigned short sum = 0;

                        for(u = 0; u < sector_size; u++)
                            sum += block_buffer[sector_byte_offset + u];

                        response_append(sum & 0xff);
                        response_append((sum >> 8) & 0xff);
                        if(debug >= DEBUG_ALL) printf("checksum calculated as %u: 0x%02X then 0x%02X\n", sum, sum & 0xff, (sum >> 8) & 0xff);
                    }
                    break;
                }

                case PIC_CMD_WRITE:
                case PIC_CMD_WRITE_SUM:
                {
                    unsigned int disk = command_bytes[1];
                    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
                    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
                    unsigned int block_number = disk * blocks_per_disk + (track * sectors_per_track + sector) / sectors_per_block;
                    unsigned int sector_byte_offset = 128 * ((track * sectors_per_track + sector) % sectors_per_block);

                    if(debug >= DEBUG_EVENTS) printf("write disk %d, sector %d, track %d -> block %d, offset %d\n", disk, sector, track, block_number, sector_byte_offset);

                    if(disk > 3) { 
                        if(debug >= DEBUG_WARNINGS) printf("asked for disk out of range\n");
                        response_append(PIC_FAILURE);
                        break;
                    }

                    if(command_request == PIC_CMD_WRITE_SUM) {
                        unsigned short sum = 0;
                        unsigned char offset = 6;
                        for(u = 0; u < sector_size; u++)
                            sum += command_bytes[offset + u];
                        unsigned short theirs = command_bytes[134] | (command_bytes[135] << 8);
                        if(sum != theirs) {
                            if(debug) printf("PIC_CMD_WRITE_SUM checksum does not match\n");
                            // XXX retry?
                            response_append(PIC_FAILURE);
                            break;
                        }
                    }

                    if(previous_block == block_number) {
                        if(debug >= DEBUG_DATA) printf("Block already in cache.\n");
                    } else {
                        if(!sdcard_readblock(block_number, block_buffer)) {
                            if(debug >= DEBUG_WARNINGS) printf("some kind of block read failure\n");
                            response_append(PIC_FAILURE);
                            break;
                        }
                        if(debug >= DEBUG_DATA) printf("New cached block\n");
                        previous_block = block_number;
                    }

                    for(u = 0; u < sector_size; u++)
                        block_buffer[sector_byte_offset + u] = command_bytes[6 + u];
                    if(!sdcard_writeblock(block_number, block_buffer)) {
                        printf("some kind of block write failure\n");
                        response_append(PIC_FAILURE);
                        break;
                    }

                    response_append(PIC_SUCCESS);

                    break;
                }

                case PIC_CMD_SEROUT: {
                    send_serial(command_bytes[1]);
                    response_append(PIC_SUCCESS);
                    break;
                }

                case PIC_CMD_CONST: {
                    if(debug >= DEBUG_EVENTS)printf("CONST\n");
                    di();
                    isempty = con_queue_isempty();
                    ei();
                    if(!isempty)
                        response_append(PIC_READY);
                    else
                        response_append(PIC_NOT_READY);
                    break;
                }

                case PIC_CMD_CONIN: {
                    unsigned char c;
                    if(debug >= DEBUG_EVENTS) printf("CONIN\n");
                    di();
                    isempty = con_queue_isempty();
                    ei();
                    if(!isempty) {
                        di();
                        c = con_dequeue();
                        ei();
                        response_append(PIC_SUCCESS);
                        response_append(c);
                    } else {
                        printf("Hm, char wasn't actually ready at CONIN\n");
                        response_append(PIC_SUCCESS);
                        response_append(0);
                    }
                    break;
                }

                default: {
                    printf("unexpected command 0x%02X from host!\n", command_request);
                    break;
                }
            }

            if(response_staging_length > 0) {
                if(debug >= DEBUG_DATA) printf("will respond with %d\n", response_staging_length);
                di(); // critical section
                command_clear();
                response_finish();
                was_response_waiting = 1;
                ei(); // end critical section
                PORTCbits.RC1 = 1; // debug LED: RC1 on means transmission in progress
            }
        }

        {
            di();
            isempty = kbd_queue_isempty();
            ei();
            if(!isempty) {
                unsigned char kb = kbd_dequeue();
                if(dump_keyboard_data)
                    printf("keyboard scan code: %02X\n", kb);
                kbd_process_byte(kb);
            }
        }

        if(console_overflowed) {
            if(debug >= DEBUG_WARNINGS) printf("WARNING: Console input queue overflow\n");
            console_overflowed = 0;
        }

        if(keyboard_overflowed) {
            if(debug >= DEBUG_WARNINGS) printf("WARNING: Keyboard data queue overflow\n");
            keyboard_overflowed = 0;
        }

        if(TRISEbits.IBOV)  {
            if(debug >= DEBUG_ERRORS) printf("ERROR: Missed a write from Z-80\n");
            TRISEbits.IBOV = 0;
        }

        if(was_response_waiting && !response_waiting) {
            if(debug >= DEBUG_EVENTS) printf("response packet was read\n");
            was_response_waiting = 0;
            PORTCbits.RC1 = 0; // debug LED: RC1 off means transmission completed
        }

        tmp = pspif_but_rd_asserted_count;
        if(bad_read_count != tmp) {
            if(debug >= DEBUG_WARNINGS) printf("%d PSP interrupts when RD was still asserted.\n", tmp - bad_read_count);
            bad_read_count = tmp;
        }
    }

    // should not reach
    panic();
}

#asm
	psect	temp
blarg:
	; opt stack 0
	; dw	65535	; assembler added errata NOP
	ds	1

#endasm
