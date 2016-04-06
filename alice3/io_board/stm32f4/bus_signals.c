#include "gpio_helpers.h"
#include "delay.h"
#include "bus_signals.h"

//----------------------------------------------------------------------------
// Alice 3 system bus

// IORQ, RD, WR, and A7 are specially implemented to be on one GPIO register
// in order to quickly determine if IO is being read or written

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
int address_line_count = sizeof(address_lines) / sizeof(address_lines[0]);

__attribute__((optimize("unroll-loops")))
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

__attribute__((optimize("unroll-loops")))
void BUS_set_ADDRESS_as_output()
{
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        set_GPIO_iotype(line->gpio, line->pin, GPIO_MODE_OUTPUT_PP);
    }
}

__attribute__((optimize("unroll-loops")))
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

void BUS_reset_start()
{
    HAL_GPIO_WritePin(BUS_RESET_PORT, BUS_RESET_PIN_MASK, BUS_RESET_ACTIVE);
    delay_ms(BUS_RESET_DURATION_MS);
}

void BUS_reset_finish()
{
    HAL_GPIO_WritePin(BUS_RESET_PORT, BUS_RESET_PIN_MASK, BUS_RESET_INACTIVE);
}

// /RESET is initially set ACTIVE
void BUS_init()
{
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

    // /RESET
    GPIO_InitStruct.Pin = BUS_RESET_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUS_RESET_PORT, &GPIO_InitStruct); 
    HAL_GPIO_WritePin(BUS_RESET_PORT, BUS_RESET_PIN_MASK, BUS_RESET_ACTIVE);

    // Address bus pins
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        GPIO_InitStruct.Pin = 1U << line->pin;
        HAL_GPIO_Init(line->gpio, &GPIO_InitStruct); 
    }
}

