#ifndef __BUS_SIGNALS_H__
#define __BUS_SIGNALS_H__

#include <stm32f4xx_hal.h>
#include "gpio_helpers.h"

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

#define BUS_RFSH_PIN_MASK GPIO_PIN_15
#define BUS_RFSH_PIN 15
#define BUS_RFSH_PORT GPIOC

#define BUS_RESET_PORT GPIOB
#define BUS_RESET_PIN_MASK GPIO_PIN_0
#define BUS_RESET_ACTIVE 0
#define BUS_RESET_INACTIVE BUS_RESET_PIN_MASK
#define BUS_RESET_DURATION_MS 10

#define BUS_WAIT_PORT GPIOD
#define BUS_WAIT_PIN_MASK GPIO_PIN_2
#define BUS_WAIT_PIN 2
#define BUS_WAIT_ACTIVE 0
#define BUS_WAIT_INACTIVE BUS_WAIT_PIN_MASK

typedef struct GPIOLine {
    GPIO_TypeDef* gpio;
    int pin; 
} GPIOLine;


static GPIOLine address_lines[] = {
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

#if ALICE3_VERSION == ALICE3_V3

    {GPIOC, 13}, // A10
    {GPIOC, 12}, // A11
    {GPIOC, 8}, // A12
    {GPIOC, 7}, // A13
    {GPIOC, 6}, // A14
    {GPIOC, 3}, // A15

#if ! ALICE3_V3_ARM_IS_RAM

    {GPIOD, 2}, // A16
    {GPIOC, 15}, // A17

#endif // ! ALICE_V3_ARM_IS_RAM

#endif // ALICE3_VERSION == ALICE3_V3
};
static int address_line_count = sizeof(address_lines) / sizeof(address_lines[0]);

__attribute__((optimize("unroll-loops")))
static inline unsigned int shuffle_address(unsigned int a)
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


void BUS_init();
void BUS_reset_finish();
void BUS_reset_start();

static inline void BUS_set_DATA_as_input()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x0;          // INPUT
}

static inline void BUS_set_DATA_as_output()
{
    GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x5555;       // OUTPUT
}

static inline unsigned char BUS_get_DATA()
{
    return GPIOA->IDR & 0xff;
}

static inline void BUS_set_DATA(unsigned char data)
{
    GPIOA->ODR = (GPIOA->ODR & ~0xff) | data;
}

__attribute__((optimize("unroll-loops")))
static inline void BUS_set_ADDRESS_as_output()
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_iotype(line->gpio, line->pin, GPIO_MODE_OUTPUT_PP);
    }
}

__attribute__((optimize("unroll-loops")))
static inline void BUS_set_ADDRESS_as_input()
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_iotype(line->gpio, line->pin, GPIO_MODE_INPUT);
    }
}

__attribute__((optimize("unroll-loops")))
static inline void BUS_set_ADDRESS(unsigned int a)
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_value(line->gpio, 0x1U << line->pin, (a >> i) & 0x01);
    }
}


#endif /* __BUS_SIGNALS_H__ */
