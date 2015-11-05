#ifdef SDCC_MODE

#include <pic18fregs.h>

#pragma config OSC=EC           // External clock
#pragma config WDT=OFF          // Watchdog timer off

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
    ClrWdt();
    WDTCON = 0;

    // debug LEDs
    TRISA = 0xF0;
}


/*--------------------------------------------------------------------------*/
/* USART - serial comms ----------------------------------------------------*/

int baud_rate_code = 0xf; // 19200 baud at 20 MHz, BRGH=0, 15 decimal

void configure_serial()
{
    // clear
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    PIR1bits.RCIF = 0;

    TRISCbits.TRISC6 = 0;       // TX is output
    TRISCbits.TRISC7 = 1;       // RX is input

    SPBRG = baud_rate_code;

    TXSTAbits.SYNC = 0;
    TXSTAbits.TXEN = 1;
    PIE1bits.TXIE = 0;
    RCSTAbits.SPEN = 1;
    PIE1bits.RCIE = 0; // 1;
    RCSTAbits.CREN = 1;
}

void send_serial(unsigned char b)
{
    while(!PIR1bits.TXIF);
    TXREG = b;
    ClrWdt(); // XXX Why do I need this?
}

/*--------------------------------------------------------------------------*/
/* SPI commands ------------------------------------------------------------*/

unsigned char spi_exchange(unsigned char b)
{
    SSPBUF = b;
    while(!SSPSTATbits.BF);
    ClrWdt(); // XXX Why do I need this?
    return SSPBUF;
}


/*--------------------------------------------------------------------------*/
/* SD-card-specific SPI commands -------------------------------------------*/

// Postcondition: SPI configured for SD, SS high (false)
void spi_config_for_sd()
{
    // Set up SPI for SD card
    // Set TMR2 to /16 pre, /1 comparator, source is Fosc/4,
    // 20Mhz / 4 / 16 / 5 / 2 = 31.25KHz,  Max out later

    T2CONbits.T2CKPS = 0b11;    // 1:16 prescale
    PR2 = 8;                    // TMR2 PR2 trigger on 8
    T2CONbits.TMR2ON = 1;       // enable timer 2

    // slave select for SD
    TRISBbits.TRISB7 = 0;       // B7 is /SS for SD - set to output
    PORTBbits.RB7 = 1;          // B7 is /SS for SD - disable

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

void spi_enable_sd()
{
    PORTBbits.RB7 = 0;          // B7 is /SS for SD - enable
}

void spi_disable_sd()
{
    PORTBbits.RB7 = 1;          // B7 is /SS for SD - disable
}


/*--------------------------------------------------------------------------*/
/* define putch() so sprintf goes to the LCD screen ------------------------*/

void putch(unsigned char byte)
{
    send_serial(byte);
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

int debug = 0;
int timeout_count = 100;

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
unsigned char sdcard_token_17_18_24 = 0xFE;

void spi_bulk(unsigned char *buffer, unsigned int nlen)
{
    int i;

    for(i = 0; i < nlen; i++) {
        buffer[i] = spi_exchange(buffer[i]);
        // printf("%d: 0x%02X\n", i, buffer[i]);
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
    unsigned char dummy;

    for(i = 0; i < nlen; i++) {
        buffer[i] = spi_exchange(0xff);
        // printf("%d: 0x%02X\n", i, buffer[i]);
    }
}

// response length must include initial R1, so 1 for CMD0
int send_sdcard_command(enum sdcard_command command, unsigned long parameter, unsigned char *response, int response_length)
{
    int count;
    int i;
    unsigned char command_buffer[6];
    command_buffer[0] = 0x40 | command;

    command_buffer[1] = (parameter >> 24) & 0xff;
    command_buffer[2] = (parameter >> 16) & 0xff;
    command_buffer[3] = (parameter >> 8) & 0xff;
    command_buffer[4] = (parameter >> 0) & 0xff;
    command_buffer[5] = ((crc7_generate_bytes(command_buffer, 5) & 0x7f) << 1) | 0x01;

    if(debug) printf("command constructed: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    spi_bulk(command_buffer, sizeof(command_buffer));
    if(debug) printf("returned in buffer: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    for(i = 0; i < response_length; i++)
        response[i] = 0xff;
    count = 0;
    do {
        if(count > timeout_count) {
            printf("send_sdcard_command: timed out waiting on response\n");
            return 0;
        }
        spi_bulk(response, 1);
        if(debug) printf("response 0x%02X\n", response[0]);
        count++;
    } while(response[0] & 0x80);

    if(response_length > 1) {
        spi_bulk(response + 1, response_length - 1);
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
    if(!send_sdcard_command(CMD0, 0, response, 8))
        return 0;
    if(response[0] != sdcard_response_IDLE) {
        printf("sdcard_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }

    /* interface condition */
    if(!send_sdcard_command(CMD8, 0x000001AA, response, 8))
        return 0;
    if(response[0] != sdcard_response_IDLE) {
        printf("sdcard_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }
    OCR = (((unsigned long)response[1]) << 24) | (((unsigned long)response[2]) << 16) | (((unsigned long)response[3]) << 8) | (((unsigned long)response[4]) << 0);
    printf("sdcard_init: OCR response is 0x%08lX\n", OCR);

    // should get CSD, CID, print information about them

    count = 0; 
    do {
        if(count > timeout_count) {
            printf("sdcard_init: timed out waiting on transition to ACMD41\n");
            return 0;
        }
        /* get operating condition */
        if(!send_sdcard_command(CMD55, 0x00000000, response, 8))
            return 0;
        if(response[0] != sdcard_response_IDLE) {
            printf("sdcard_init: not in IDLE mode for CMD55, response was 0x%02X\n", response[0]);
            return 0;
        }
        if(!send_sdcard_command(ACMD41, 0x40000000, response, 8))
            return 0;
        count++;
    } while(response[0] != sdcard_response_SUCCESS);
    if(debug) printf("returned from ACMD41: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        response[0], response[1], response[2], response[3],
        response[4], response[5], response[6], response[7]);

    return 1;
}

const unsigned int block_size = 512;

/* precondition: SDcard CS is low (active) */
int sdcard_readblock(unsigned int blocknum, unsigned char *block)
{
    unsigned int u;
    int count;
    unsigned char response[8];

    response[0] = 0xff;
    if(!send_sdcard_command(CMD17, blocknum, response, 1))
        return 0;
    if(response[0] != sdcard_response_SUCCESS) {
        printf("sdcard_readblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    response[0] = 0xff;
    count = 0;
    do {
        if(count > timeout_count) {
            printf("sdcard_readblock: timed out waiting on response\n");
            return 0;
        }
        spi_bulk(response, 1);
        if(debug) printf("readblock response 0x%02X\n", response[0]);
        count++;
    } while(response[0] != sdcard_token_17_18_24);

    spi_readn(block, block_size);
    response[0] = 0xff;
    response[1] = 0xff;
    spi_bulk(response, 2);
    if(debug) printf("CRC is 0x%02X%02X\n", response[0], response[1]);
    // discard CRC

    for(u = 0; u < sizeof(response); u++)
        response[u] = 0xff;
    spi_bulk(response, sizeof(response));
    if(debug) printf("trailing read: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        response[0], response[1], response[2], response[3],
        response[4], response[5], response[6], response[7]);

    return 1;
}

/* precondition: SDcard CS is low (active) */
int sdcard_writeblock(unsigned int blocknum, unsigned char *block)
{
    int count;
    unsigned int u;
    unsigned char response[8];

    for(u = 0; u < sizeof(response); u++)
        response[u] = 0xff;
    if(!send_sdcard_command(CMD24, blocknum, response, 1))
        return 0;
    if(response[0] != sdcard_response_SUCCESS) {
        printf("sdcard_writeblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    response[0] = sdcard_token_17_18_24;
    spi_bulk(response, 1);

    spi_writen(block, block_size);

    // junk CRC
    for(u = 0; u < sizeof(response); u++)
        response[u] = 0xff;
    spi_bulk(response, 2);

    count = 0;
    do {
        if(count > timeout_count) {
            printf("sdcard_writeblock: timed out waiting on response\n");
            return 0;
        }
        spi_bulk(response, 1);
        if(debug) printf("writeblock response 0x%02X\n", response[0]);
        count++;
    } while(response[0] == 0);

    for(u = 0; u < sizeof(response); u++)
        response[u] = 0xff;
    spi_bulk(response, sizeof(response));
    if(debug) printf("trailing write: %02X %02X %02X %02X %02X %02X %02X %02X\n",
        response[0], response[1], response[2], response[3],
        response[4], response[5], response[6], response[7]);

    return 1;
}

int isprint(unsigned char a)
{
    return (a >= ' ') && (a <= '~');
}

void dump_buffer_hex(int indent, unsigned char *data, int size)
{
    int address = 0;

    while(size > 0) {
        int howmany = (size < 16) ? size : 16;

        printf("%*s0x%04X: ", indent, "", address);
        for(int i = 0; i < howmany; i++)
            printf("%02X ", data[i]);
        printf("\n");

        printf("%*s        ", indent, "");
        for(int i = 0; i < howmany; i++)
            printf(" %c ", isprint(data[i]) ? data[i] : '.');
        printf("\n");

        size -= howmany;
        data += howmany;
        address += howmany;
    }
}

unsigned char originalblock[512];
unsigned char testblock[512];

void main()
{
    unsigned int u;
    int i;
    int success;
    int block_number;

    setup();
    pause();

    // XXX MAX232 device
    configure_serial();
    printf("Hello RS-232.\n");

    // Set up PS/2 keyboard?
    // Prompt
    // Get string
    // Print string

    pause();

    // test SD card
    spi_config_for_sd();
    if(!sdcard_init()) {
        printf("failed to start access to SD card as SPI\n");
        goto stop;
    }
    printf("SD Card interface is initialized for SPI\n");

    block_number = 0;

    for(;;) {
        printf("operating on block %d\n", block_number);
        printf("Reading a block\n");
        if(!sdcard_readblock(block_number, originalblock)) {
            goto stop;
        }
        printf("Original block:\n");
        dump_buffer_hex(4, originalblock, 512);

        for(i = 0; i < 512; i++)
            testblock[i] = (originalblock[i] + 0x55) % 256;

        pause();
        if(!sdcard_writeblock(block_number, testblock)) {
            printf("Failed writeblock\n");
            goto stop;
        }
        printf("Wrote junk block\n");

        for(i = 0; i < 512; i++)
            testblock[i] = 0;
        pause();
        if(!sdcard_readblock(block_number, testblock)) {
            printf("Failed readblock\n");
            goto stop;
        }

        success = 1;
        for(i = 0; i < 512; i++)
            if(testblock[i] != (originalblock[i] + 0x55) % 256)
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of junk to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                testblock[0], testblock[1], testblock[2], testblock[3]);
        } else {
            printf("Verified junk block was written\n");
        }

        pause();
        if(!sdcard_writeblock(block_number, originalblock)) {
            printf("Failed writeblock\n");
            goto stop;
        }
        printf("Wrote original block\n");

        pause();
        if(!sdcard_readblock(block_number, testblock)) {
            printf("Failed readblock\n");
            goto stop;
        }

        success = 1;
        for(i = 0; i < 512; i++)
            if(originalblock[i] != testblock[i])
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of original to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                testblock[0], testblock[1], testblock[2], testblock[3]);
        } else {
            printf("Verified original block was written\n");
        }
        block_number ++;
    }
    spi_disable_sd();

    PORTA = 0x8;

stop:
    for(;;) {
        PORTA = 0x55;
        pause();

        PORTA = 0xaa;
        pause();
    }
}
