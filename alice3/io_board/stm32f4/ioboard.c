#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ff.h"
#include "diskio.h"

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

void set_GPIO_value(GPIO_TypeDef* gpio, int mask, int value)
{
    unsigned long int data = value ? mask : 0;
    gpio->ODR = (gpio->ODR & ~mask) | data;
}

void set_GPIO_iotype(GPIO_TypeDef* gpio, int pin, unsigned int iotype)
{
    long unsigned int mask = ~(3U << (pin * 2));
    long unsigned int value = iotype << (pin * 2);
    gpio->MODER = (gpio->MODER & mask) | value;
}

#define LED_PIN_MASK GPIO_PIN_2
#define LED_PORT GPIOB

void LED_set_panic(int on)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, on);
}

void LED_beat_heart()
{
    static int heartbeat_level = 1;
    static unsigned int previous_heartbeat_tick = 0;

    unsigned int now = HAL_GetTick();
    if(heartbeat_level == 1) {
        if(now - previous_heartbeat_tick > 350) {
            heartbeat_level = 0;
            HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    } else {
        if(now - previous_heartbeat_tick > 650) {
            heartbeat_level = 1;
            HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    }
}

void LED_set_info(int on)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, on);
}

void LED_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = LED_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct); 

    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, 1);
}

void panic_worse()
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, 0);
    LED_set_panic(1);
    for(;;);
}

void serial_flush();

static void panic(void)
{
    static int entered = 0;

    LED_set_panic(1);

    int pin = 0;
    for(;;) {
        if(!entered) {
            // serial_flush() can itself panic(), so stop reentry here
            entered = 1;
            serial_flush();
            entered = 0;
        }

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

void dump_buffer_hex(int indent, const unsigned char *data, int size)
{
    int address = 0;
    int i;

    while(size > 0) {
        int howmany = (size < 16) ? size : 16;

        printf("%*s0x%04X: ", indent, "", address);
        for(i = 0; i < howmany; i++)
            printf("%02X ", data[i]);
        printf("\n");
        serial_flush();

        printf("%*s        ", indent, "");
        for(i = 0; i < howmany; i++)
            printf(" %c ", isprint(data[i]) ? data[i] : '.');
        printf("\n");
        serial_flush();

        size -= howmany;
        data += howmany;
        address += howmany;
    }
}

//----------------------------------------------------------------------------
// From: https://kbiva.wordpress.com/2013/03/25/microsecond-delay-function-for-stm32/

void DWT_Init(void) 
{
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) 
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}
 
uint32_t DWT_Get(void)
{
  return DWT->CYCCNT;
}
 
__inline
uint8_t DWT_Compare(int32_t tp)
{
  return (((int32_t)DWT_Get() - tp) < 0);
}
 
void DWT_Delay(uint32_t us) // microseconds
{
  int32_t tp = DWT_Get() + us * (SystemCoreClock/1000000);
  while (DWT_Compare(tp));
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

    DWT_Init();
}


//----------------------------------------------------------------------------
// Byte consumer-producer queue

struct queue {
    short next_head;
    short tail;
    unsigned short capacity;
    unsigned char queue[];
};

void queue_init(volatile struct queue *q, unsigned short capacity)
{
    q->next_head = 0;
    q->tail = 0;
    q->capacity = capacity;
}

// Protect with critical section if not called from producer
int queue_isfull(volatile struct queue *q)
{
    int length = (q->next_head + q->capacity - q->tail) % q->capacity;
    return length == q->capacity - 1;
}

// Protect with critical section if not called from consumer
int queue_isempty(volatile struct queue *q)
{
    return q->next_head == q->tail;
}

// Protect with critical section if not called from producer
void queue_enq(volatile struct queue *q, unsigned char d)
{
    q->queue[q->next_head] = d;
    q->next_head = (q->next_head + 1) % q->capacity;
}

// Protect with critical section if not called from consumer
unsigned char queue_deq(volatile struct queue *q)
{
    unsigned char d = q->queue[q->tail];
    q->tail = (q->tail + 1) % q->capacity;
    return d;
}


//----------------------------------------------------------------------------
// Monitor input queue

#define MON_QUEUE_CAPACITY 32
struct mon_queue_struct {
    struct queue q;
    unsigned char queue[MON_QUEUE_CAPACITY];
};
volatile struct mon_queue_struct mon_queue;

// Call this from ISR, so skip di/ei
void monitor_enqueue_key_unsafe(unsigned char d)
{
    if(!queue_isfull(&mon_queue.q)) {
        queue_enq(&mon_queue.q, d);
    }
}


//----------------------------------------------------------------------------
// Console input queue

#define CON_QUEUE_CAPACITY 64
struct con_queue_struct {
    struct queue q;
    unsigned char queue[CON_QUEUE_CAPACITY];
};
volatile struct con_queue_struct con_queue;

void console_enqueue_key(unsigned char d)
{
    unsigned char full;
    disable_interrupts();
    full = queue_isfull(&con_queue.q);
    if(full) {
        gConsoleOverflowed = 1;
    } else {
        queue_enq(&con_queue.q, d);
    }
    enable_interrupts();
}

// Call this from ISR, so skip di/ei
void console_enqueue_key_unsafe(unsigned char d)
{
    unsigned char full;
    full = queue_isfull(&con_queue.q);
    if(full) {
        gConsoleOverflowed = 1;
    } else {
        queue_enq(&con_queue.q, d);
    }
}


//----------------------------------------------------------------------------
// AT and PS/2 Keyboard processing

#define KBD_QUEUE_CAPACITY 16
struct kbd_queue_struct {
    struct queue q;
    unsigned char queue[KBD_QUEUE_CAPACITY];
};
volatile struct kbd_queue_struct kbd_queue;

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

#define KEYBOARD_CLOCK_PIN_MASK GPIO_PIN_11
#define KEYBOARD_CLOCK_PORT GPIOB

#define KEYBOARD_DATA_PIN_MASK GPIO_PIN_12
#define KEYBOARD_DATA_PORT GPIOB

void KBD_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    // There's a possibility we could use USART for this
    // and remove some CPU work

    GPIO_InitStruct.Pin = KEYBOARD_CLOCK_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEYBOARD_CLOCK_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = KEYBOARD_DATA_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEYBOARD_DATA_PORT, &GPIO_InitStruct); 

    /* Enable and set EXTI Line15-10 interrupt */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(KEYBOARD_CLOCK_PIN_MASK);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

    unsigned int keyboard_data_pin = GPIOB->IDR & KEYBOARD_DATA_PIN_MASK;
    unsigned int keyboard_data = keyboard_data_pin ? 1 : 0;

    kbd_data = (kbd_data >> 1) | (keyboard_data << 10);
    kbd_bits++;
    if(kbd_bits == KBD_BIT_COUNT) {
        if(queue_isfull(&kbd_queue.q)) {
            gKeyboardOverflowed = 1;
        } else {
            queue_enq(&kbd_queue.q, (kbd_data >> 1) & 0xff);
        }
        kbd_data = 0;
        kbd_bits = 0;
    }
}


/*--------------------------------------------------------------------------*/
/* USART - serial comms ----------------------------------------------------*/

static UART_HandleTypeDef gUARTHandle;

#define TRANSMIT_BUFFER_SIZE 512
volatile unsigned char gTransmitBuffers[2][TRANSMIT_BUFFER_SIZE];
volatile int gNextTransmitBuffer = 0;
volatile int gTransmitBufferLengths[2] = {0, 0};
volatile int gUARTTransmitBusy = 0;

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
  HAL_NVIC_SetPriority(USART1_IRQn, 3, 1);
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    gUARTTransmitBusy = 0;
}

void serial_try_to_transmit_buffers()
{
    if(!gUARTTransmitBusy && gTransmitBufferLengths[gNextTransmitBuffer] > 0) {
        gUARTTransmitBusy = 1;

        if(HAL_UART_Transmit_IT(&gUARTHandle, (uint8_t *)&gTransmitBuffers[gNextTransmitBuffer][0], gTransmitBufferLengths[gNextTransmitBuffer]) != HAL_OK) {
            panic();
        }

        gNextTransmitBuffer ^= 1;
        gTransmitBufferLengths[gNextTransmitBuffer] = 0;
    }
}

void serial_flush()
{
    while(gUARTTransmitBusy || gTransmitBufferLengths[gNextTransmitBuffer] > 0)
        serial_try_to_transmit_buffers();
}

void serial_enqueue_one_char(char c)
{
    do {
        // Transmit the current buffer if there is one and serial
        // port is not busy
        serial_try_to_transmit_buffers();

        // While there's no room in the current buffer, repeat until buffer becomes available
    } while(gTransmitBufferLengths[gNextTransmitBuffer] >= TRANSMIT_BUFFER_SIZE);

    int length = gTransmitBufferLengths[gNextTransmitBuffer];
    gTransmitBuffers[gNextTransmitBuffer][length] = c;
    gTransmitBufferLengths[gNextTransmitBuffer]++;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    panic();
}

volatile char gSerialCharBuffer;
volatile char gStartAnotherUARTReceive = 1;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    queue_enq(&mon_queue.q, gSerialCharBuffer);
    gStartAnotherUARTReceive = 1;
}

void SERIAL_init()
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
// Alice 3 Bus IO communication protocol

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

volatile unsigned char command_bytes[1 + 5 + 128 + 2]; // largest is status + write + sector + 16-bit checksum
volatile unsigned char command_length;

unsigned char gNextByteForReading = IOBOARD_POLL_AGAIN;
volatile unsigned char response_bytes[1 + 128 + 2]; // largest is status + sector + 16-bit checksum
volatile unsigned char response_staging_length;
volatile unsigned char response_length;
volatile unsigned char response_index;
volatile unsigned char response_waiting;
volatile unsigned char gResponseWasWaiting = 0;

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

// Caller must protect with disable_interrupts() / enable_interrupts()
void response_finish()
{
    gNextByteForReading = response_bytes[0];
    response_index = 1;
    response_length = response_staging_length;
    response_waiting = 1;
}

// Element 0 is 1 here to force stoppage on receiving a bad command
const unsigned char command_lengths[8] = {1, 6, 134, 1, 1, 2, 6, 136};

// Caller must protect with disable_interrupts()/enable_interrupts() if necessary
void command_clear()
{
    command_length = 0;
}

void BUS_set_DATA(unsigned char data);
void response_clear()
{
    response_length = 0;
    response_index = 0;
    response_waiting = 0;
    gNextByteForReading = IOBOARD_POLL_AGAIN;
    BUS_set_DATA(gNextByteForReading);
}


//----------------------------------------------------------------------------
// Alice 3 system bus

#define MEMORY_DELAY_MICROS     10
#define IO_DELAY_MICROS     2

// IORQ, RD, WR, and A7 are specially implemented to be on one GPIO register
// in order to quickly determine if IO is being read or written

#define BUS_SIGNAL_CHECK_PORT GPIOC

#define BUS_IORQ_PIN_MASK GPIO_PIN_0
#define BUS_IORQ_PIN 0
#define BUS_IORQ_PORT BUS_SIGNAL_CHECK_PORT
#define BUS_IORQ_ACTIVE 0
#define BUS_IORQ_INACTIVE BUS_IORQ_PIN_MASK

#define BUS_RD_PIN_MASK GPIO_PIN_1
#define BUS_RD_PIN 1
#define BUS_RD_PORT BUS_SIGNAL_CHECK_PORT
#define BUS_RD_ACTIVE 0
#define BUS_RD_INACTIVE BUS_RD_PIN_MASK

#define BUS_WR_PIN_MASK GPIO_PIN_2
#define BUS_WR_PIN 2
#define BUS_WR_PORT BUS_SIGNAL_CHECK_PORT
#define BUS_WR_ACTIVE 0
#define BUS_WR_INACTIVE BUS_WR_PIN_MASK

#define BUS_A7_PIN_MASK GPIO_PIN_4
#define BUS_A7_PORT BUS_SIGNAL_CHECK_PORT

#define BUS_MREQ_PIN_MASK GPIO_PIN_1
#define BUS_MREQ_PIN 1
#define BUS_MREQ_PORT GPIOB
#define BUS_MREQ_ACTIVE 0
#define BUS_MREQ_INACTIVE BUS_MREQ_PIN_MASK

#define BUS_BUSRQ_PIN_MASK GPIO_PIN_11
#define BUS_BUSRQ_PIN 11
#define BUS_BUSRQ_PORT GPIOC
#define BUS_BUSRQ_ACTIVE 0
#define BUS_BUSRQ_INACTIVE BUS_BUSRQ_PIN_MASK

#define BUS_BUSAK_PIN_MASK GPIO_PIN_10
#define BUS_BUSAK_PIN 10
#define BUS_BUSAK_PORT GPIOC
#define BUS_BUSAK_ACTIVE 0
#define BUS_BUSAK_INACTIVE BUS_BUSAK_PIN_MASK

#define BUS_HALT_PIN_MASK GPIO_PIN_15
#define BUS_HALT_PIN 15
#define BUS_HALT_PORT GPIOA

volatile int gZ80IsInRESET = 0;
volatile int gZ80IsInHALT = 0;

void BUS_acquire_bus()
{
    set_GPIO_iotype(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN, GPIO_MODE_OUTPUT_PP);
    set_GPIO_value(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK, BUS_BUSRQ_ACTIVE);
    while(HAL_GPIO_ReadPin(BUS_BUSAK_PORT, BUS_BUSAK_PIN_MASK));
}

void BUS_release_bus()
{
    set_GPIO_value(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK, BUS_BUSRQ_INACTIVE);
    while(!HAL_GPIO_ReadPin(BUS_BUSAK_PORT, BUS_BUSAK_PIN_MASK));
    set_GPIO_iotype(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN, GPIO_MODE_INPUT);
}

typedef struct GPIOLine {
    GPIO_TypeDef* gpio;
    int pin; 
} GPIOLine;

GPIOLine address_lines[] = {
    {GPIOA, 14}, // A0
    {GPIOA, 13}, // A1
    {GPIOC, 5}, // A2
    {GPIOB, 3}, // A3
    {GPIOB, 10}, // A4
    {GPIOB, 9}, // A5
    {GPIOB, 8}, // A6
    {GPIOC, 4}, // A7
    {GPIOC, 9}, // A8
    {GPIOB, 4}, // A9
    {GPIOC, 13}, // A10
    {GPIOC, 12}, // A11
    {GPIOC, 8}, // A12
    {GPIOC, 7}, // A13
    {GPIOC, 6}, // A14
    {GPIOC, 3}, // A15
};
int address_line_count = sizeof(address_lines) / sizeof(address_lines[0]);

unsigned int shuffle_address(unsigned int a)
{
    unsigned int A = 0, B = 0, C = 0;
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        int bit = (a >> i) & 0x01;
        if(line->gpio == GPIOA)
            A |= (bit << line->pin);
        else if(line->gpio == GPIOB)
            B |= (bit << line->pin);
        else if(line->gpio == GPIOC)
            C |= (bit << line->pin);
    }
    unsigned int ddr = ((A & 0x6000) << 1) | ((B & 0x0700) >> 8) | ((B & 0x0018) << 7) | (C & 0x33F8);
    return ddr;
}

#define BUS_IO_MASK (BUS_IORQ_PIN_MASK | BUS_RD_PIN_MASK | BUS_WR_PIN_MASK | BUS_A7_PIN_MASK)

#define IO_BOARD_ADDR   0
#define IO_BOARD_ADDR_PINS   (IO_BOARD_ADDR & BUS_A7_PIN_MASK)

const unsigned int gREAD_IO_Signals = BUS_WR_INACTIVE | BUS_IORQ_ACTIVE | BUS_RD_ACTIVE | IO_BOARD_ADDR_PINS;
const unsigned int gWRITE_IO_Signals = BUS_WR_ACTIVE | BUS_IORQ_ACTIVE | BUS_RD_INACTIVE | IO_BOARD_ADDR_PINS;

void BUS_set_DATA_as_input()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x0;          // INPUT
}

void BUS_set_DATA_as_output()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x5555;       // OUTPUT
}

unsigned char BUS_get_DATA()
{
    return GPIOA->IDR & 0xff;
}

void BUS_set_DATA(unsigned char data)
{
    GPIOA->ODR = (GPIOA->ODR & ~0xff) | data;
}

void ccmram_set(unsigned int address, unsigned char b)
{
    ((unsigned char *)0x10000000)[address] = b;
}

unsigned char ccmram_get(unsigned int address)
{
    return ((unsigned char *)0x10000000)[address];
}

unsigned int BUS_compute_ADDRESS(unsigned int A, unsigned int B, unsigned C)
{
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
    unsigned int address = ((A & 0x6000) << 1) | ((B & 0x0700) >> 8) | ((B & 0x0018) << 7) | (C & 0x33F8);
    return address;
}

unsigned int BUS_get_ADDRESS()
{
    unsigned int A = GPIOA->IDR;
    unsigned int B = GPIOB->IDR;
    unsigned int C = GPIOC->IDR;
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
    unsigned int address = ((A & 0x6000) << 1) | ((B & 0x0700) >> 8) | ((B & 0x0018) << 7) | (C & 0x33F8);
    return address;
}

int gUnclaimedWrite = 0;
unsigned int gUnclaimedWriteAddress;
unsigned int gUnclaimedWriteData;
int gUnclaimedRead = 0;
unsigned int gUnclaimedReadAddress;
int gReadWasAlreadyInactive = 0;

void EXTI1_IRQHandler(void)
{
    if((BUS_SIGNAL_CHECK_PORT->IDR & BUS_IO_MASK) == gREAD_IO_Signals) {

        // Put this here even before clearing interrupt so it happens
        // as soon as possible.
        BUS_set_DATA(gNextByteForReading);
        BUS_set_DATA_as_output();

        if((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_INACTIVE)
            gReadWasAlreadyInactive = 1;

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        while((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_ACTIVE); /* busy wait for RD to rise */
        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
        BUS_set_DATA_as_input();

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);

        if(response_index >= response_length) {
            response_index = 0;
            response_length = 0;
            response_waiting = 0;
            gNextByteForReading = IOBOARD_POLL_AGAIN;
        } else if(response_length > 0) {
            gNextByteForReading = response_bytes[response_index++];
        } 

    } else if((BUS_MREQ_PORT->IDR & BUS_MREQ_PIN_MASK) == BUS_MREQ_ACTIVE) {
        // Memory read
        unsigned int address = BUS_get_ADDRESS();
        unsigned char byte = ccmram_get(address);

        BUS_set_DATA(byte);
        BUS_set_DATA_as_output();

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        while((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_ACTIVE); /* busy wait for RD to rise */
        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
        BUS_set_DATA_as_input();

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);

    } else {

        gUnclaimedRead = 1;
        gUnclaimedReadAddress = BUS_get_ADDRESS();

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);
    }
}

void EXTI2_IRQHandler(void)
{
    unsigned int initialMREQ = BUS_MREQ_PORT->IDR;
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
    unsigned char d = BUS_get_DATA();
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

    if((BUS_IORQ_PORT->IDR & BUS_IORQ_PIN_MASK) == BUS_IORQ_ACTIVE) {

        if((BUS_SIGNAL_CHECK_PORT->IDR & BUS_IO_MASK) == gWRITE_IO_Signals) {
            command_bytes[command_length++] = d;
        }

    } else {

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        unsigned int A = GPIOA->IDR;
        unsigned int B = GPIOB->IDR;
        unsigned int C = GPIOC->IDR;

        __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

        // Memory write
        if((initialMREQ & BUS_MREQ_PIN_MASK) == BUS_MREQ_ACTIVE) {
            unsigned int address = BUS_compute_ADDRESS(A, B, C);
            ccmram_set(address, d);
        } else {
            gUnclaimedWrite = 1;
            gUnclaimedWriteAddress = BUS_compute_ADDRESS(A, B, C);
            gUnclaimedWriteData = d;
        }
    }

    __HAL_GPIO_EXTI_CLEAR_IT(BUS_WR_PIN_MASK);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

void BUS_set_ADDRESS_as_output()
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_iotype(line->gpio, line->pin, GPIO_MODE_OUTPUT_PP);
    }
}

void BUS_set_ADDRESS_as_input()
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_iotype(line->gpio, line->pin, GPIO_MODE_INPUT);
    }
}

__attribute__((optimize("unroll-loops")))
void BUS_set_ADDRESS(unsigned int a)
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_value(line->gpio, 0x1U << line->pin, (a >> i) & 0x01);
    }
}

// Caller has to guarantee A and D access will not collide with
// another peripheral
// Caller has to save and restore D if desired
// Caller has to BUS_start() before and BUS_finish() after
// Caller has to set DATA as output
void BUS_write_memory_byte(unsigned int a, unsigned char d)
{
    BUS_set_ADDRESS(a);
    BUS_set_DATA(d);

    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_ACTIVE);
    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_ACTIVE);

    DWT_Delay(MEMORY_DELAY_MICROS);

    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_INACTIVE);
    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_INACTIVE);
}

// Caller has to guarantee A and D access will not collide with
// another peripheral
// Caller has to save and restore D if desired
// Caller has to BUS_start() before and BUS_finish() after
// Caller has to set DATA as input
unsigned char BUS_read_memory_byte(unsigned int a)
{
    BUS_set_ADDRESS(a);

    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_ACTIVE);
    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_ACTIVE);

    DWT_Delay(MEMORY_DELAY_MICROS);
    unsigned char d = BUS_get_DATA();

    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_WR_ACTIVE);
    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_INACTIVE);

    return d;
}

// Caller has to guarantee A and D access will not collide with
// another peripheral
// Caller has to save and restore D if desired
// Caller has to BUS_start() before and BUS_finish() after
// Caller has to set DATA as output
void BUS_write_io_byte(unsigned int a, unsigned char d)
{
    BUS_set_ADDRESS(a);
    BUS_set_DATA(d);

    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_ACTIVE);
    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_ACTIVE);

    DWT_Delay(IO_DELAY_MICROS);

    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_INACTIVE);
    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_INACTIVE);
}

// Caller has to guarantee A and D access will not collide with
// another peripheral
// Caller has to save and restore D if desired
// Caller has to BUS_start() before and BUS_finish() after
// Caller has to set DATA as input
unsigned char BUS_read_io_byte(unsigned int a)
{
    BUS_set_ADDRESS(a);

    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_ACTIVE);
    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_ACTIVE);

    DWT_Delay(IO_DELAY_MICROS);
    unsigned char d = BUS_get_DATA();

    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_INACTIVE);
    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_INACTIVE);

    return d;
}

void BUS_mastering_start()
{
    if(!gZ80IsInRESET && !gZ80IsInHALT)
        BUS_acquire_bus();
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
    if(!gZ80IsInRESET && !gZ80IsInHALT)
        BUS_release_bus();
}

void BUS_init()
{
    command_clear();
    response_clear();

    GPIO_InitTypeDef  GPIO_InitStruct;

    // configure PORT A0:A7 (bus D0..D7) outputs for later
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~0xffff) | 0x0000;   // LOW
    GPIOA->OTYPER = (GPIOA->OTYPER & 0xff) | 0x0000;        // PUSH_PULL
    GPIOA->PUPDR = (GPIOA->PUPDR & 0xffff) | 0x0000;        // no PUPD

    BUS_set_DATA_as_input();

    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

    // RD and WR are port C pins 1, 2 as inputs driving interrupts
    GPIO_InitStruct.Pin = BUS_RD_PIN_MASK | BUS_WR_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUS_SIGNAL_CHECK_PORT, &GPIO_InitStruct); 
    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_INACTIVE);
    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_INACTIVE);

    /* Enable and set EXTI Line1 Interrupt to the highest priority */
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    /* Enable and set EXTI Line2 Interrupt to the highest priority */
    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    // IORQ and A7 are port C pins 0, 4, as inputs
    GPIO_InitStruct.Pin = BUS_IORQ_PIN_MASK | BUS_A7_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_SIGNAL_CHECK_PORT, &GPIO_InitStruct); 
    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_INACTIVE);

    // MREQ
    GPIO_InitStruct.Pin = BUS_MREQ_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_MREQ_PORT, &GPIO_InitStruct); 
    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_INACTIVE);

    // HALT
    GPIO_InitStruct.Pin = BUS_HALT_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_HALT_PORT, &GPIO_InitStruct); 

    // BUSRQ
    GPIO_InitStruct.Pin = BUS_BUSRQ_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_BUSRQ_PORT, &GPIO_InitStruct); 
    set_GPIO_value(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK, BUS_BUSRQ_INACTIVE);

    // BUSAK
    GPIO_InitStruct.Pin = BUS_BUSAK_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_BUSAK_PORT, &GPIO_InitStruct); 

    // Address bus pins
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        GPIO_InitStruct.Pin = 1U << line->pin;
        HAL_GPIO_Init(line->gpio, &GPIO_InitStruct); 
    }
}

// Caller has to guarantee A and D access will not collide with
// another peripheral; basically either Z80 BUSRQ or RESET
void BUS_read_memory_block(unsigned int a, unsigned int l, unsigned char *b)
{
    if(0) {
        BUS_mastering_start();
        BUS_set_DATA_as_input();

        for(unsigned int u = a; u < a + l; u++)
            b[u - a] = BUS_read_memory_byte(u);

        BUS_set_DATA(gNextByteForReading);
        BUS_mastering_finish();
    } else {

        for(unsigned int u = a; u < a + l; u++)
            b[u - a] = ccmram_get(shuffle_address(u));

    }
}

// Caller has to guarantee A and D access will not collide with
// another peripheral; basically either Z80 BUSRQ or RESET
void BUS_write_IO(int io, unsigned char byte)
{
    BUS_mastering_start();
    BUS_set_DATA(0);
    BUS_set_DATA_as_output();

    BUS_write_io_byte(io, byte);

    BUS_set_DATA_as_input();
    BUS_set_DATA(gNextByteForReading);
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

    if(1) {

        for(unsigned int a = 0; a < romimage_length; a++)
            ccmram_set(shuffle_address(a), romimage_bytes[a]);

        for(unsigned int a = 0; a < romimage_length; a++) {
            unsigned char t = ccmram_get(shuffle_address(a));
            if(t != romimage_bytes[a]) {
                printf("BUS_write_ROM_image : expected 0x%02X byte at RAM address 0x%04X, read 0x%02X\n", romimage_bytes[a], a, t);
                succeeded = 0;
                break;
            }
        }
    } else {

        BUS_mastering_start();
        BUS_set_ADDRESS(0);
        BUS_set_DATA(0);
        BUS_set_DATA_as_output();

        for(unsigned int a = 0; a < romimage_length; a++)
            BUS_write_memory_byte(a, romimage_bytes[a]);

        BUS_set_DATA_as_input();

        for(unsigned int a = 0; a < romimage_length; a++) {
            unsigned char t = BUS_read_memory_byte(a);
            if(t != romimage_bytes[a]) {
                printf("BUS_write_ROM_image : expected 0x%02X byte at RAM address 0x%04X, read 0x%02X\n", romimage_bytes[a], a, t);
                succeeded = 0;
                break;
            }
        }

        BUS_set_DATA(gNextByteForReading);
        BUS_mastering_finish();
    }


    return succeeded;
}


/*--------------------------------------------------------------------------*/
// IO writes

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
    BUS_set_DATA(gNextByteForReading);
    BUS_mastering_finish();
}


//----------------------------------------------------------------------------
// stdio

#define OUTPUT_TO_SERIAL        0x01
#define OUTPUT_TO_VIDEO         0x02

int gOutputDevices = OUTPUT_TO_SERIAL;

void __io_putchar( char c )
{
    if(gOutputDevices & OUTPUT_TO_SERIAL)
        serial_enqueue_one_char(c);
    if(gOutputDevices & OUTPUT_TO_VIDEO) {
        if(c == '\n') // XXX
            BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, '\r');
        BUS_write_IO(VIDEO_BOARD_OUTPUT_ADDR, c);
    }
}

void logprintf(int level, char *fmt, ...)
{
    va_list args;
    static char dummy[512];

    if(level > gDebugLevel)
        return;

    va_start(args, fmt);
    vsprintf(dummy, fmt, args);
    va_end(args);
    
    char *s = dummy;
    while(*s) {
        putchar(*s++);
    }
    serial_flush();

    if(!(gOutputDevices & OUTPUT_TO_VIDEO) && (level <= DEBUG_WARNINGS))
        VIDEO_output_string(dummy, 1);
}


//----------------------------------------------------------------------------
// Alice 3 Bus /RESET

#define RESET_BUTTON_PORT GPIOB
#define RESET_BUTTON_PIN_MASK GPIO_PIN_5
#define RESET_BUTTON_IRQn EXTI9_5_IRQn
#define RESET_BUTTON_DELAY_MS 10

#define Z80_RESET_PORT GPIOB
#define Z80_RESET_PIN_MASK GPIO_PIN_0
#define Z80_RESET_ACTIVE 0
#define Z80_RESET_INACTIVE Z80_RESET_PIN_MASK
#define Z80_RESET_DURATION_MS 10

void BUS_reset_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = Z80_RESET_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Z80_RESET_PORT, &GPIO_InitStruct); 
    HAL_GPIO_WritePin(Z80_RESET_PORT, Z80_RESET_PIN_MASK, Z80_RESET_ACTIVE);
    gZ80IsInRESET = 1;
}

void BUS_reset_start()
{
    HAL_GPIO_WritePin(Z80_RESET_PORT, Z80_RESET_PIN_MASK, Z80_RESET_ACTIVE);
    gZ80IsInRESET = 1;
    delay_ms(Z80_RESET_DURATION_MS);
}

void BUS_reset_finish()
{
    gZ80IsInRESET = 0;
    HAL_GPIO_WritePin(Z80_RESET_PORT, Z80_RESET_PIN_MASK, Z80_RESET_INACTIVE);
}

volatile int gResetTheZ80 = 0;

void RESET_BUTTON_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = RESET_BUTTON_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RESET_BUTTON_PORT, &GPIO_InitStruct); 

    /* Enable and set interrupt priority */
    HAL_NVIC_SetPriority(RESET_BUTTON_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(RESET_BUTTON_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(RESET_BUTTON_PIN_MASK);
    NVIC_ClearPendingIRQ(RESET_BUTTON_IRQn);

    gResetTheZ80 = 1;
}

void Z80_reset()
{
    BUS_reset_start();

    response_clear();
    command_clear();

    BUS_write_ROM_image(gZ80BootImage, gZ80BootImageLength);

    VIDEO_start_clock();

    BUS_reset_finish();
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

#define SPI_SS_PIN_MASK      GPIO_PIN_8
#define SPI_SS_PORT     GPIOA

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
    GPIO_InitStruct.Pin = SPI_SS_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // XXX change to match SPI rate?
    HAL_GPIO_Init(SPI_SS_PORT, &GPIO_InitStruct); 
    SPI_disable_sd();
}


/*--------------------------------------------------------------------------*/
/* SD card -----------------------------------------------------------------*/

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

int gSDCardTimeoutMillis = 1000;

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

void spi_writen(const unsigned char *buffer, unsigned int nlen)
{
    if(gDebugLevel >= DEBUG_INSANE) {
        printf("spi_writen write:\n");
        dump_buffer_hex(4, buffer, nlen);
    }
    int result = HAL_SPI_Transmit(&gSPIHandle, (unsigned char *)buffer, nlen, 1000);
    if(result != HAL_OK){
        logprintf(DEBUG_ERRORS, "spi_writen: SPI error 0x%04X\n", result);
        panic();
    }
}

void spi_readn(unsigned char *buffer, unsigned int nlen)
{
    memset(buffer, 0xff, nlen);
    int result = HAL_SPI_TransmitReceive(&gSPIHandle, buffer, buffer, nlen, 1000);
    if(result != HAL_OK){
        logprintf(DEBUG_ERRORS, "spi_readn: SPI error 0x%04X\n", result);
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

    logprintf(DEBUG_DATA, "command constructed: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    spi_bulk(command_buffer, sizeof(command_buffer));
    logprintf(DEBUG_ALL, "returned in buffer: %02X %02X %02X %02X %02X %02X\n",
        command_buffer[0], command_buffer[1], command_buffer[2],
        command_buffer[3], command_buffer[4], command_buffer[5]);

    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "sdcard_send_command: timed out waiting on response\n");
            panic();
        }
        response[0] = 0xff;
        spi_bulk(response, 1);
        logprintf(DEBUG_ALL, "response 0x%02X\n", response[0]);
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
    SPI_enable_sd();
    /* interface init */
    if(!sdcard_send_command(CMD0, 0, response, 1))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        logprintf(DEBUG_WARNINGS, "sdcard_init: failed to enter IDLE mode, response was 0x%02X\n", response[0]);
        return 0;
    }
    delay_ms(100);

    /* check voltage */
    if(!sdcard_send_command(CMD8, 0x000001AA, response, 5))
        return 0;
    if(response[0] != gSDCardResponseIDLE) {
        logprintf(DEBUG_WARNINGS, "sdcard_init: failed to get OCR, response was 0x%02X\n", response[0]);
        return 0;
    }
    OCR = (((unsigned long)response[1]) << 24) | (((unsigned long)response[2]) << 16) | (((unsigned long)response[3]) << 8) | (((unsigned long)response[4]) << 0);
    logprintf(DEBUG_DATA, "sdcard_init: OCR response is 0x%08lX\n", OCR);

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
            logprintf(DEBUG_WARNINGS, "sdcard_init: not in IDLE mode for CMD55, response was 0x%02X\n", response[0]);
            return 0;
        }
        /* start initialization process, set HCS (high-capacity) */
        if(!sdcard_send_command(ACMD41, 0x40000000, response, 1))
            return 0;
    } while(response[0] != gSDCardResponseSUCCESS);
    logprintf(DEBUG_ALL, "returned from ACMD41: %02X\n", response[0]);

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
        logprintf(DEBUG_ERRORS, "sdcard_readblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait for the data token.
    int then = HAL_GetTick();
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "sdcard_readblock: timed out waiting for data token\n");
            return 0;
        }
        spi_readn(response, 1);
        logprintf(DEBUG_ALL, "readblock response 0x%02X\n", response[0]);
    } while(response[0] != gSDCardToken_17_18_24);

    // Read data.
    spi_readn(block, BLOCK_SIZE);

    // Read CRC
    spi_readn(response, 2);
    logprintf(DEBUG_DATA, "CRC is 0x%02X%02X\n", response[0], response[1]);

    unsigned short crc_theirs = response[0] * 256 + response[1];

    // calculate our version of CRC and compare
    unsigned short crc_ours = crc_itu_t(0, block, BLOCK_SIZE);

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
            logprintf(DEBUG_ERRORS, "sdcard_readblock: timed out waiting on completion\n");
            return 0;
        }
        spi_readn(response, 1);
        logprintf(DEBUG_ALL, "readblock response 0x%02X\n", response[0]);
    } while(response[0] != 0xFF);

    if(gDebugLevel >= DEBUG_ALL) dump_more_spi_bytes("read completion");

    return 1;
}

/* precondition: SDcard CS is low (active) */
int sdcard_writeblock(unsigned int blocknum, const unsigned char *block)
{
    int count;
    unsigned char response[8];

    // Send write block command.
    if(!sdcard_send_command(CMD24, blocknum, response, 1))
        return 0;
    if(response[0] != gSDCardResponseSUCCESS) {
        logprintf(DEBUG_ERRORS, "sdcard_writeblock: failed to respond with SUCCESS, response was 0x%02X\n", response[0]);
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
    logprintf(DEBUG_DATA, "writeblock response 0x%02X\n", response[0]);
    if(response[0] != gSDCardResponseDATA_ACCEPTED) {
        logprintf(DEBUG_ERRORS, "sdcard_writeblock: failed to respond with DATA_ACCEPTED, response was 0x%02X\n", response[0]);
        return 0;
    }

    // Wait while busy (DO = low).
    int then = HAL_GetTick();
    count = 0;
    do {
        int now = HAL_GetTick();
        if(now - then > gSDCardTimeoutMillis) {
            logprintf(DEBUG_ERRORS, "sdcard_writeblock: timed out waiting on completion\n");
            panic();
        }
        spi_readn(response, 1);
        logprintf(DEBUG_ALL, "writeblock completion 0x%02X\n", response[0]);
        count++;
    } while(response[0] != 0xFF);
    logprintf(DEBUG_DATA, "read %d SPI bytes waiting on write to complete.\n", count);

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


//----------------------------------------------------------------------------
// FatFS device routines

DSTATUS disk_status (BYTE pdrv)
{
    if(pdrv != 0)
        return STA_NODISK;

    return 0;
}

DSTATUS disk_initialize (BYTE pdrv)
{
    if(pdrv != 0)
        return RES_ERROR;

    return RES_OK;
}

DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector,	UINT count)
{
    if(pdrv != 0)
        return RES_ERROR;

    for(int i = 0; i < count; i++)
        if(!sdcard_readblock(sector + i, buff + BLOCK_SIZE * i))  {
            logprintf(DEBUG_ERRORS, "ERROR: failed reading SD block %d\n", sector + i);
            return RES_ERROR;
        }
    
    return RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if(pdrv != 0)
        return RES_ERROR;

    for(int i = 0; i < count; i++)
        if(!sdcard_writeblock(sector + i, buff + BLOCK_SIZE * i)) {
            logprintf(DEBUG_ERRORS, "ERROR: failed reading SD block %d\n", sector + i);
            return RES_ERROR;
        }
    
    return RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_OK;

    if(pdrv != 0)
        return RES_ERROR;

    switch(cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        default:
            logprintf(DEBUG_ERRORS, "ERROR: unexpected FatFS ioctl %d\n", cmd);
            result = RES_ERROR;
            break;
    }

    return result;
}

DWORD get_fattime(void)
{
    // returns FatFS formatted date-time
    // hardcoded to Mar 15 2016, midnight
    return
        ((_NORTC_YEAR - 1980) << 25) | 
        (_NORTC_MON << 21) | 
        (_NORTC_MDAY << 16) | 
        (0 << 11) | 
        (0 << 5) | 
        (0 << 0)
        ;
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

        if(f_size(&gDiskImageFiles[i]) != BLOCKS_PER_DISK * BLOCK_SIZE) {
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

            if(!sdcard_init())
                printf("Failed to start access to SD card as SPI\n");

        } else if(strcmp(gMonitorCommandLine, "dumpkbd") == 0) {

            dump_keyboard_data = !dump_keyboard_data;
            if(dump_keyboard_data)
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

        } else if(strcmp(gMonitorCommandLine, "low128") == 0) {

            BUS_reset_start();

            static unsigned char buf[128];
            BUS_read_memory_block(0, 128, buf);

            printf("low 128 bytes of RAM:\n");
            dump_buffer_hex(4, buf, sizeof(buf));

            BUS_reset_finish();

        } else if(strcmp(gMonitorCommandLine, "reset") == 0) {

            printf("Resetting Z-80 and communication buffers...\n");
            Z80_reset();

        } else if(strcmp(gMonitorCommandLine, "pass") == 0) {

            gSerialInputToMonitor = 0;
            printf("Press CTRL-A then CTRL-B to return to monitor.\n");

        } else if(strcmp(gMonitorCommandLine, "buffers") == 0) {

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
                SPI_enable_sd();
                printf("/SS to SD card is enabled (GND)\n");
            } else {
                SPI_disable_sd();
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
        response_append(IOBOARD_FAILURE);
        return;
    }

    FRESULT result = f_lseek(&gDiskImageFiles[disk], lba_sector * SECTOR_SIZE);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't seek to sector %d, track %d in \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        response_append(IOBOARD_FAILURE);
        return;
    }

    UINT wasread;
    result = f_read(&gDiskImageFiles[disk], buffer, SECTOR_SIZE, &wasread);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: couldn't read sector %d, track %d from \"%s\", FatFS result %d\n", sector, track, gDiskImageFilenames[disk], result);
        response_append(IOBOARD_FAILURE);
        return;
    }
    if(wasread != SECTOR_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to read %lu bytes from sector %d, track %d of \"%s\", only read %ld\n", SECTOR_SIZE, gDiskImageFilenames[disk], sector, track, wasread);
        response_append(IOBOARD_FAILURE);
        return;
    }
    if(gDebugLevel >= DEBUG_ALL) dump_buffer_hex(4, buffer, SECTOR_SIZE);

    response_append(IOBOARD_SUCCESS);

    for(unsigned int u = 0; u < SECTOR_SIZE; u++)
        response_append(buffer[u]);

    if(command_request == IOBOARD_CMD_READ_SUM) {
        unsigned short sum = 0;

        for(unsigned int u = 0; u < SECTOR_SIZE; u++)
            sum += buffer[u];

        response_append(sum & 0xff);
        response_append((sum >> 8) & 0xff);
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
        response_append(IOBOARD_FAILURE);
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
            response_append(IOBOARD_FAILURE);
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
        response_append(IOBOARD_FAILURE);
        return;
    }
    if(waswritten != SECTOR_SIZE) {
        logprintf(DEBUG_ERRORS, "ERROR: tried to write %lu bytes to sector %d, track %d of \"%s\", only read %ld\n", SECTOR_SIZE, gDiskImageFilenames[disk], sector, track, waswritten);
        response_append(IOBOARD_FAILURE);
        return;
    }
    // XXX Should probably set this up as a timeout, so multiple
    // sectors writes can take advantage of single block
    f_sync(&gDiskImageFiles[disk]);

    response_append(IOBOARD_SUCCESS);
}

void check_and_process_command()
{
    unsigned char isEmpty;

    if(command_length == 0)
        return;

    unsigned char command = command_bytes[0];

    if((command < IOBOARD_CMD_MIN) || (command > IOBOARD_CMD_MAX)) {

        logprintf(DEBUG_ERRORS, "ERROR: Unknown command 0x%02X received\n", command);
        command_clear();
        return;
    }

    if(command_length < command_lengths[command])
        return;

    logprintf(DEBUG_EVENTS, "complete command received.\n");
    if(command_length > command_lengths[command]) {
        logprintf(DEBUG_ERRORS, "ERROR: command buffer longer than expected for command.\n");
    }

    if(command == IOBOARD_CMD_NONE)
        return;

    response_start();
    LED_set_info(1);
    switch(command) {
        case IOBOARD_CMD_READ:
        case IOBOARD_CMD_READ_SUM:
            process_command_read(command, command_bytes);
            break;

        case IOBOARD_CMD_WRITE:
        case IOBOARD_CMD_WRITE_SUM:
            process_command_write(command, command_bytes);
            break;

        case IOBOARD_CMD_SEROUT: {
            putchar(command_bytes[1]);
            response_append(IOBOARD_SUCCESS);
            break;
        }

        case IOBOARD_CMD_CONST: {
            logprintf(DEBUG_EVENTS, "CONST\n");
            isEmpty = queue_isempty(&con_queue.q);
            if(!isEmpty)
                response_append(IOBOARD_READY);
            else
                response_append(IOBOARD_NOT_READY);
            break;
        }

        case IOBOARD_CMD_CONIN: {
            unsigned char c;
            logprintf(DEBUG_EVENTS, "CONIN\n");
            isEmpty = queue_isempty(&con_queue.q);
            if(!isEmpty) {
                c = queue_deq(&con_queue.q);
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
            printf("unexpected command 0x%02X from z80!\n", command);
            break;
        }
    }
    LED_set_info(0);

    if(response_staging_length > 0) {
        logprintf(DEBUG_DATA, "will respond with %d\n", response_staging_length);
        command_clear();
        disable_interrupts();
        response_finish();
        enable_interrupts();
        gResponseWasWaiting = 1;
    }
}

void process_keyboard_queue()
{
    int isEmpty = queue_isempty(&kbd_queue.q);
    if(!isEmpty) {
        unsigned char kb = queue_deq(&kbd_queue.q);
        if(dump_keyboard_data)
            logprintf(DEBUG_DATA, "keyboard scan code: %02X\n", kb);
        kbd_process_byte(kb);
    }
}

void check_and_process_HALT()
{
    static int Z80WasInHALT = 0;
    gZ80IsInHALT = !HAL_GPIO_ReadPin(BUS_HALT_PORT, BUS_HALT_PIN_MASK);
    if(gZ80IsInHALT != Z80WasInHALT) {
        gOutputDevices = OUTPUT_TO_VIDEO | OUTPUT_TO_SERIAL;
        if(gZ80IsInHALT) {
            logprintf(DEBUG_EVENTS, "Z80 has HALTed.\n");
        } else {
            logprintf(DEBUG_EVENTS, "Z80 has exited HALT state.\n");
        }
        gOutputDevices = OUTPUT_TO_SERIAL;
        Z80WasInHALT = gZ80IsInHALT;
    }
}

void check_and_process_soft_reset()
{
    if(gResetTheZ80 || HAL_GPIO_ReadPin(RESET_BUTTON_PORT, RESET_BUTTON_PIN_MASK)) {
        while(HAL_GPIO_ReadPin(RESET_BUTTON_PORT, RESET_BUTTON_PIN_MASK));
        delay_ms(RESET_BUTTON_DELAY_MS);// software debounce
        gResetTheZ80 = 0;

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
                console_enqueue_key(c);
            }
        }
    }
}

void process_serial_polling()
{
    // This is terrible; UART interrupt should fill a buffer and we should examine in here, not poll
    if(gStartAnotherUARTReceive) {
        gStartAnotherUARTReceive = 0;
        int result;
        if((result = HAL_UART_Receive_IT(&gUARTHandle, (uint8_t *)&gSerialCharBuffer, 1)) != HAL_OK) {
            /* error_code = gUARTHandle.State; */
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

#define PROP_READY_PIN_MASK GPIO_PIN_14
#define PROP_READY_PORT GPIOC

void VIDEO_init()
{
    // set PROP_READY pull up
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin = PROP_READY_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PROP_READY_PORT, &GPIO_InitStruct); 
}

void VIDEO_wait()
{
    int started = HAL_GetTick();
    int then = HAL_GetTick();
    while(HAL_GPIO_ReadPin(PROP_READY_PORT, PROP_READY_PIN_MASK)) {
        int now = HAL_GetTick();
        if(now - then > 500) {
            printf("Waited 500 more milliseconds for PROP_READY\n");
            then = now;
            serial_flush();
        }
        if(now - started > 5000) {
            printf("Waited 5 seconds for PROP_READY\n");
            serial_flush();
            panic();
        }
    }
}

int gStandaloneARM = 0;

int main()
{
    system_init();

    queue_init(&mon_queue.q, MON_QUEUE_CAPACITY);
    queue_init(&kbd_queue.q, KBD_QUEUE_CAPACITY);
    queue_init(&con_queue.q, CON_QUEUE_CAPACITY);

    LED_init();
    LED_beat_heart();

    setbuf(stdout, NULL);

    SERIAL_init(); // transmit and receive but global interrupts disabled
    LED_beat_heart();

    printf("\n\nAlice 3 I/O firmware, %s\n", IOBOARD_FIRMWARE_VERSION_STRING);
    printf("System core clock: %lu MHz\n", SystemCoreClock / 1000000);

    LED_beat_heart();
    serial_flush();

    RESET_BUTTON_init();

    if(!gStandaloneARM) {
        VIDEO_init();
        VIDEO_wait();

        BUS_reset_init();
        BUS_init();

        VIDEO_output_string("Alice 3 I/O firmware, " IOBOARD_FIRMWARE_VERSION_STRING "\r\n", 0);
    }

    SPI_config_for_sd();
    LED_beat_heart();

    if(!sdcard_init())
        printf("Failed to start access to SD card as SPI!!\n");
    else 
        printf("SD Card interface is initialized for SPI\n");
    LED_beat_heart();
    serial_flush();

    if(0) {
        test_sd_card();
        LED_beat_heart();
    }

    FRESULT result = f_mount(&gFATVolume, "0:", 1);
    if(result != FR_OK) {
        logprintf(DEBUG_ERRORS, "ERROR: FATFS mount result is %d\n", result);
        panic();
    } else {
        printf("Mounted FATFS from SD card successfully.\n");
    }
    serial_flush();

    int success = read_bootrom();
    if(!success) {
        panic();
    }
    LED_beat_heart();

    success = read_disk_image_list();
    if(!success) {
        panic();
    }
    for(int i = 0; i < gDiskImageCount; i++) {
        printf("disk %c: \"%s\"\n", 'A' + i, gDiskImageFilenames[i]);
    }
    success = open_disk_images();
    if(!success) {
        panic();
    }
    LED_beat_heart();

    {
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
        VIDEO_start_clock();
        delay_ms(1); // XXX delay for at least 4 Z80 clock cycles, maybe 10us
        BUS_reset_finish();
    }

    printf("* ");
    serial_flush();

    for(;;) {

        serial_try_to_transmit_buffers();
        LED_beat_heart();

        process_serial_polling();

        process_monitor_queue();

        check_and_process_command();

        process_keyboard_queue();

        check_exceptional_conditions();

        if(gResponseWasWaiting && !response_waiting) {
            logprintf(DEBUG_EVENTS, "response packet was read\n");
            gResponseWasWaiting = 0;
        }

        check_and_process_soft_reset();

        check_and_process_HALT();
    }

    // should not reach
    panic();
}
