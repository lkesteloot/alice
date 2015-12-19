#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XSTR(x) STR(x)
#define STR(x) # x

#include <stm32f4xx_hal.h>

#define enable_interrupts() __enable_irq()
#define disable_interrupts() __disable_irq()

void delay_ms(int ms)
{
    HAL_Delay(ms);
}

void delay_100ms(unsigned char count)
{
    HAL_Delay(count * 100);
}

#define PANIC_LED_PIN GPIO_PIN_13 
#define PANIC_LED_PORT GPIOC

#define INFO_LED_PIN GPIO_PIN_12 
#define INFO_LED_PORT GPIOC

#define HEARTBEAT_LED_PIN GPIO_PIN_15 
#define HEARTBEAT_LED_PORT GPIOA

void LED_set_panic(int on)
{
    HAL_GPIO_WritePin(PANIC_LED_PORT, PANIC_LED_PIN, on);
}

void LED_heartbeat()
{
    static int heartbeat_level = 1;
    static unsigned int previous_heartbeat_tick = 0;

    // TODO green heartbeat toggling .5Hz
    unsigned int now = HAL_GetTick();
    if(heartbeat_level == 1) {
        if(now - previous_heartbeat_tick > 350) {
            heartbeat_level = 0;
            HAL_GPIO_WritePin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    } else {
        if(now - previous_heartbeat_tick > 650) {
            heartbeat_level = 1;
            HAL_GPIO_WritePin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    }
}

void LED_set_info(int on)
{
    HAL_GPIO_WritePin(INFO_LED_PORT, INFO_LED_PIN, on);
}

void LED_setup()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = HEARTBEAT_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(HEARTBEAT_LED_PORT, &GPIO_InitStruct); 

    HAL_GPIO_WritePin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN, 1);

    GPIO_InitStruct.Pin = PANIC_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PANIC_LED_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = INFO_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(INFO_LED_PORT, &GPIO_InitStruct); 
}

void panic_worse()
{
    HAL_GPIO_WritePin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN, 0);
    LED_set_panic(1);
    for(;;);
}

static void panic(void)
{
    HAL_GPIO_WritePin(HEARTBEAT_LED_PORT, HEARTBEAT_LED_PIN, 0);
    int pin = 0;
    for(;;) {
        LED_set_panic(pin);
        pin = pin ? 0 : 1;
        delay_ms(100);
    }
}

void bell()
{
    putchar('\a');
}

int isprint(unsigned char a)
{
    return (a >= ' ') && (a <= '~');
}

unsigned char gConsoleOverflowed = 0;
unsigned char gKeyboardOverflowed = 0;

enum DebugLevels {
    DEBUG_SILENT = 0,
    DEBUG_ERRORS,
    DEBUG_WARNINGS,
    DEBUG_EVENTS,
    DEBUG_DATA,
    DEBUG_ALL,
    DEBUG_INSANE = 99,
};
int gDebugLevel = DEBUG_WARNINGS;

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


//----------------------------------------------------------------------------
// System Initialization Goop

// From Projects/STM32F411RE-Nucleo/Examples/GPIO/GPIO_IOToggle/Src/main.c
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 400
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
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
  
  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    panic();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    panic();
  }
}

void system_setup()
{
    HAL_Init();

    /* Configure the system clock to 100 MHz */
    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}


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

// Currently called from main() or intr(), so must wrap with disable_interrupts(), enable_interrupts();
unsigned char mon_queue_isfull()
{
    return mon_queue_length() == MON_QUEUE_LENGTH - 1;
}

// Currently called from main(), so, wrap with disable_interrupts(), enable_interrupts();
unsigned char mon_queue_isempty()
{
    return mon_queue_length() == 0;
}

// Caller must ensure only one call can proceed at a time ; currently
// called from main() and intr(), so wrap with disable_interrupts(), enable_interrupts();
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

// Currently called from main() or intr(), so must wrap with disable_interrupts(), enable_interrupts();
unsigned char con_queue_isfull()
{
    return con_queue_length() == CON_QUEUE_LENGTH - 1;
}

// Currently called from main(), so, wrap with disable_interrupts(), enable_interrupts();
unsigned char con_queue_isempty()
{
    return con_queue_length() == 0;
}

// Caller must ensure only one call can proceed at a time ; currently
// called from main() and intr(), so wrap with disable_interrupts(), enable_interrupts();
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
    disable_interrupts();
    full = con_queue_isfull();
    enable_interrupts();
    if(full) {
        gConsoleOverflowed = 1;
    } else {
        disable_interrupts();
        con_enqueue(d);
        enable_interrupts();
    }
}

// Call this from ISR, so skip di/ei
void console_enqueue_key_unsafe(unsigned char d)
{
    unsigned char full;
    full = con_queue_isfull();
    if(full) {
        gConsoleOverflowed = 1;
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

// Currently called from main() so must wrap with disable_interrupts(), enable_interrupts();
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
                                putchar('\n');
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
    // TODO
}

#if 0
    {
    // KBD clock TODO
        kbd_data = (kbd_data >> 1) | (?? << 10);
        kbd_bits++;
        if(kbd_bits == KBD_BIT_COUNT) {
            if(kbd_queue_isfull()) {
                gKeyboardOverflowed = 1;
            } else {
                kbd_enqueue((kbd_data >> 1) & 0xff);
            }
            kbd_data = 0;
            kbd_bits = 0;
        }
    }
#endif

/*--------------------------------------------------------------------------*/
/* USART - serial comms ----------------------------------------------------*/

static UART_HandleTypeDef gUARTHandle;

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&gUARTHandle);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  // __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /* Enable USART clock */
  __HAL_RCC_USART1_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* NVIC for USART */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
}

// XXX Really need to enqueue and drain in interrupt handler
// Or double-buffer two arrays for DMA
void __io_putchar( char c )
{
    char tmp = c;

    uint32_t tmp1 = 0;
    do {
        tmp1 = gUARTHandle.State;
    } while ((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX));

    if(HAL_UART_Transmit_IT(&gUARTHandle, (uint8_t *)&tmp, 1) != HAL_OK) {
        panic();
    }
}

volatile char gSerialCharBuffer;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    panic();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    int result;
    unsigned char tmp = gSerialCharBuffer;
    if((result = HAL_UART_Receive_IT(&gUARTHandle, (uint8_t *)&gSerialCharBuffer, 1)) != HAL_OK) {
        /* error_code = gUARTHandle.State; */
    }
    mon_enqueue(tmp);
}

void setup_serial()
{
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
        - Word Length = 8 Bits
        - Stop Bit = One Stop bit
        - Parity = ODD parity
        - BaudRate = 115200 baud
        - Hardware flow control disabled (RTS and CTS signals) */
    gUARTHandle.Instance          = USART1;
    
    gUARTHandle.Init.BaudRate     = 115200;
    gUARTHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    gUARTHandle.Init.StopBits     = UART_STOPBITS_1;
    gUARTHandle.Init.Parity       = UART_PARITY_NONE;
    gUARTHandle.Init.HwFlowCtl    = UART_HWCONTROL_RTS_CTS;
    gUARTHandle.Init.Mode         = UART_MODE_TX_RX;
    gUARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;
      
    if(HAL_UART_Init(&gUARTHandle) != HAL_OK)
    {
      /* Initialization Error */
      panic(); 
    }

    if(HAL_UART_Receive_IT(&gUARTHandle, (uint8_t *)&gSerialCharBuffer, 1) != HAL_OK)
    {
      /* Transfer error in reception process */
      panic();
    }
}


//----------------------------------------------------------------------------
// Alice 3 Bus

#define Z80_RESET_PIN GPIO_PIN_0
#define Z80_RESET_PORT GPIOB

#define Z80_INT_PIN GPIO_PIN_1
#define Z80_INT_PORT GPIOB

void z80_reset_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = Z80_RESET_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Z80_RESET_PORT, &GPIO_InitStruct); 
}

void z80_reset_start()
{
    HAL_GPIO_WritePin(Z80_RESET_PORT, Z80_RESET_PIN, 0);
    delay_ms(1);
}

void z80_reset_finish()
{
    HAL_GPIO_WritePin(Z80_RESET_PORT, Z80_RESET_PIN, 1);
}

void z80_interrupt_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = Z80_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Z80_INT_PORT, &GPIO_InitStruct); 
}

void z80_interrupt_start()
{
    HAL_GPIO_WritePin(Z80_INT_PORT, Z80_INT_PIN, 0);
    delay_ms(1);
}

void z80_interrupt_finish()
{
    HAL_GPIO_WritePin(Z80_INT_PORT, Z80_INT_PIN, 1);
}


volatile unsigned char gSerialInputToMonitor = 1;
volatile unsigned char gEscapeBackToMonitor = 0;

#define IOBOARD_POLL_AGAIN 0x00
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

// Caller must protect with disable_interrupts() / enable_interrupts();
void response_finish()
{
    response_index = 0;
    response_length = response_staging_length;
    response_waiting = 1;
}

// Element 0 is 1 here to force stoppage on receiving a bad command
const unsigned char command_lengths[8] = {1, 6, 134, 1, 1, 2, 6, 136};

void command_clear()
{
    disable_interrupts();
    command_request = IOBOARD_CMD_NONE;
    command_length = 0;
    enable_interrupts();
}

void response_clear()
{
    disable_interrupts();
    response_length = 0;
    response_index = 0;
    response_waiting = 0;
    enable_interrupts();
}

#define PIN_IORQ GPIO_PIN_0
#define PIN_RD GPIO_PIN_1
#define PIN_WR GPIO_PIN_2
#define PIN_A7 GPIO_PIN_4

#define BUS_PIN_MASK (PIN_IORQ | PIN_RD | PIN_WR | PIN_A7)
#define IO_BOARD_ADDR   0
#define IO_BOARD_ADDR_PINS   (IO_BOARD_ADDR & PIN_A7)

const unsigned int gREADSignals = PIN_WR | IO_BOARD_ADDR_PINS; // IORQ and RD 0
const unsigned int gWRITESignals = PIN_RD | IO_BOARD_ADDR_PINS; // IORQ and WR 0

void set_GPIOA_0_7_as_input()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x0;          // INPUT
}

void set_GPIOA_0_7_as_output()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x5555;       // OUTPUT
}

unsigned char get_GPIOA_0_7_value()
{
    return GPIOA->IDR & 0xff;
}

void set_GPIOA_0_7_value(unsigned char data)
{
    GPIOA->ODR = (GPIOA->ODR & ~0xff) | data;
}

void EXTI1_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(PIN_RD);
    NVIC_ClearPendingIRQ(EXTI1_IRQn);

    if((GPIOC->IDR & BUS_PIN_MASK) == gREADSignals) {

        unsigned char data = IOBOARD_POLL_AGAIN;

        if(response_length > 0) {

            data = response_bytes[response_index++];

            if(response_index >= response_length) {
                response_index = 0;
                response_length = 0;
                response_waiting = 0;
            }
        }

        set_GPIOA_0_7_value(data);
        set_GPIOA_0_7_as_output();

    } else {

        set_GPIOA_0_7_as_input();
    }
}

void EXTI2_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(PIN_WR);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);

    if((GPIOC->IDR & BUS_PIN_MASK) == gWRITESignals) {

        command_bytes[command_length++] = get_GPIOA_0_7_value();

    }
}

void setup_host()
{
    command_clear();
    response_clear();

    GPIO_InitTypeDef  GPIO_InitStruct;

    // __HAL_RCC_GPIOC_CLK_ENABLE();

    // configure PORT A0:A7 outputs for later
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~0xffff) | 0x0000;    // LOW
    GPIOA->OTYPER = (GPIOA->OTYPER & 0xff) | 0x0000;        // PUSH_PULL
    GPIOA->PUPDR = 0x0000;                                  // no PUPD

    set_GPIOA_0_7_as_input();

    // port C pins 1, 2 as inputs driving interrupts
    GPIO_InitStruct.Pin = PIN_RD | PIN_WR;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 

    // port C pins 0, 4, as inputs
    GPIO_InitStruct.Pin = PIN_IORQ | PIN_A7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 

    /* Enable and set EXTI Line0 Interrupt to the highest? priority */
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    /* Enable and set EXTI Line0 Interrupt to the highest? priority */
    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}


/*--------------------------------------------------------------------------*/
/* SD card -----------------------------------------------------------------*/

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

#define SPI_SD_PIN      GPIO_PIN_8
#define SPI_SD_PORT     GPIOA

void spi_enable_sd()
{
    HAL_GPIO_WritePin(SPI_SD_PORT, SPI_SD_PIN, 0);     // SS true
    delay_ms(100);
}

void spi_disable_sd()
{
    HAL_GPIO_WritePin(SPI_SD_PORT, SPI_SD_PIN, 1);     // SS false
    delay_ms(100);
}

// Postcondition: SPI configured for SD, SS high (false)
void spi_config_for_sd()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    gSPIHandle.Instance               = SPI2;

    // SPI2 is APB1, which is 1/4 system clock, or at 168MHz, APB1 is
    // 42MHz.  INit should be at  100KHz - 400 KHz, 128 will be 328.124Khz
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
    GPIO_InitStruct.Pin = SPI_SD_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // XXX change to match SPI rate?
    HAL_GPIO_Init(SPI_SD_PORT, &GPIO_InitStruct); 
    spi_disable_sd();
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

int gSDCardTimeoutMillis = 100;

#define BLOCK_SIZE 512

int gCachedBlockNumber = 0xffff;
unsigned char gCachedBlock[BLOCK_SIZE];

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

void spi_bulk(unsigned char *buffer, unsigned int nlen)
{
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("spi_bulk write:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
    int result = HAL_SPI_TransmitReceive(&gSPIHandle, buffer, buffer, nlen, 1000);
    if(result != HAL_OK){
        printf("spi_bulk: SPI error 0x%04X\n", result);
        panic();
    }
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("spi_bulk read:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
}

void spi_writen(unsigned char *buffer, unsigned int nlen)
{
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("spi_writen write:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
    int result = HAL_SPI_Transmit(&gSPIHandle, buffer, nlen, 1000);
    if(result != HAL_OK){
        printf("spi_writen: SPI error 0x%04X\n", result);
        panic();
    }
}

void spi_readn(unsigned char *buffer, unsigned int nlen)
{
    memset(buffer, 0xff, nlen);
    int result = HAL_SPI_TransmitReceive(&gSPIHandle, buffer, buffer, nlen, 1000);
    if(result != HAL_OK){
        printf("spi_readn: SPI error 0x%04X\n", result);
        panic();
    }
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("spi_readn read:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
}

// response length must include initial R1, so 1 for CMD0
int sdcard_send_command(enum SDCardCommand command, unsigned long parameter, unsigned char *response, int response_length)
{
    unsigned char command_buffer[6];

    command_buffer[0] = 0xff;
    spi_bulk(command_buffer, 1);

    command_buffer[0] = 0x40 | command;
    command_buffer[1] = (parameter >> 24) & 0xff;
    command_buffer[2] = (parameter >> 16) & 0xff;
    command_buffer[3] = (parameter >> 8) & 0xff;
    command_buffer[4] = (parameter >> 0) & 0xff;
    command_buffer[5] = ((crc7_generate_bytes(command_buffer, 5) & 0x7f) << 1) | 0x01;

    if(gDebugLevel >= DEBUG_DATA) printf("command constructed: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    spi_bulk(command_buffer, sizeof(command_buffer));
    if(gDebugLevel >= DEBUG_ALL) printf("returned in buffer: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("sdcard_send_command: timed out waiting on response\n");
            return 0;
        }
        response[0] = 0xff;
        spi_bulk(response, 1);
        if(gDebugLevel >= DEBUG_ALL) printf("response 0x%02X\n", response[0]);
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

    /* CS false, 80 clk pulses (read 10 bytes) */
    unsigned char buffer[10];
    for(unsigned int u = 0; u < sizeof(buffer); u++)
        buffer[u] = 0xff;
    spi_writen(buffer, sizeof(buffer));

    delay_ms(100);
    spi_enable_sd();
    /* interface init */
    if(!sdcard_send_command(CMD0, 0, response, 1))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        printf("sdcard_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }
    delay_ms(100);

    /* check voltage */
    if(!sdcard_send_command(CMD8, 0x000001AA, response, 5))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        printf("sdcard_init: failed to get OCR, response was 0x%02X\n", response[0]);
        return 0;
    }
    OCR = (((unsigned long)response[1]) << 24) | (((unsigned long)response[2]) << 16) | (((unsigned long)response[3]) << 8) | (((unsigned long)response[4]) << 0);
    if(gDebugLevel >= DEBUG_DATA) printf("sdcard_init: OCR response is 0x%08lX\n", OCR);

    // should get CSD, CID, print information about them

    // Ask the card to initialize itself, and wait for it to get out of idle mode.
    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("sdcard_init: timed out waiting on transition to ACMD41\n");
            return 0;
        }
        /* leading command to the ACMD41 command */
        if(!sdcard_send_command(CMD55, 0x00000000, response, 1))
            return 0;
        if(response[0] != gSDCardResponseIDLE) {
            printf("sdcard_init: not in IDLE mode for CMD55, response was 0x%02X\n", response[0]);
            return 0;
        }
        /* start initialization process, set HCS (high-capacity) */
        if(!sdcard_send_command(ACMD41, 0x40000000, response, 1))
            return 0;
    } while(response[0] != gSDCardResponseSUCCESS);
    if(gDebugLevel >= DEBUG_ALL) printf("returned from ACMD41: %02X\n", response[0]);

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
    unsigned char response[8];

    // Send read block command.
    response[0] = 0xff;
    if(!sdcard_send_command(CMD17, blocknum, response, 1))
        return 0;
    if(response[0] != gSDCardResponseSUCCESS) {
        printf("sdcard_readblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait for the data token.
    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("sdcard_readblock: timed out waiting for data token\n");
            return 0;
        }
        spi_readn(response, 1);
        if(gDebugLevel >= DEBUG_ALL) printf("readblock response 0x%02X\n", response[0]);
    } while(response[0] != gSDCardToken_17_18_24);

    // Read data.
    spi_readn(block, BLOCK_SIZE);

    // Read CRC
    spi_readn(response, 2);
    if(gDebugLevel >= DEBUG_DATA) printf("CRC is 0x%02X%02X\n", response[0], response[1]);

    unsigned short crc_theirs = response[0] * 256 + response[1];

    // calculate our version of CRC and compare
    unsigned short crc_ours = crc_itu_t(0, block, BLOCK_SIZE);

    if(crc_theirs != crc_ours) {
        printf("CRC mismatch (theirs %04X versus ours %04X, reporting failure)\n", crc_theirs, crc_ours);
        return 0;
    } else {
        if(gDebugLevel >= DEBUG_DATA) printf("CRC matches\n");
    }

    // Wait for DO to go high. I don't think we need to do this for block reads,
    // but I don't think it'll hurt.
    then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("sdcard_readblock: timed out waiting on completion\n");
            return 0;
        }
        spi_readn(response, 1);
        if(gDebugLevel >= DEBUG_ALL) printf("readblock response 0x%02X\n", response[0]);
    } while(response[0] != 0xFF);

    if(gDebugLevel >= DEBUG_ALL) dump_more_spi_bytes("read completion");

    return 1;
}

/* precondition: SDcard CS is low (active) */
int sdcard_writeblock(unsigned int blocknum, unsigned char *block)
{
    int count;
    unsigned char response[8];

    // Send write block command.
    if(!sdcard_send_command(CMD24, blocknum, response, 1))
        return 0;
    if(response[0] != gSDCardResponseSUCCESS) {
        printf("sdcard_writeblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }
    // XXX - elm-chan.org says I should be waiting >= 1byte here

    // Data token.
    response[0] = gSDCardToken_17_18_24;
    spi_writen(response, 1);

    // Send data.
    spi_writen(block, BLOCK_SIZE);

    // junk CRC
    response[0] = 0xff;
    response[1] = 0xff;
    spi_writen(response, 2);

    // Get DATA_ACCEPTED response from WRITE
    spi_readn(response, 1);
    if(gDebugLevel >= DEBUG_DATA) printf("writeblock response 0x%02X\n", response[0]);
    if(response[0] != gSDCardResponseDATA_ACCEPTED) {
        printf("sdcard_writeblock: failed to respond with DATA_ACCEPTED, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait while busy (DO = low).
    int then = HAL_GetTick();
    count = 0;
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            printf("sdcard_writeblock: timed out waiting on completion\n");
            return 0;
        }
        spi_readn(response, 1);
        if(gDebugLevel >= DEBUG_ALL) printf("writeblock completion 0x%02X\n", response[0]);
        count++;
    } while(response[0] != 0xFF);
    if(gDebugLevel >= DEBUG_DATA) printf("read %d SPI bytes waiting on write to complete.\n", count);

    if(gDebugLevel >= DEBUG_ALL) dump_more_spi_bytes("write completion");

    return 1;
}

void test_sd_card()
{
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
            gCachedBlock[i] = (original_block[i] + 0x55) % 256;

        if(!sdcard_writeblock(block_number, gCachedBlock)) {
            printf("PANIC: Failed writeblock\n");
            panic();
        }
        // printf("Wrote junk block\n");

        for(i = 0; i < BLOCK_SIZE; i++)
            gCachedBlock[i] = 0;
        if(!sdcard_readblock(block_number, gCachedBlock)) {
            printf("PANIC: Failed readblock\n");
            panic();
        }

        success = 1;
        for(i = 0; i < BLOCK_SIZE; i++)
            if(gCachedBlock[i] != (original_block[i] + 0x55) % 256)
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of junk to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                gCachedBlock[0], gCachedBlock[1], gCachedBlock[2], gCachedBlock[3]);
        } else {
            printf("Verified junk block was written\n");
        }

        if(!sdcard_writeblock(block_number, original_block)) {
            printf("PANIC: Failed writeblock\n");
            panic();
        }
        // printf("Wrote original block\n");

        if(!sdcard_readblock(block_number, gCachedBlock)) {
            printf("PANIC: Failed readblock\n");
            panic();
        }

        success = 1;
        for(i = 0; i < BLOCK_SIZE; i++)
            if(original_block[i] != gCachedBlock[i])
                success = 0;

        if(!success) {
            printf("whoops, error verifying write of original to block 0\n");
            printf("block read: %02X %02X %02X %02X\n",
                gCachedBlock[0], gCachedBlock[1], gCachedBlock[2], gCachedBlock[3]);
        } else {
            printf("Verified original block was written\n");
        }
        block_number ++;
    }
}

#define SECTORS_PER_BLOCK 4
#define SECTORS_PER_TRACK 64
#define TRACKS_PER_DISK 1024
#define SECTOR_SIZE 128
/* disk is 8MB, so 16384 512-byte blocks per disk */
#define BLOCKS_PER_DISK 16384


char gMonitorCommandLine[80];
unsigned char gMonitorCommandLineLength = 0;

void usage()
{
    printf("help       - this help message\n");
    printf("debug N    - set debug level\n");
    printf("buffers    - print summary of command and response buffers\n");
    printf("reset      - reset Z80 and clear communication buffers\n");
    printf("int        - send /INT to Z80\n");
    printf("sdreset    - reset SD\n");
    printf("sdss {0|1} - 1: enable SD (SS=GND), 0: disable SD (SS=3.3V)\n");
    printf("spiwrite B0 [B1 ...]\n");
    printf("           - send bytes to SD SPI port and print read bytes\n");
    printf("spiread N  - read N bytes from SD SPI port\n");
    printf("dumpkbd    - toggle dumping keyboard\n");
    printf("clear      - clear command and response buffer\n");
    printf("pass       - pass monitor keys to Z80\n");
    printf("version    - print firmware build version\n");
    printf("read N     - read and dump block\n");
    printf("bus        - print bus line status\n");
    printf("panic      - force panic\n");
}

#define IOBOARD_FIRMWARE_VERSION_STRING XSTR(IOBOARD_FIRMWARE_VERSION)

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

        } else if(strcmp(gMonitorCommandLine, "bus") == 0) {

            unsigned char control = GPIOC->IDR & BUS_PIN_MASK;
            printf("control = 0x%02X\n", control);
            printf("    ");
            if(!(control & PIN_IORQ))
                printf("IORQ ");
            if(!(control & PIN_RD))
                printf("RD ");
            if(!(control & PIN_WR))
                printf("WR ");
            printf("\n");
            printf("    A7 = %d\n", control & PIN_A7 ? 1 : 0);

            unsigned char data = get_GPIOA_0_7_value();
            printf("data = 0x%02X\n", data);

        } else if(strcmp(gMonitorCommandLine, "sdreset") == 0) {

            printf("Resetting SD card...\n");

            if(!sdcard_init())
                printf("Failed to start access to SD card as SPI\n");

        } else if(strcmp(gMonitorCommandLine, "dumpkbd") == 0) {

            dump_keyboard_data = !dump_keyboard_data;
            if(dump_keyboard_data)
                printf("Dumping keyboard data...\n");
            else
                printf("Not dumping keyboard data...\n");

        } else if(strcmp(gMonitorCommandLine, "panic") == 0) {

            panic();

        } else if(strcmp(gMonitorCommandLine, "reset") == 0) {

            z80_reset_start();
            printf("Resetting Z-80 and communication buffers...\n");
            response_clear();
            command_clear();
            z80_reset_finish();

        } else if(strcmp(gMonitorCommandLine, "int") == 0) {

            z80_interrupt_start();
            printf("Interupting Z-80...");
            putchar('\n');
            z80_interrupt_finish();

        } else if(strcmp(gMonitorCommandLine, "clear") == 0) {

            response_clear();
            command_clear();
            printf("Command and response data cleared\n");

        } else if(strcmp(gMonitorCommandLine, "pass") == 0) {

            gSerialInputToMonitor = 0;
            printf("Press CTRL-A then CTRL-B to return to monitor.\n");

        } else if(strcmp(gMonitorCommandLine, "buffers") == 0) {

            printf("Command request: 0x%02X\n", command_request);
            printf("Command length: %d bytes\n", command_length);
            if(command_length > 0) {
                printf("Command buffer:\n");
                dump_buffer_hex(4, (unsigned char *)command_bytes, command_length);
            }

            printf("Response length: %d bytes\n", response_length);
            printf("Response next byte to put on bus: %d\n", response_index);
            if(response_length > 0) {
                printf("Response buffer:\n");
                dump_buffer_hex(4, (unsigned char *)response_bytes, response_length);
            }

        } else if(strcmp(gMonitorCommandLine, "version") == 0) {

            printf("%s\n", IOBOARD_FIRMWARE_VERSION_STRING);

        } else if(strncmp(gMonitorCommandLine, "debug ", 6) == 0) {

            char *p = gMonitorCommandLine + 5;
            while(*p == ' ')
                p++;
            gDebugLevel = strtol(p, NULL, 0);
            printf("Debug level set to %d\n", gDebugLevel);

        } else if(strncmp(gMonitorCommandLine, "spiwrite ", 9) == 0) {

            char *endptr = gMonitorCommandLine + 9;
            char *p;
            int count = 0;
            unsigned char buffer[16];
            do {
                p = endptr;
                buffer[count] = strtol(p, &endptr, 0);
                if(p != endptr)
                    count++;
            } while(endptr != p);
            printf("Writing %d bytes\n", count);
            dump_buffer_hex(4, buffer, count);

            spi_bulk(buffer, count);

            if(count == 1) {
                printf("Byte returned: 0x%02X\n", buffer[0]);
            } else {
                printf("Bytes returned:\n");
                dump_buffer_hex(4, buffer, count);
            }

        } else if(strncmp(gMonitorCommandLine, "spiread ", 8) == 0) {

            char *p = gMonitorCommandLine + 8;
            while(*p == ' ')
                p++;
            int count = strtol(p, NULL, 0);
            unsigned char buffer[16];
            if(count > sizeof(buffer)) {
                printf("count %d is larger than available buffer, truncating to %d\n", count, sizeof(buffer));
                count = sizeof(buffer);
            }
            spi_readn(buffer, count);
            if(count == 1) {
                printf("Byte returned: 0x%02X\n", buffer[0]);
            } else {
                printf("Bytes returned:\n");
                dump_buffer_hex(4, buffer, count);
            }

        } else if(strncmp(gMonitorCommandLine, "sdss ", 5) == 0) {

            char *p = gMonitorCommandLine + 4;
            while(*p == ' ')
                p++;
            int ss = strtol(p, NULL, 0);
            if(ss) {
                spi_enable_sd();
                printf("/SS to SD card is enabled (GND)\n");
            } else {
                spi_disable_sd();
                printf("/SS to SD card is disabled (+3.3V)\n");
            }

        } else if(strncmp(gMonitorCommandLine, "read ", 5) == 0) {

            char *p = gMonitorCommandLine + 4;
            while(*p == ' ')
                p++;
            gCachedBlockNumber = strtol(p, NULL, 0);
            if(sdcard_readblock(gCachedBlockNumber, gCachedBlock)) {
                dump_buffer_hex(4, gCachedBlock, BLOCK_SIZE);
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

int main()
{
    unsigned char responseWasWaiting = 0;

    system_setup();

    LED_setup();
    LED_heartbeat();

    z80_reset_init();
    z80_interrupt_init();
    LED_heartbeat();

    setbuf(stdout, NULL);
    setup_serial(); // transmit and receive but global interrupts disabled
    LED_heartbeat();

    printf("\n\nAlice 3 I/O board firmware, %s\n", IOBOARD_FIRMWARE_VERSION_STRING);
    LED_heartbeat();

    spi_config_for_sd();
    LED_heartbeat();

    if(!sdcard_init())
        printf("Failed to start access to SD card as SPI!!\n");
    else 
        printf("SD Card interface is initialized for SPI\n");
    LED_heartbeat();

    if(0) {
        test_sd_card();
        LED_heartbeat();
    }

    setup_host();
    LED_heartbeat();

    setup_keyboard();
    LED_heartbeat();

    printf("* ");

    for(;;) {

        LED_heartbeat();

        disable_interrupts();
        unsigned char isEmpty = mon_queue_isempty();
        enable_interrupts();

        if(!isEmpty) {
            disable_interrupts();
            unsigned char c = mon_dequeue();
            enable_interrupts();
            if(gSerialInputToMonitor)
                process_local_key(c);
            else {
                if(gEscapeBackToMonitor == 0 && c == 1)
                    gEscapeBackToMonitor = 1;
                else if(gEscapeBackToMonitor != 0 && c == 2) {
                    gEscapeBackToMonitor = 0;
                    gSerialInputToMonitor = 1;
                    printf("Serial input returned to monitor\n");
                } else {
                    gEscapeBackToMonitor = 0;
                    console_enqueue_key(c);
                }
            }
        }

        if(command_length > 0) {
            unsigned char command_byte = command_bytes[0];
            if(gDebugLevel >= DEBUG_DATA) printf("receiving command...\n");

            if((command_byte < IOBOARD_CMD_MIN) || (command_byte > IOBOARD_CMD_MAX)) {

                if(gDebugLevel >= DEBUG_ERRORS) printf("ERROR: Unknown command 0x%02X received\n", command_byte);
                command_clear();

            } else if(command_length >= command_lengths[command_byte]) {
                command_request = command_byte;
                if(gDebugLevel >= DEBUG_EVENTS) printf("complete command received.\n");
                if(command_length > command_lengths[command_byte]) {
                    if(gDebugLevel >= DEBUG_ERRORS) printf("ERROR: command buffer longer than expected for command.\n");
                }
            }
        }

        if(command_request != IOBOARD_CMD_NONE) {
            response_start();
            LED_set_info(1);
            switch(command_request) {
                case IOBOARD_CMD_READ:
                case IOBOARD_CMD_READ_SUM:
                {
                    unsigned int disk = command_bytes[1];
                    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
                    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
                    unsigned int block_number = disk * BLOCKS_PER_DISK + (track * SECTORS_PER_TRACK + sector) / SECTORS_PER_BLOCK;
                    unsigned int sector_byte_offset = 128 * ((track * SECTORS_PER_TRACK + sector) % SECTORS_PER_BLOCK);

                    if(gDebugLevel >= DEBUG_EVENTS) printf("read disk %d, sector %d, track %d -> block %d, offset %d\n", disk, sector, track, block_number, sector_byte_offset);

                    if(disk > 3) { 
                        if(gDebugLevel >= DEBUG_WARNINGS) printf("asked for disk out of range\n");
                        response_append(IOBOARD_FAILURE);
                        break;
                    }

                    if(gCachedBlockNumber == block_number) {
                        if(gDebugLevel >= DEBUG_DATA) printf("Block already in cache.\n");
                    } else {
                        if(!sdcard_readblock(block_number, gCachedBlock)) {
                            if(gDebugLevel >= DEBUG_WARNINGS) printf("some kind of block read failure\n");
                            response_append(IOBOARD_FAILURE);
                            break;
                        }
                        if(gDebugLevel >= DEBUG_DATA) printf("New cached block\n");
                        if(gDebugLevel >= DEBUG_ALL) dump_buffer_hex(4, gCachedBlock, BLOCK_SIZE);
                        gCachedBlockNumber = block_number;
                    }

                    response_append(IOBOARD_SUCCESS);

                    for(unsigned int u = 0; u < SECTOR_SIZE; u++)
                        response_append(gCachedBlock[sector_byte_offset + u]);

                    if(command_request == IOBOARD_CMD_READ_SUM) {
                        unsigned short sum = 0;

                        for(unsigned int u = 0; u < SECTOR_SIZE; u++)
                            sum += gCachedBlock[sector_byte_offset + u];

                        response_append(sum & 0xff);
                        response_append((sum >> 8) & 0xff);
                        if(gDebugLevel >= DEBUG_ALL) printf("checksum calculated as %u: 0x%02X then 0x%02X\n", sum, sum & 0xff, (sum >> 8) & 0xff);
                    }
                    break;
                }

                case IOBOARD_CMD_WRITE:
                case IOBOARD_CMD_WRITE_SUM:
                {
                    unsigned int disk = command_bytes[1];
                    unsigned int sector = command_bytes[2] + 256 * command_bytes[3];
                    unsigned int track = command_bytes[4] + 256 * command_bytes[5];
                    unsigned int block_number = disk * BLOCKS_PER_DISK + (track * SECTORS_PER_TRACK + sector) / SECTORS_PER_BLOCK;
                    unsigned int sector_byte_offset = 128 * ((track * SECTORS_PER_TRACK + sector) % SECTORS_PER_BLOCK);

                    if(gDebugLevel >= DEBUG_EVENTS) printf("write disk %d, sector %d, track %d -> block %d, offset %d\n", disk, sector, track, block_number, sector_byte_offset);

                    if(disk > 3) { 
                        if(gDebugLevel >= DEBUG_WARNINGS) printf("asked for disk out of range\n");
                        response_append(IOBOARD_FAILURE);
                        break;
                    }

                    if(command_request == IOBOARD_CMD_WRITE_SUM) {
                        unsigned short sum = 0;
                        unsigned char offset = 6;
                        for(unsigned int u = 0; u < SECTOR_SIZE; u++)
                            sum += command_bytes[offset + u];
                        unsigned short theirs = command_bytes[134] | (command_bytes[135] << 8);
                        if(sum != theirs) {
                            if(gDebugLevel >= DEBUG_WARNINGS) printf("WARNING: IOBOARD_CMD_WRITE_SUM checksum does not match\n");
                            // XXX retry?
                            response_append(IOBOARD_FAILURE);
                            break;
                        }
                    }

                    if(gCachedBlockNumber == block_number) {
                        if(gDebugLevel >= DEBUG_DATA) printf("Block already in cache.\n");
                    } else {
                        if(!sdcard_readblock(block_number, gCachedBlock)) {
                            if(gDebugLevel >= DEBUG_WARNINGS) printf("some kind of block read failure\n");
                            response_append(IOBOARD_FAILURE);
                            break;
                        }
                        if(gDebugLevel >= DEBUG_DATA) printf("New cached block\n");
                        gCachedBlockNumber = block_number;
                    }

                    for(unsigned int u = 0; u < SECTOR_SIZE; u++)
                        gCachedBlock[sector_byte_offset + u] = command_bytes[6 + u];
                    if(!sdcard_writeblock(block_number, gCachedBlock)) {
                        printf("some kind of block write failure\n");
                        response_append(IOBOARD_FAILURE);
                        break;
                    }

                    response_append(IOBOARD_SUCCESS);

                    break;
                }

                case IOBOARD_CMD_SEROUT: {
                    putchar(command_bytes[1]);
                    response_append(IOBOARD_SUCCESS);
                    break;
                }

                case IOBOARD_CMD_CONST: {
                    if(gDebugLevel >= DEBUG_EVENTS)printf("CONST\n");
                    disable_interrupts();
                    isEmpty = con_queue_isempty();
                    enable_interrupts();
                    if(!isEmpty)
                        response_append(IOBOARD_READY);
                    else
                        response_append(IOBOARD_NOT_READY);
                    break;
                }

                case IOBOARD_CMD_CONIN: {
                    unsigned char c;
                    if(gDebugLevel >= DEBUG_EVENTS) printf("CONIN\n");
                    disable_interrupts();
                    isEmpty = con_queue_isempty();
                    enable_interrupts();
                    if(!isEmpty) {
                        disable_interrupts();
                        c = con_dequeue();
                        enable_interrupts();
                        response_append(IOBOARD_SUCCESS);
                        response_append(c);
                    } else {
                        printf("Hm, char wasn't actually ready at CONIN\n");
                        response_append(IOBOARD_SUCCESS);
                        response_append(0);
                    }
                    break;
                }

                default: {
                    printf("unexpected command 0x%02X from z80!\n", command_request);
                    break;
                }
            }
            LED_set_info(0);

            if(response_staging_length > 0) {
                if(gDebugLevel >= DEBUG_DATA) printf("will respond with %d\n", response_staging_length);
                disable_interrupts();
                command_clear();
                response_finish();
                responseWasWaiting = 1;
                enable_interrupts();
            }
        }

        {
            disable_interrupts();
            isEmpty = kbd_queue_isempty();
            enable_interrupts();
            if(!isEmpty) {
                unsigned char kb = kbd_dequeue();
                if(dump_keyboard_data)
                    printf("keyboard scan code: %02X\n", kb);
                kbd_process_byte(kb);
            }
        }

        if(gConsoleOverflowed) {
            if(gDebugLevel >= DEBUG_WARNINGS) printf("WARNING: Console input queue overflow\n");
            gConsoleOverflowed = 0;
        }

        if(gKeyboardOverflowed) {
            if(gDebugLevel >= DEBUG_WARNINGS) printf("WARNING: Keyboard data queue overflow\n");
            gKeyboardOverflowed = 0;
        }

        if(responseWasWaiting && !response_waiting) {
            if(gDebugLevel >= DEBUG_EVENTS) printf("response packet was read\n");
            responseWasWaiting = 0;
        }
    }

    // should not reach
    panic();
}
