#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "sd_spi.h"
#include "delay.h"
#include "logprintf.h"
#include "utility.h"
#include "crc7.h"

#include <stm32f4xx_hal.h>

/*--------------------------------------------------------------------------*/
/* SD over SPI -------------------------------------------------------------*/

#define SPI_SS_PIN_MASK      GPIO_PIN_8
#define SPI_SS_PORT     GPIOA

SPI_HandleTypeDef gSPIHandle;

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable GPIO CK/TX/RX clocks */
    // __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Enable SPI clock */
    __HAL_RCC_SPI2_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/

    // Alternate function SPI2, high speed, push-pull
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;

    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_13;
    GPIO_InitStruct.Pull      = GPIO_NOPULL; // GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL; // GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL; // GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_SPI2_FORCE_RESET();
    __HAL_RCC_SPI2_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15);
}

void SPI_enable_sd()
{
    HAL_GPIO_WritePin(SPI_SS_PORT, SPI_SS_PIN_MASK, 0);     // SS true
    delay_ms(100);
}

void SPI_disable_sd()
{
    HAL_GPIO_WritePin(SPI_SS_PORT, SPI_SS_PIN_MASK, 1);     // SS false
    delay_ms(100);
}

// Postcondition: SPI configured for SD, SS high (false)
void SPI_config_for_sd()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    gSPIHandle.Instance               = SPI2;

    // SPI2 is APB1, which is 1/4 system clock, or at 168MHz, APB1 is
    // 42MHz.  Init should be at  100KHz - 400 KHz, 128 will be 328.124Khz,
    // 256 will be about 164.062KHz
    gSPIHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    gSPIHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    gSPIHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    gSPIHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    gSPIHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    gSPIHandle.Init.CRCPolynomial     = 7;
    gSPIHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    gSPIHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    gSPIHandle.Init.NSS               = SPI_NSS_SOFT;
    gSPIHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
    gSPIHandle.Init.Mode              = SPI_MODE_MASTER;

    if(HAL_SPI_Init(&gSPIHandle) != HAL_OK)
    {
        printf("failed to initialize SPI\n");
        panic();
    }

    // SD /CS card select, slave select /SS
    GPIO_InitStruct.Pin = SPI_SS_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // XXX change to match SPI rate?
    HAL_GPIO_Init(SPI_SS_PORT, &GPIO_InitStruct); 
    SPI_disable_sd();
}

void SPI_bulk_exchange(unsigned char *buffer, unsigned int nlen)
{
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("SPI_bulk_exchange write:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
    int result = HAL_SPI_TransmitReceive(&gSPIHandle, buffer, buffer, nlen, 1000);
    if(result != HAL_OK){
        printf("SPI_bulk_exchange: SPI error 0x%04X\n", result);
        panic();
    }
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("SPI_bulk_exchange read:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
}

void SPI_write(const unsigned char *buffer, unsigned int nlen)
{
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("SPI_write write:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
    int result = HAL_SPI_Transmit(&gSPIHandle, (unsigned char *)buffer, nlen, 1000);
    if(result != HAL_OK){
        logprintf(DEBUG_ERRORS, "SPI_write: SPI error 0x%04X\n", result);
        panic();
    }
}

void SPI_read(unsigned char *buffer, unsigned int nlen)
{
    memset(buffer, 0xff, nlen);
    int result = HAL_SPI_TransmitReceive(&gSPIHandle, buffer, buffer, nlen, 1000);
    if(result != HAL_OK){
        logprintf(DEBUG_ERRORS, "SPI_read: SPI error 0x%04X\n", result);
        panic();
    }
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("SPI_read read:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
}

/*--------------------------------------------------------------------------*/
/* SD card -----------------------------------------------------------------*/

int gSDCardTimeoutMillis = 1000;

// cribbed somewhat from http://elm-chan.org/docs/mmc/mmc_e.html
enum SDCardCommand {
    CMD0 = 0,    // init; go to idle state
    CMD8 = 8,    // send interface condition
    CMD17 = 17,  // read single block
    CMD24 = 24,  // write single block
    CMD55 = 55,  // prefix command for application command
    ACMD41 = 41, // application command to send operating condition
};
const unsigned char gSDCardResponseIDLE = 0x01;
const unsigned char gSDCardResponseSUCCESS = 0x00;
const unsigned char gSDCardResponseDATA_ACCEPTED = 0xE5;
const unsigned char gSDCardToken_17_18_24 = 0xFE;

// response length must include initial R1, so 1 for CMD0
int SDCARD_send_command(enum SDCardCommand command, unsigned long parameter, unsigned char *response, int response_length)
{
    unsigned char command_buffer[6];

    command_buffer[0] = 0xff;
    SPI_write(command_buffer, 1);

    command_buffer[0] = 0x40 | command;
    command_buffer[1] = (parameter >> 24) & 0xff;
    command_buffer[2] = (parameter >> 16) & 0xff;
    command_buffer[3] = (parameter >> 8) & 0xff;
    command_buffer[4] = (parameter >> 0) & 0xff;
    command_buffer[5] = ((crc7_generate_bytes(command_buffer, 5) & 0x7f) << 1) | 0x01;

    logprintf(DEBUG_DATA, "command constructed: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    SPI_bulk_exchange(command_buffer, sizeof(command_buffer));
    logprintf(DEBUG_ALL, "returned in buffer: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "SDCARD_send_command: timed out waiting on response\n");
            panic();
        }
        response[0] = 0xff;
        SPI_read(response, 1);
        logprintf(DEBUG_ALL, "response 0x%02X\n", response[0]);
    } while(response[0] & 0x80);

    if(response_length > 1) {
        SPI_read(response + 1, response_length - 1);
    }

    return 1;
}

// precondition: SD card CS is high (disabled)
// postcondition: SD card CS is low (enabled)
int SDCARD_init()
{
    unsigned char response[8];
    unsigned long OCR;

    /* CS false, 80 clk pulses (read 10 bytes) */
    unsigned char buffer[10];
    for(unsigned int u = 0; u < sizeof(buffer); u++)
        buffer[u] = 0xff;
    SPI_write(buffer, sizeof(buffer));

    delay_ms(100);
    SPI_enable_sd();
    /* interface init */
    if(!SDCARD_send_command(CMD0, 0, response, 1))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        logprintf(DEBUG_WARNINGS, "SDCARD_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }
    delay_ms(100);

    /* check voltage */
    if(!SDCARD_send_command(CMD8, 0x000001AA, response, 5))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        logprintf(DEBUG_WARNINGS, "SDCARD_init: failed to get OCR, response was 0x%02X\n", response[0]);
        return 0;
    }
    OCR = (((unsigned long)response[1]) << 24) | (((unsigned long)response[2]) << 16) | (((unsigned long)response[3]) << 8) | (((unsigned long)response[4]) << 0);
    logprintf(DEBUG_DATA, "SDCARD_init: OCR response is 0x%08lX\n", OCR);

    // should get CSD, CID, print information about them

    // Ask the card to initialize itself, and wait for it to get out of idle mode.
    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("SDCARD_init: timed out waiting on transition to ACMD41\n");
            return 0;
        }
        /* leading command to the ACMD41 command */
        if(!SDCARD_send_command(CMD55, 0x00000000, response, 1))
            return 0;
        if(response[0] != gSDCardResponseIDLE) {
            logprintf(DEBUG_WARNINGS, "SDCARD_init: not in IDLE mode for CMD55, response was 0x%02X\n", response[0]);
            return 0;
        }
        /* start initialization process, set HCS (high-capacity) */
        if(!SDCARD_send_command(ACMD41, 0x40000000, response, 1))
            return 0;
    } while(response[0] != gSDCardResponseSUCCESS);
    logprintf(DEBUG_ALL, "returned from ACMD41: %02X\n", response[0]);

    // SPI2 is APB1, which is 1/4 system clock, or at 168MHz, APB1 is
    // 42MHz.  After init, we should be able to set the clock as
    // high as 25MHz.  Baud scaler at 2 should be 21MHz.
    SPI2->CR1 = (SPI2->CR1 & ~SPI_CR1_BR) | SPI_BAUDRATEPRESCALER_2;

    return 1;
}

void dump_more_spi_bytes(const char *why)
{
    unsigned char response[8];
    SPI_read(response, sizeof(response));
    printf("trailing %s: %02X %02X %02X %02X %02X %02X %02X %02X\n", why,
        response[0], response[1], response[2], response[3],
        response[4], response[5], response[6], response[7]);
}

/* precondition: SDcard CS is low (active) */
int SDCARD_readblock(unsigned int blocknum, unsigned char *block)
{
    unsigned char response[8];

    // Send read block command.
    response[0] = 0xff;
    if(!SDCARD_send_command(CMD17, blocknum, response, 1))
        return 0;
    if(response[0] != gSDCardResponseSUCCESS) {
        logprintf(DEBUG_ERRORS, "SDCARD_readblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait for the data token.
    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "SDCARD_readblock: timed out waiting for data token\n");
            return 0;
        }
        SPI_read(response, 1);
        logprintf(DEBUG_ALL, "readblock response 0x%02X\n", response[0]);
    } while(response[0] != gSDCardToken_17_18_24);

    // Read data.
    SPI_read(block, SD_BLOCK_SIZE);

    // Read CRC
    SPI_read(response, 2);
    logprintf(DEBUG_DATA, "CRC is 0x%02X%02X\n", response[0], response[1]);

    unsigned short crc_theirs = response[0] * 256 + response[1];

    // calculate our version of CRC and compare
    unsigned short crc_ours = crc_itu_t(0, block, SD_BLOCK_SIZE);

    if(crc_theirs != crc_ours) {
        logprintf(DEBUG_ERRORS, "CRC mismatch (theirs %04X versus ours %04X, reporting failure)\n", crc_theirs, crc_ours);
        return 0;
    } else {
        logprintf(DEBUG_DATA, "CRC matches\n");
    }

    // Wait for DO to go high. I don't think we need to do this for block reads,
    // but I don't think it'll hurt.
    then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "SDCARD_readblock: timed out waiting on completion\n");
            return 0;
        }
        SPI_read(response, 1);
        logprintf(DEBUG_ALL, "readblock response 0x%02X\n", response[0]);
    } while(response[0] != 0xFF);

    if(gDebugLevel >= DEBUG_ALL) dump_more_spi_bytes("read completion");

    return 1;
}

/* precondition: SDcard CS is low (active) */
int SDCARD_writeblock(unsigned int blocknum, const unsigned char *block)
{
    int count;
    unsigned char response[8];

    // Send write block command.
    if(!SDCARD_send_command(CMD24, blocknum, response, 1))
        return 0;
    if(response[0] != gSDCardResponseSUCCESS) {
        logprintf(DEBUG_ERRORS, "SDCARD_writeblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }
    // XXX - elm-chan.org says I should be waiting >= 1byte here

    // Data token.
    response[0] = gSDCardToken_17_18_24;
    SPI_write(response, 1);

    // Send data.
    SPI_write(block, SD_BLOCK_SIZE);

    // junk CRC
    response[0] = 0xff;
    response[1] = 0xff;
    SPI_write(response, 2);

    // Get DATA_ACCEPTED response from WRITE
    SPI_read(response, 1);
    logprintf(DEBUG_DATA, "writeblock response 0x%02X\n", response[0]);
    if(response[0] != gSDCardResponseDATA_ACCEPTED) {
        logprintf(DEBUG_ERRORS, "SDCARD_writeblock: failed to respond with DATA_ACCEPTED, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait while busy (DO = low).
    int then = HAL_GetTick();
    count = 0;
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "SDCARD_writeblock: timed out waiting on completion\n");
            panic();
        }
        SPI_read(response, 1);
        logprintf(DEBUG_ALL, "writeblock completion 0x%02X\n", response[0]);
        count++;
    } while(response[0] != 0xFF);
    logprintf(DEBUG_DATA, "read %d SPI bytes waiting on write to complete.\n", count);

    if(gDebugLevel >= DEBUG_ALL) dump_more_spi_bytes("write completion");

    return 1;
}

