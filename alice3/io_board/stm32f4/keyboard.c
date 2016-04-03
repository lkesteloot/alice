#include <stm32f4xx_hal.h>
#include "defs.h"
#include "keyboard.h"
#include "logprintf.h"
#include "ps2_keyboard.h"
#include "byte_queue.h"

//----------------------------------------------------------------------------
// AT and PS/2 Keyboard processing

#define KBD_QUEUE_CAPACITY 16

struct kbd_queue_struct {
    struct queue q;
    unsigned char queue[KBD_QUEUE_CAPACITY];
};
volatile struct kbd_queue_struct kbd_queue;

#define KEYBOARD_CLOCK_PIN_MASK GPIO_PIN_11
#define KEYBOARD_CLOCK_PORT GPIOB

#define KEYBOARD_DATA_PIN_MASK GPIO_PIN_12
#define KEYBOARD_DATA_PORT GPIOB

// Keyboard I/O constants
#define KBD_BIT_COUNT 11

volatile static short kbd_bits = 0;
volatile static unsigned short kbd_data = 0;

volatile unsigned char gKeyboardOverflowed = 0;

void KBD_init()
{
    queue_init(&kbd_queue.q, KBD_QUEUE_CAPACITY);

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

int KBD_process_queue(int dump_data)
{
    int key = -1;

    int isEmpty = queue_isempty(&kbd_queue.q);
    if(!isEmpty) {
        unsigned char kb = queue_deq(&kbd_queue.q);
        if(dump_data)
            logprintf(DEBUG_DATA, "keyboard scan code: %02X\n", kb);
        key = PS2_process_byte(dump_data, kb);
    }

    return key;
}


