#include <stm32f4xx_hal.h>
#include "defs.h"
#include "delay.h"
#include "keyboard.h"
#include "logprintf.h"
#include "ps2_keyboard.h"
#include "byte_queue.h"

//----------------------------------------------------------------------------
// AT and PS/2 Keyboard processing

volatile int gKeyboardRawData = 0;

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

static inline unsigned char KBD_data_code(short data)
{
    return (data >> 1) & 0xff;
}

static inline int KBD_data_parity(short data)
{
    return (data >> 9) & 0x1;
}

volatile static short kbd_bits = 0;
volatile static unsigned short kbd_data = 0;

#define KBD_stop_bit 1
#define KBD_start_bit 0

#define HOST_TO_KBD_parity_error 0xFE
#define HOST_TO_KBD_command_success 0xFA

volatile int gKeyboardOverflowed = 0;
volatile int gKeyboardBATBadParity = 0;
volatile int gKeyboardParityError = 0;

void KBD_set_receive()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pull = GPIO_NOPULL;

    GPIO_InitStruct.Pin = KEYBOARD_CLOCK_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    HAL_GPIO_Init(KEYBOARD_CLOCK_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = KEYBOARD_DATA_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(KEYBOARD_DATA_PORT, &GPIO_InitStruct); 

    /* Enable and set EXTI Line15-10 interrupt */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void KBD_init()
{
    queue_init(&kbd_queue.q, KBD_QUEUE_CAPACITY);
    KBD_set_receive();
}

int KBD_sending = 0;
int KBD_next_CLK_is_ACK = 0;

int KBD_byte_odd_parity(int b)
{
    return 
        (1 ^
        (b >> 0) ^
        (b >> 1) ^
        (b >> 2) ^
        (b >> 3) ^
        (b >> 4) ^
        (b >> 5) ^
        (b >> 6) ^
        (b >> 7)) & 0x1;
}

void KBD_send_byte(int b)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

    HAL_GPIO_WritePin(KEYBOARD_CLOCK_PORT, KEYBOARD_CLOCK_PIN_MASK, 1);
    HAL_GPIO_WritePin(KEYBOARD_DATA_PORT, KEYBOARD_DATA_PIN_MASK, 1);

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = KEYBOARD_CLOCK_PIN_MASK;
    HAL_GPIO_Init(KEYBOARD_CLOCK_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = KEYBOARD_DATA_PIN_MASK;
    HAL_GPIO_Init(KEYBOARD_DATA_PORT, &GPIO_InitStruct); 

    // Set up all 11 bits we're going to send
    kbd_bits = 11;
    int parity = KBD_byte_odd_parity(b) ^ 0x01;
    kbd_data = (KBD_stop_bit << 10) | (parity << 9) | (b << 1) | (KBD_start_bit << 0);
    KBD_sending = 1;

    // Pull CLK low for at least 100 uS
    HAL_GPIO_WritePin(KEYBOARD_CLOCK_PORT, KEYBOARD_CLOCK_PIN_MASK, 0);
    delay_us(150);

    // Pull DATA low, release CLK
    int pin = (kbd_data & 0x01) ? 1 : 0;
    kbd_data >>= 1;
    HAL_GPIO_WritePin(KEYBOARD_DATA_PORT, KEYBOARD_DATA_PIN_MASK, pin);
    // At this point KBD_start_bit is on data line
    HAL_GPIO_WritePin(KEYBOARD_CLOCK_PORT, KEYBOARD_CLOCK_PIN_MASK, 1);

    // Restore CLK to input
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = KEYBOARD_CLOCK_PIN_MASK;
    HAL_GPIO_Init(KEYBOARD_CLOCK_PORT, &GPIO_InitStruct); 

    /* Enable EXTI Line15-10 interrupt */
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(KEYBOARD_CLOCK_PIN_MASK);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

    if(KBD_sending) {

        // Decrement kbd_bits for the bit just read by the keyboard
        --kbd_bits;

        if(kbd_bits > 0) {
            // If there are more bits to send, put them on the data line

            int pin = (kbd_data & 0x01) ? 1 : 0;
            HAL_GPIO_WritePin(KEYBOARD_DATA_PORT, KEYBOARD_DATA_PIN_MASK, pin);
            kbd_data >>= 1;

        } else {
            // If there aren't more bits, let go of data, mark that next
            // clock is an ACK from keyboard

            GPIO_InitTypeDef  GPIO_InitStruct;

            KBD_sending = 0;
            KBD_next_CLK_is_ACK = 1;
            kbd_data = 0;

            // Restore DATA to input
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = KEYBOARD_DATA_PIN_MASK;
            HAL_GPIO_Init(KEYBOARD_DATA_PORT, &GPIO_InitStruct); 

            // Should do this as interrupt on rise of CLK
            // Wait for clock to rise, meaning DATA has been read by device
            while(!HAL_GPIO_ReadPin(KEYBOARD_CLOCK_PORT, KEYBOARD_CLOCK_PIN_MASK));
        }

    } else {
        unsigned int pin = GPIOB->IDR & KEYBOARD_DATA_PIN_MASK;
        unsigned int bit = pin ? 1 : 0;

        if(KBD_next_CLK_is_ACK) {
            KBD_next_CLK_is_ACK = 0;
            return;
        }

        kbd_data = (kbd_data >> 1) | (bit << 10);
        kbd_bits++;

        if(kbd_bits == KBD_BIT_COUNT) {

            gKeyboardRawData = kbd_data;

            int byte = KBD_data_code(kbd_data);
            int parity = KBD_data_parity(kbd_data);

            kbd_data = 0;
            kbd_bits = 0;

            if(KBD_byte_odd_parity(byte) != parity) {

                if(byte == PS2_KBD_BAT) {
                    gKeyboardBATBadParity = 1;
                    KBD_send_byte(HOST_TO_KBD_parity_error);
                } else {
                    gKeyboardParityError = 256 + byte;
                }

            } else {

                if(byte == PS2_KBD_BAT) {
                    KBD_send_byte(HOST_TO_KBD_command_success);
                } else if(queue_isfull(&kbd_queue.q)) {
                    gKeyboardOverflowed = 1;
                } else {
                    queue_enq(&kbd_queue.q, byte);
                }
            }
        }
    }
}

void SERIAL_flush(void);

int KBD_process_queue(int dump_data)
{
    int key = -1;

    int isEmpty = queue_isempty(&kbd_queue.q);
    if(!isEmpty) {
        unsigned char kb = queue_deq(&kbd_queue.q);
        if(dump_data) {
            logprintf(DEBUG_DATA, "keyboard scan code: %02X\n", kb);
        }
        key = PS2_process_byte(dump_data, kb);
    }

    return key;
}


