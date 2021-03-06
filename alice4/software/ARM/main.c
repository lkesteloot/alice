#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <stm32f4xx_hal.h>

#include "main.h"
#include "ff.h"

#include "defs.h"
#include "delay.h"
#include "leds.h"
#include "byte_queue.h"
#include "gpio_helpers.h"
#include "utility.h"
#include "logprintf.h"

#include "sd_spi.h"

#include "monitor_queue.h"
#include "uart.h"

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

/*--------------------------------------------------------------------------*/
/* USB ---------------------------------------------------------------------*/

#if 0
USBD_HandleTypeDef USBD_Device;

void USB_init()
{
    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    // Enable TIM peripherals Clock 
    TIMx_CLK_ENABLE();
  
    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    // Configure the NVIC for TIMx 
    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriority(TIMx_IRQn, 6, 0);
  
    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMx_IRQn);

    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    /* Init Device Library */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    /* Add CDC Interface Class */
    USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
    /* Start Device Process */
    USBD_Start(&USBD_Device);
    printf("%s, line %d\n", __FILE__, __LINE__); SERIAL_flush();
}

#endif

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
        // XXX on-screen video output
    }
}

void errorchar(char c)
{
    SERIAL_enqueue_one_char(c);
    // XXX on-screen video output
}

void errorchar_flush()
{
    SERIAL_flush();
}

char gMonitorCommandLine[80];
unsigned char gMonitorCommandLineLength = 0;

void usage()
{
    printf("help       - this help message\n");
    printf("debug N    - set debug level\n");
    printf("panic      - force panic\n");
}

int go = 0;

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

        } else if(strcmp(gMonitorCommandLine, "go") == 0) {

            MAIN();

        } else if(strcmp(gMonitorCommandLine, "panic") == 0) {

            printf("panicking now\n");
            panic();

        } else if(strncmp(gMonitorCommandLine, "debug ", 6) == 0) {

            char *p = gMonitorCommandLine + 5;
            while(*p == ' ')
                p++;
            gDebugLevel = strtol(p, NULL, 0);
            printf("Debug level set to %d\n", gDebugLevel);

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
    }
}

void process_monitor_queue()
{
    unsigned char isEmpty = queue_isempty(&mon_queue.q);

    if(!isEmpty) {
        unsigned char c = queue_deq(&mon_queue.q);
        process_local_key(c);
    }
}

void uart_received(char c)
{
    queue_enq(&mon_queue.q, c);
}

#define Var_BB(VarAddr, BitNumber) (((int)PERIPH_BB_BASE | (((int)VarAddr - (int)PERIPH_BASE) << 5) | ((BitNumber) << 2)))


//----------------------------------------------------------------------------
// libgl connection

FIL capture_file;

uint8_t receive_uint8()
{
}

#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];
uint8_t buffer_count = 0;

static void write_buffer()
{
    if(buffer_count == 0)
        return;

    UINT waswritten;
    FRESULT result = f_write(&capture_file, &buffer, BUFFER_SIZE, &waswritten);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't write buffer to capture file\n");
        panic();
    }
    if(waswritten != BUFFER_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to write %ld bytes, wrote %ld bytes\n", BUFFER_SIZE, waswritten);
        panic();
    }
    buffer_count = 0; 
}

void send_uint8(uint8_t b)
{
    LED_beat_heart();
#if 0
    buffer[buffer_count++] = b;

    if(buffer_count == BUFFER_SIZE) {
        SERIAL_enqueue_one_char('.'); SERIAL_flush();
        write_buffer();
    }
#endif
}

int count = 0;
void flush()
{
    printf("frame %d\n", count); SERIAL_flush();
    if(++count > 60) {
        write_buffer();
        f_close(&capture_file);
        f_mount(NULL, "0:", 1);
        printf("finished capturing\n"); SERIAL_flush();
        panic();
    }
}

void open_connection()
{
}


//----------------------------------------------------------------------------
// main

FATFS gFATVolume;

int main()
{
    system_init();

    LED_init();
    LED_beat_heart();

    MON_init();
    LED_beat_heart();

    setbuf(stdout, NULL);

    SERIAL_init(); // transmit and receive but global interrupts disabled
    LED_beat_heart();

    printf("\n\nAlice 4 CPU\n");
    printf("System core clock: %lu MHz\n", SystemCoreClock / 1000000);

    LED_beat_heart();
    SERIAL_flush();

    if(1) {
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

        result = f_open (&capture_file, "logo.cap", FA_WRITE | FA_CREATE_ALWAYS);

        if(result != FR_OK) {
            logprintf(DEBUG_ERRORS, "ERROR: couldn't open \"logo.capture\" for writing, FatFS result %d\n", result);
            panic();
        }

    }
    // USB_init();

    printf("* ");
    SERIAL_flush();

    while(!go) {
        int key;

        SERIAL_try_to_transmit_buffers();
        LED_beat_heart();

        SERIAL_poll_continue();

        process_monitor_queue();
    }

    // should not reach
    panic();
}
