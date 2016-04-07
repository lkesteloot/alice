#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <stm32f4xx_hal.h>

#include "ff.h"

#include "defs.h"
#include "delay.h"
#include "leds.h"
#include "byte_queue.h"
#include "gpio_helpers.h"
#include "utility.h"
#include "crc7.h"
#include "logprintf.h"

#include "monitor_queue.h"
#include "console_queue.h"
#include "uart.h"
#include "sd_spi.h"
#include "video.h"
#include "keyboard.h"
#include "reset_button.h"
#include "bus_signals.h"
#include "bus_aggregate.h"
#include "io_service.h"

static int gDumpKeyboardData = 0;

void panic_worse()
{
    LED_set_panic(1);
    for(;;);
}

void panic(void)
{
    static int entered = 0;

    LED_set_panic(1);

    int pin = 0;
    for(;;) {
        if(!entered) {
            // SERIAL_flush() can itself panic(), so stop reentry here
            entered = 1;
            SERIAL_flush();
            entered = 0;
        }

        LED_set_panic(pin);
        pin = pin ? 0 : 1;
        delay_ms(100);
    }
}

//----------------------------------------------------------------------------
// System Initialization Goop

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16; // Divide HSE by this
  RCC_OscInitStruct.PLL.PLLN = 336; // Then multiply by this
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // Then divide by this
  RCC_OscInitStruct.PLL.PLLQ = 7; // Divide by this for SD, USB OTG FS, and some other peripherals
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    panic();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4; // APB1 will be 42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; // APB2 will be 84MHz
  // grantham - 5 cycles for 168MHz is stated in Table 10 in the STM32F4 reference manual
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    panic();
  }
}

void system_init()
{
    HAL_Init();

    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    delay_init();
}

int gInRESETBeforeMastering = 0;

void BUS_mastering_start()
{
    gInRESETBeforeMastering = !HAL_GPIO_ReadPin(BUS_RESET_PORT, BUS_RESET_PIN_MASK);
    BUS_acquire_bus(gInRESETBeforeMastering);

    BUS_set_ADDRESS(0);
    BUS_set_ADDRESS_as_output();

    HAL_NVIC_DisableIRQ(EXTI1_IRQn); // Stop /RD from interrupting
    HAL_NVIC_DisableIRQ(EXTI2_IRQn); // Stop /WR from interrupting

    set_GPIO_iotype(BUS_IORQ_PORT, BUS_IORQ_PIN, GPIO_MODE_OUTPUT_PP);
    set_GPIO_iotype(BUS_MREQ_PORT, BUS_MREQ_PIN, GPIO_MODE_OUTPUT_PP);
    set_GPIO_iotype(BUS_RD_PORT, BUS_RD_PIN, GPIO_MODE_OUTPUT_PP);
    set_GPIO_iotype(BUS_WR_PORT, BUS_WR_PIN, GPIO_MODE_OUTPUT_PP);
}

void BUS_mastering_finish()
{
    set_GPIO_iotype(BUS_WR_PORT, BUS_WR_PIN, GPIO_MODE_INPUT);
    set_GPIO_iotype(BUS_RD_PORT, BUS_RD_PIN, GPIO_MODE_INPUT);
    set_GPIO_iotype(BUS_MREQ_PORT, BUS_MREQ_PIN, GPIO_MODE_INPUT);
    set_GPIO_iotype(BUS_IORQ_PORT, BUS_IORQ_PIN, GPIO_MODE_INPUT);

    HAL_NVIC_EnableIRQ(EXTI2_IRQn); // /WR will interrupt
    HAL_NVIC_EnableIRQ(EXTI1_IRQn); // /RD will interrupt

    BUS_set_ADDRESS_as_input();

    BUS_release_bus(gInRESETBeforeMastering);
}


//----------------------------------------------------------------------------
// System bus high-level operations

// Caller has to guarantee A and D access will not collide with
// another peripheral; basically either Z80 BUSRQ or RESET
void BUS_read_memory_block(unsigned int a, unsigned int l, unsigned char *b)
{
    if((ALICE3_VERSION == ALICE3_V3) && ALICE3_V3_ARM_IS_RAM) {

        for(unsigned int u = a; u < a + l; u++)
            b[u - a] = ccmram_get(shuffle_address(u));

    } else {

        BUS_mastering_start();
        BUS_set_DATA_as_input();

        for(unsigned int u = a; u < a + l; u++)
            b[u - a] = BUS_read_memory_byte(u);

        BUS_mastering_finish();
    }
}

// Guarantees no collisions through BUSRQ
// another peripheral; basically either Z80 BUSRQ or RESET
void BUS_write_IO(int io, unsigned char byte)
{
    BUS_mastering_start();
    BUS_set_DATA(0);
    BUS_set_DATA_as_output();

    BUS_write_io_byte(io, byte);

    BUS_set_DATA_as_input();
    BUS_mastering_finish();
}

#define ALICE3_SRAM_SIZE (256 * 1024)

int BUS_check_RAM()
{
    int succeeded = 1;

    if((ALICE3_VERSION == ALICE3_V3) && !ALICE3_V3_ARM_IS_RAM) {
        int t;

        BUS_mastering_start();
        // XXX verified by printf BUSAK is low here & RESET high & HALT high

        BUS_set_DATA(0);
        BUS_set_DATA_as_output();

        t = 0;
        for(unsigned int a = 0; a < ALICE3_SRAM_SIZE; a++) {
            BUS_write_memory_byte(a, t);
            t = (t + 1) % 251;
        }

        BUS_set_DATA_as_input();

        t = 0;
        for(unsigned int a = 0; a < ALICE3_SRAM_SIZE; a++) {
            unsigned char q = BUS_read_memory_byte(a);
            if(q != t) {
                printf("BUS_check_RAM : expected 0x%02X byte at RAM address 0x%X, read 0x%02X\n", t, a, q);
                succeeded = 0;
                break;
            }
            t = (t + 1) % 251;
        }

        BUS_mastering_finish();
    }

    return succeeded;
}


/*--------------------------------------------------------------------------*/
// VIDEO board operations

#define VIDEO_BOARD_OUTPUT_ADDR   0x80
#define VIDEO_BOARD_CONTROL_ADDR   0x81
#define VIDEO_BOARD_START_CLOCK   0x01

void VIDEO_output_string(char *c, int inverse)
{
    while(*c) {
        if(*c == '\n') {
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, '\r');
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, *c);
        } else
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, *c + (inverse ? 128 : 0));
        c++;
    }
}

void VIDEO_start_clock()
{
    BUS_mastering_start();
    BUS_set_DATA(0);
    BUS_set_DATA_as_output();

    BUS_write_io_byte(VIDEO_BOARD_OUTPUT_ADDR, VIDEO_BOARD_START_CLOCK);

    BUS_set_DATA_as_input();
    BUS_mastering_finish();
}


//----------------------------------------------------------------------------
// Alice 3 ROM (boot) image

unsigned char *gZ80BootImage;
unsigned int gZ80BootImageLength;

// Caller has to guarantee A and D access will not collide with
// another peripheral; basically either Z80 BUSRQ or RESET
int BUS_write_ROM_image(unsigned char *romimage_bytes, unsigned int romimage_length)
{
    int succeeded = 1;

    if(romimage_bytes == NULL) {
        printf("BUS_write_ROM_image : Z80 boot image is undefined (gZ80BootImage == NULL)\n");
        return 0;
    }

    if((ALICE3_VERSION == ALICE3_V3) && ALICE3_V3_ARM_IS_RAM) {

        for(unsigned int a = 0; a < romimage_length; a++)
            ccmram_set(shuffle_address(a), romimage_bytes[a]);

        for(unsigned int a = 0; a < romimage_length; a++) {
            unsigned char t = ccmram_get(shuffle_address(a));
            if(t != romimage_bytes[a]) {
                printf("BUS_write_ROM_image ccmram: expected 0x%02X byte at RAM address 0x%04X, read 0x%02X\n", romimage_bytes[a], a, t);
                succeeded = 0;
                break;
            }
        }

    } else {

        BUS_mastering_start();

        BUS_set_DATA(0);
        BUS_set_DATA_as_output();

        for(unsigned int a = 0; a < romimage_length; a++)
            BUS_write_memory_byte(a, romimage_bytes[a]);

        BUS_set_DATA_as_input();

        for(unsigned int a = 0; a < romimage_length; a++) {
            unsigned char t = BUS_read_memory_byte(a);
            if(t != romimage_bytes[a]) {
                printf("BUS_write_ROM_image bus : expected 0x%02X byte at RAM address 0x%04X, read 0x%02X\n", romimage_bytes[a], a, t);
                succeeded = 0;
                // break;
            }
        }

        BUS_mastering_finish();

    }

    return succeeded;
}


//----------------------------------------------------------------------------
// reset the Z80 and restore to initial booting state

void Z80_reset()
{
    BUS_reset_start();
    VIDEO_start_clock();

    IOSERVICE_clear_response();
    IOSERVICE_clear_command();

    BUS_write_ROM_image(gZ80BootImage, gZ80BootImageLength);

    BUS_reset_finish();
}

//----------------------------------------------------------------------------
// stdio

#define OUTPUT_TO_SERIAL        0x01
#define OUTPUT_TO_VIDEO         0x02

int gOutputDevices = OUTPUT_TO_SERIAL;

void __io_putchar( char c )
{
    if(gOutputDevices & OUTPUT_TO_SERIAL)
        SERIAL_enqueue_one_char(c);
    if(gOutputDevices & OUTPUT_TO_VIDEO) {
        if(c == '\n') // XXX
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, '\r');
        BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, c);
    }
}

void errorchar(char c)
{
    SERIAL_enqueue_one_char(c);
    if(c == '\n') // XXX
        BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, '\r');
    BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, c);
}

void errorchar_flush()
{
    SERIAL_flush();
}

//----------------------------------------------------------------------------
// CP/M 8MB Disk definitions


#define SECTORS_PER_BLOCK 4
#define SECTORS_PER_TRACK 64
#define TRACKS_PER_DISK 1024
#define SECTOR_SIZE 128
/* disk is 8MB, so 16384 512-byte blocks per disk */
#define BLOCKS_PER_DISK 16384

FATFS gFATVolume;

#define DISK_IMAGE_MAX 8
char gDiskImageFilenames[DISK_IMAGE_MAX][13];
FIL gDiskImageFiles[DISK_IMAGE_MAX];
int gDiskImageCount = 0;

int read_disk_image_list()
{
    FIL f;
    FRESULT result = f_open (&f, "disks.txt", FA_READ | FA_OPEN_EXISTING);

    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't open \"disks.txt\" for reading, FatFS result %d\n", result);
        return 0;
    }

    while(!f_eof(&f) && (gDiskImageCount < DISK_IMAGE_MAX)) {
        static char line[80];
        if(f_gets(line, sizeof(line), &f)) {
            if(line[strlen(line) - 1] != '\n') {
                logprintf(DEBUG_WARNINGS, "\"disks.txt\" unexpectedly contained a line longer than 80 characters:\n");
                logprintf(DEBUG_WARNINGS, "\"%s\"\n", line);
                return 0;
            }

            line[strlen(line) - 1] = '\0';

            if(strlen(line) > 12) {
                logprintf(DEBUG_WARNINGS, "Unexpectedly long disk image name \"%s\" ignored\n", line);
            } else {
                strcpy(gDiskImageFilenames[gDiskImageCount], line);
                gDiskImageCount++;
            }
        }
    }

    if(!f_eof(&f)) {
        logprintf(DEBUG_WARNINGS, "Maximum disk images reached (%d), further contents of \"disks.txt\" ignored\n", DISK_IMAGE_MAX);
    }

    f_close(&f);
    return 1;
}

int open_disk_images()
{
    int success = 1;
    int i;

    for(i = 0; i < gDiskImageCount; i++) {
        FRESULT result = f_open (&gDiskImageFiles[i], gDiskImageFilenames[i], FA_READ | FA_WRITE | FA_OPEN_EXISTING);

        if(result != FR_OK) {
            logprintf(DEBUG_ERRORS, "ERROR: couldn't open disk image \"%s\" for rw, FatFS result %d\n", gDiskImageFilenames[i], result);
            success = 0;
            break;
        }

        if(f_size(&gDiskImageFiles[i]) != BLOCKS_PER_DISK * SD_BLOCK_SIZE) {
            logprintf(DEBUG_ERRORS, "ERROR: expected disk image \"%s\" to be 8MB, is %d bytes\n", gDiskImageFilenames[i], f_size(&gDiskImageFiles[i]));
            success = 0;
            break;
        }
    }

    if(!success)
        for(int j = 0; j < i; j++)
            f_close(&gDiskImageFiles[j]);

    return success;
}

char gMonitorCommandLine[80];
unsigned char gMonitorCommandLineLength = 0;

void usage()
{
    printf("help       - this help message\n");
    printf("debug N    - set debug level\n");
    printf("buffers    - print summary of command and response buffers\n");
    printf("reset      - reset Z80 and clear communication buffers\n");
    printf("sdreset    - reset SD\n");
    printf("sdss {0|1} - 1: enable SD (SS=GND), 0: disable SD (SS=3.3V)\n");
    printf("spiwrite B0 [B1 ...]\n");
    printf("           - send bytes to SD SPI port and print read bytes\n");
    printf("spiread N  - read N bytes from SD SPI port\n");
    printf("dumpkbd    - toggle dumping keyboard\n");
    printf("pass       - pass monitor keys to Z80\n");
    printf("version    - print firmware build version\n");
    printf("read N     - read and dump block N\n");
    printf("low128     - dump low 128 bytes from RAM (resetting Z80!!)\n");
    printf("bus        - print bus signals\n");
    printf("panic      - force panic\n");
    printf("flashinfo  - force flashing the info LED\n");
}

#define IOBOARD_FIRMWARE_VERSION_STRING XSTR(IOBOARD_FIRMWARE_VERSION)

volatile unsigned char gSerialInputToMonitor = 1;

void process_local_key(unsigned char c)
{
    // XXX make this table driven, break into lots smaller functions
    if(c == '\r' || c == '\n') {
        putchar('\n');
        gMonitorCommandLine[gMonitorCommandLineLength] = 0;

        if((strcmp(gMonitorCommandLine, "help") == 0) ||
           (strcmp(gMonitorCommandLine, "h") == 0) ||
           (strcmp(gMonitorCommandLine, "?") == 0)) {

            usage();

        } else if(strcmp(gMonitorCommandLine, "sdreset") == 0) {

            printf("Resetting SD card...\n");

            if(!SDCARD_init())
                printf("Failed to start access to SD card as SPI\n");

        } else if(strcmp(gMonitorCommandLine, "dumpkbd") == 0) {

            gDumpKeyboardData = !gDumpKeyboardData;
            if(gDumpKeyboardData)
                printf("Dumping keyboard data...\n");
            else
                printf("Not dumping keyboard data...\n");

        } else if(strcmp(gMonitorCommandLine, "flashinfo") == 0) {

            for(int i = 0; i < 8; i++) {
                LED_set_info(1);
                delay_ms(125);
                LED_set_info(0);
                delay_ms(125);
            }

        } else if(strcmp(gMonitorCommandLine, "panic") == 0) {

            printf("panicking now\n");
            panic();

        } else if(strcmp(gMonitorCommandLine, "bus") == 0) {

            printf("RESET %s\n", HAL_GPIO_ReadPin(BUS_RESET_PORT, BUS_RESET_PIN_MASK) ? "high" : "low");
            printf("HALT %s\n", HAL_GPIO_ReadPin(BUS_HALT_PORT, BUS_HALT_PIN_MASK) ? "high" : "low");
            printf("IORQ %s\n", HAL_GPIO_ReadPin(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK) ? "high" : "low");
            printf("MREQ %s\n", HAL_GPIO_ReadPin(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK) ? "high" : "low");
            printf("RD %s\n", HAL_GPIO_ReadPin(BUS_RD_PORT, BUS_RD_PIN_MASK) ? "high" : "low");
            printf("WR %s\n", HAL_GPIO_ReadPin(BUS_WR_PORT, BUS_WR_PIN_MASK) ? "high" : "low");
            printf("BUSRQ %s\n", HAL_GPIO_ReadPin(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK) ? "high" : "low");
            printf("BUSAK %s\n", HAL_GPIO_ReadPin(BUS_BUSAK_PORT, BUS_BUSAK_PIN_MASK) ? "high" : "low");

        } else if(strncmp(gMonitorCommandLine, "chaff ", 6) == 0) {

            char *p = gMonitorCommandLine + 5;
            while(*p == ' ')
                p++;
            int addr = strtol(p, NULL, 0);

            BUS_mastering_start();
            BUS_set_DATA_as_input();
            BUS_set_ADDRESS(addr);
            set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_ACTIVE);
            delay_us(1);
            set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_ACTIVE);
            delay_us(1);

        } else if(strcmp(gMonitorCommandLine, "low128") == 0) {

            BUS_reset_start();

            static unsigned char buf[128];
            BUS_read_memory_block(0, 128, buf);

            printf("low 128 bytes of RAM:\n");
            dump_buffer_hex(4, buf, sizeof(buf));

            // BUS_reset_finish();

        } else if(strcmp(gMonitorCommandLine, "reset") == 0) {

            printf("Resetting Z-80 and communication buffers...\n");
            Z80_reset();

        } else if(strcmp(gMonitorCommandLine, "pass") == 0) {

            gSerialInputToMonitor = 0;
            printf("Press CTRL-A then CTRL-B to return to monitor.\n");

        } else if(strcmp(gMonitorCommandLine, "version") == 0) {

            printf("%s\n", IOBOARD_FIRMWARE_VERSION_STRING);

        } else if(strncmp(gMonitorCommandLine, "debug ", 6) == 0) {

            char *p = gMonitorCommandLine + 5;
            while(*p == ' ')
                p++;
            gDebugLevel = strtol(p, NULL, 0);
            printf("Debug level set to %d\n", gDebugLevel);

        } else if(strncmp(gMonitorCommandLine, "read ", 5) == 0) {

            static unsigned char buffer[SD_BLOCK_SIZE];
            char *p = gMonitorCommandLine + 4;
            while(*p == ' ')
                p++;
            int n = strtol(p, NULL, 0);
            if(SDCARD_readblock(n, buffer)) {
                dump_buffer_hex(4, buffer, sizeof(buffer));
            }

        } else {

            printf("Unknown command.\n");
            usage();
        }

        printf("* ");
        gMonitorCommandLineLength = 0;

    } else {

        if(c == 127 || c == '\b') {
            if(gMonitorCommandLineLength > 0) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                gMonitorCommandLineLength--;
            } else {
                bell();
            }
        } else {
            if(gMonitorCommandLineLength < sizeof(gMonitorCommandLine) - 1) {
                putchar(c);
                gMonitorCommandLine[gMonitorCommandLineLength++] = c;
            } else {
                bell();
            }
        }
    // TODO - upload data, write block
    }
}

#define IOBOARD_SUCCESS 0x01
#define IOBOARD_READY 0x01
#define IOBOARD_FAILURE 0xFF
#define IOBOARD_NOT_READY 0xFF

#define IOBOARD_CMD_NONE  0x00
#define IOBOARD_CMD_MIN  0x01
#define IOBOARD_CMD_READ  0x01
#define IOBOARD_CMD_WRITE 0x02
#define IOBOARD_CMD_CONST 0x03
#define IOBOARD_CMD_CONIN 0x04
#define IOBOARD_CMD_SEROUT 0x05
#define IOBOARD_CMD_READ_SUM 0x06
#define IOBOARD_CMD_WRITE_SUM 0x07
#define IOBOARD_CMD_MAX 0x07

void process_command_read(unsigned char command_request, volatile unsigned char *command_bytes)
{
    static unsigned char buffer[128];
    unsigned int disk = command_bytes[1];
    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
    unsigned int lba_sector = track * SECTORS_PER_TRACK + sector;

    logprintf(DEBUG_EVENTS, "read disk %d, sector %d, track %d, lba sector %d\n", disk, sector, track, lba_sector);

    if(disk > gDiskImageCount) { 
        logprintf(DEBUG_WARNINGS, "asked for disk out of range\n");
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }

    FRESULT result = f_lseek(&gDiskImageFiles[disk], lba_sector * SECTOR_SIZE);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't seek to sector %d, track %d in \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }

    UINT wasread;
    result = f_read(&gDiskImageFiles[disk], buffer, SECTOR_SIZE, &wasread);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't read sector %d, track %d from \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }
    if(wasread != SECTOR_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to read %lu bytes from sector %d, track %d of \"%s\", only read %ld\n", SECTOR_SIZE, gDiskImageFilenames[disk], sector, track, wasread);
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }
    if(gDebugLevel >= DEBUG_ALL) dump_buffer_hex(4, buffer, SECTOR_SIZE);

    IOSERVICE_append_response_byte(IOBOARD_SUCCESS);

    for(unsigned int u = 0; u < SECTOR_SIZE; u++)
        IOSERVICE_append_response_byte(buffer[u]);

    if(command_request == IOBOARD_CMD_READ_SUM) {
        unsigned short sum = 0;

        for(unsigned int u = 0; u < SECTOR_SIZE; u++)
            sum += buffer[u];

        IOSERVICE_append_response_byte(sum & 0xff);
        IOSERVICE_append_response_byte((sum >> 8) & 0xff);
        logprintf(DEBUG_ALL, "checksum calculated as %u: 0x%02X then 0x%02X\n", sum, sum & 0xff, (sum >> 8) & 0xff);
    }
}

void process_command_write(unsigned char command_request, volatile unsigned char *command_bytes)
{
    unsigned int disk = command_bytes[1];
    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
    unsigned int lba_sector = track * SECTORS_PER_TRACK + sector;

    logprintf(DEBUG_EVENTS, "write disk %d, sector %d, track %d, lba sector %d\n", disk, sector, track, lba_sector);

    if(disk > gDiskImageCount) { 
        logprintf(DEBUG_WARNINGS, "asked for disk out of range\n");
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }

    if(command_request == IOBOARD_CMD_WRITE_SUM) {
        unsigned short sum = 0;
        unsigned char offset = 6;
        for(unsigned int u = 0; u < SECTOR_SIZE; u++)
            sum += command_bytes[offset + u];
        unsigned short theirs = command_bytes[134] | (command_bytes[135] << 8);
        if(sum != theirs) {
            logprintf(DEBUG_WARNINGS, "WARNING: IOBOARD_CMD_WRITE_SUM checksum does not match\n");
            // XXX retry?
            IOSERVICE_append_response_byte(IOBOARD_FAILURE);
            return;
        }
    }

    FRESULT result = f_lseek(&gDiskImageFiles[disk], lba_sector * SECTOR_SIZE);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't seek to sector %d, track %d in \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        panic();
    }

    UINT waswritten;
    result = f_write(&gDiskImageFiles[disk], (unsigned char*)(command_bytes + 6), SECTOR_SIZE, &waswritten);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't write to sector %d, track %d in \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }
    if(waswritten != SECTOR_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to write %lu bytes to sector %d, track %d of \"%s\", only read %ld\n", SECTOR_SIZE, gDiskImageFilenames[disk], sector, track, waswritten);
        IOSERVICE_append_response_byte(IOBOARD_FAILURE);
        return;
    }
    // XXX Should probably set this up as a timeout, so multiple
    // sectors writes can take advantage of single block
    f_sync(&gDiskImageFiles[disk]);

    IOSERVICE_append_response_byte(IOBOARD_SUCCESS);
}

// Element 0 is 1 here to force stoppage on receiving a bad command
const unsigned char command_lengths[8] = {1, 6, 134, 1, 1, 2, 6, 136};

volatile int gResponseWasWaiting = 0;

void check_and_process_command()
{
    unsigned char isEmpty;

    volatile unsigned char *bytes;
    int length = IOSERVICE_get_command(&bytes);

    if(length == 0)
        return;

    unsigned char command = bytes[0];

    if((command < IOBOARD_CMD_MIN) || (command > IOBOARD_CMD_MAX)) {

        logprintf(DEBUG_ERRORS, "ERROR: Unknown command 0x%02X received\n", command);
        IOSERVICE_clear_command();
        return;
    }

    if(length < command_lengths[command])
        return;

    logprintf(DEBUG_EVENTS, "complete command received.\n");
    if(length > command_lengths[command]) {
        logprintf(DEBUG_ERRORS, "ERROR: command buffer longer than expected for command.\n");
    }

    if(command == IOBOARD_CMD_NONE)
        return;

    IOSERVICE_start_response();
    LED_set_info(1);
    switch(command) {
        case IOBOARD_CMD_READ:
        case IOBOARD_CMD_READ_SUM:
            process_command_read(command, bytes);
            break;

        case IOBOARD_CMD_WRITE:
        case IOBOARD_CMD_WRITE_SUM:
            process_command_write(command, bytes);
            break;

        case IOBOARD_CMD_SEROUT: {
            putchar(bytes[1]);
            IOSERVICE_append_response_byte(IOBOARD_SUCCESS);
            break;
        }

        case IOBOARD_CMD_CONST: {
            logprintf(DEBUG_EVENTS, "CONST\n");
            isEmpty = queue_isempty(&con_queue.q);
            if(!isEmpty)
                IOSERVICE_append_response_byte(IOBOARD_READY);
            else
                IOSERVICE_append_response_byte(IOBOARD_NOT_READY);
            break;
        }

        case IOBOARD_CMD_CONIN: {
            unsigned char c;
            logprintf(DEBUG_EVENTS, "CONIN\n");
            isEmpty = queue_isempty(&con_queue.q);
            if(!isEmpty) {
                c = queue_deq(&con_queue.q);
                IOSERVICE_append_response_byte(IOBOARD_SUCCESS);
                IOSERVICE_append_response_byte(c);
            } else {
                printf("Hm, char wasn't actually ready at CONIN\n");
                IOSERVICE_append_response_byte(IOBOARD_SUCCESS);
                IOSERVICE_append_response_byte(0);
            }
            break;
        }

        default: {
            printf("unexpected command 0x%02X from z80!\n", command);
            break;
        }
    }
    LED_set_info(0);

    if(IOSERVICE_get_response_staging_length() > 0) {
        logprintf(DEBUG_DATA, "will respond with %d\n", IOSERVICE_get_response_staging_length());
        IOSERVICE_clear_command();
        disable_interrupts();
        IOSERVICE_finish_response();
        enable_interrupts();
        gResponseWasWaiting = 1;
    }
}

void check_and_process_HALT()
{
    int halted;
    static int was_halted = 0;

    halted = !HAL_GPIO_ReadPin(BUS_HALT_PORT, BUS_HALT_PIN_MASK);
    if(halted != was_halted) {
        gOutputDevices = OUTPUT_TO_VIDEO | OUTPUT_TO_SERIAL;
        if(halted) {
            logprintf(DEBUG_EVENTS, "Z80 has HALTed.\n");
        } else {
            logprintf(DEBUG_EVENTS, "Z80 has exited HALT state.\n");
        }
        gOutputDevices = OUTPUT_TO_SERIAL;
        was_halted = halted;
    }
}

void check_and_process_soft_reset()
{
    if(HAL_GPIO_ReadPin(RESET_BUTTON_PORT, RESET_BUTTON_PIN_MASK)) {
        while(HAL_GPIO_ReadPin(RESET_BUTTON_PORT, RESET_BUTTON_PIN_MASK));
        delay_ms(RESET_BUTTON_DELAY_MS);// software debounce

        Z80_reset();

        logprintf(DEBUG_EVENTS, "Z80 was reset\n");
    }
}

void check_exceptional_conditions()
{
    if(gConsoleOverflowed) {
        logprintf(DEBUG_WARNINGS, "WARNING: Console input queue overflow\n");
        gConsoleOverflowed = 0;
    }

    if(gKeyboardOverflowed) {
        logprintf(DEBUG_WARNINGS, "WARNING: Keyboard data queue overflow\n");
        gKeyboardOverflowed = 0;
    }

    if(gReadWasAlreadyInactive) {
        logprintf(DEBUG_WARNINGS, "A");
        gReadWasAlreadyInactive = 0;
    }

    if(gUnclaimedRead) {
        BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, 'R' + 128);
        logprintf(DEBUG_WARNINGS, "(%04X)", gUnclaimedReadAddress);
        gUnclaimedRead = 0;
    }

    if(gUnclaimedWrite) {
        BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, 'W' + 128);
        logprintf(DEBUG_WARNINGS, "(%04X,%02X)", gUnclaimedWriteAddress, gUnclaimedWriteData);
        gUnclaimedWrite = 0;
    }
}

void process_monitor_queue()
{
    unsigned char isEmpty = queue_isempty(&mon_queue.q);
    static unsigned char escapeBackToMonitor = 0;

    if(!isEmpty) {
        unsigned char c = queue_deq(&mon_queue.q);
        if(gSerialInputToMonitor)
            process_local_key(c);
        else {
            if(escapeBackToMonitor == 0 && c == 1)
                escapeBackToMonitor = 1;
            else if(escapeBackToMonitor != 0 && c == 2) {
                escapeBackToMonitor = 0;
                gSerialInputToMonitor = 1;
                printf("Serial input returned to monitor\n");
            } else {
                escapeBackToMonitor = 0;
                disable_interrupts();
                console_enqueue_key_unsafe(c);
                enable_interrupts();
            }
        }
    }
}

int read_bootrom()
{
    FIL f;
    FRESULT result = f_open (&f, "bootrom.bin", FA_READ | FA_OPEN_EXISTING);

    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't open \"bootrom.bin\" for reading, FatFS result %d\n", result);
        return 0;
    }

    gZ80BootImageLength = f_size(&f);

    gZ80BootImage = (unsigned char*)malloc(gZ80BootImageLength);
    if(gZ80BootImage == NULL) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't allocate %lu bytes to read Z80 boot image\n", gZ80BootImageLength);
        return 0;
    }

    UINT wasread;
    result = f_read(&f, gZ80BootImage, gZ80BootImageLength, &wasread);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't read from \"bootrom.bin\", FatFS result %d\n", result);
        free(gZ80BootImage);
        gZ80BootImage = NULL;
        return 0;
    }

    if(wasread != gZ80BootImageLength) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to read %lu bytes, only read %ld\n", gZ80BootImageLength, wasread);
        free(gZ80BootImage);
        gZ80BootImage = NULL;
        return 0;
    }

    f_close(&f);
    return 1;
}

const int gStandaloneARM = 0;

void uart_received(char c)
{
    queue_enq(&mon_queue.q, c);
}

void console_queue_init()
{
    queue_init(&con_queue.q, CON_QUEUE_CAPACITY);
}

int main()
{
    system_init();

    LED_init();
    LED_beat_heart();

    MON_init();
    console_queue_init();
    LED_beat_heart();

    setbuf(stdout, NULL);

    SERIAL_init(); // transmit and receive but global interrupts disabled
    LED_beat_heart();

    printf("\n\nAlice 3 I/O firmware, %s\n", IOBOARD_FIRMWARE_VERSION_STRING);
    printf("System core clock: %lu MHz\n", SystemCoreClock / 1000000);

    LED_beat_heart();
    SERIAL_flush();

    RESET_BUTTON_init();

    IOSERVICE_clear_command();
    IOSERVICE_clear_response();

    if(!gStandaloneARM) {
        VIDEO_init();
        VIDEO_wait();

	// Pulls Z80 RESET low (active), but Z80 may not actually
	// reset until some clock cycles have run.  So run clock below
	// before trying to access bus.  (As of Apr 6, 2016, the Prop runs
        // Z80 clock continuously...)

        BUS_init();

        VIDEO_start_clock();
        VIDEO_output_string("Alice 3 I/O firmware, " IOBOARD_FIRMWARE_VERSION_STRING "\r\n", 0);

        if((ALICE3_VERSION == ALICE3_V3) && !ALICE3_V3_ARM_IS_RAM) {
            printf("RAM check...\n");
            SERIAL_flush();
            if(!BUS_check_RAM()) {
                panic();
            }
            printf("RAM okay...\n");
            SERIAL_flush();
        }

    }

    // XXX debugging - print Propeller character set
    if(0)
        for(int i = 0; i < 256; i++) {
            if(i % 16 == 0) {
                char buf[16];
                sprintf(buf, "%02X (%3d) : ", i, i);
                VIDEO_output_string(buf, 0);
            }
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, i);
            if((i + 1) % 16 == 0) {
                char buf[16];
                sprintf(buf, "\n");
                VIDEO_output_string(buf, 0);
            }
        }

    SPI_config_for_sd();
    LED_beat_heart();

    if(!SDCARD_init())
        printf("Failed to start access to SD cardSPI!!\n");
    else 
        printf("Opened SD Card\n");
    LED_beat_heart();
    SERIAL_flush();

    FRESULT result = f_mount(&gFATVolume, "0:", 1);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: FATFS mount result is %d\n", result);
        panic();
    } else {
        printf("Mounted FATFS from SD card successfully.\n");
    }
    SERIAL_flush();

    int success = read_bootrom();
    if(!success) {
        panic();
    }
    LED_beat_heart();

    success = read_disk_image_list();
    if(!success) {
        panic();
    }
    if(0)
        for(int i = 0; i < gDiskImageCount; i++) {
            printf("disk %c: \"%s\"\n", 'A' + i, gDiskImageFilenames[i]);
        }
    success = open_disk_images();
    if(!success) {
        panic();
    }
    LED_beat_heart();

    if(0) {
        unsigned char block[128];
        UINT wasread;
        for(int i = 0; i < gDiskImageCount; i++) {
            result = f_read(&gDiskImageFiles[i], block, sizeof(block), &wasread);
            printf("\"%s\", read resulted in %d, got %d bytes:\n", gDiskImageFilenames[i], result, wasread);
            dump_buffer_hex(4, block, sizeof(block));
        }
    }

    KBD_init();
    LED_beat_heart();

    if(!gStandaloneARM) { // Can't have floating BUS in test mode

        BUS_reset_start();
        if(!BUS_write_ROM_image(gZ80BootImage, gZ80BootImageLength)) {
            // panic();
        }
        delay_ms(1); // XXX delay for at least 4 Z80 clock cycles, maybe 10us
        BUS_reset_finish();
    }

    printf("* ");
    SERIAL_flush();

    for(;;) {
        int key;

        SERIAL_try_to_transmit_buffers();
        LED_beat_heart();

        SERIAL_poll_continue();

        process_monitor_queue();

        check_and_process_command();

        key = KBD_process_queue(gDumpKeyboardData);
        if(key >= 0) {
            disable_interrupts();
            console_enqueue_key_unsafe(key);
            enable_interrupts();
        }

        check_exceptional_conditions();

        if(gResponseWasWaiting && IOSERVICE_is_response_empty()) {
            logprintf(DEBUG_EVENTS, "response packet was read\n");
            gResponseWasWaiting = 0;
        }

        check_and_process_soft_reset();

        check_and_process_HALT();
    }

    // should not reach
    panic();
}
