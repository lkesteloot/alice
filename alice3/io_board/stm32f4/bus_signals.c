#include "gpio_helpers.h"
#include "delay.h"
#include "bus_signals.h"

//----------------------------------------------------------------------------
// Alice 3 system bus

// IORQ, RD, WR, and A7 are specially implemented to be on one GPIO register
// in order to quickly determine if IO is being read or written

void BUS_reset_start()
{
    HAL_GPIO_WritePin(BUS_RESET_PORT, BUS_RESET_PIN_MASK, BUS_RESET_ACTIVE);
    delay_ms(BUS_RESET_DURATION_MS);
}

void BUS_reset_finish()
{
    HAL_GPIO_WritePin(BUS_RESET_PORT, BUS_RESET_PIN_MASK, BUS_RESET_INACTIVE);
}

// N.B. /RESET is initially set ACTIVE
void BUS_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    // configure PORT A0:A7 (bus D0..D7) outputs for later
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~0xffff) | 0x5555; // HIGH
    GPIOA->OTYPER = (GPIOA->OTYPER & 0xff) | 0x00;        // PUSH_PULL
    GPIOA->PUPDR = (GPIOA->PUPDR & 0xffff) | 0x0000;      // no PUPD

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

    // IORQ is port C pin 0 as input
    GPIO_InitStruct.Pin = BUS_IORQ_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUS_SIGNAL_CHECK_PORT, &GPIO_InitStruct); 
    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_INACTIVE);

    // A7 is port C pin 4 as input
    GPIO_InitStruct.Pin = BUS_A7_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_SIGNAL_CHECK_PORT, &GPIO_InitStruct); 

    // MREQ
    GPIO_InitStruct.Pin = BUS_MREQ_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
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
    GPIO_InitStruct.Pull = GPIO_PULLUP;
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

    // /WAIT output on D2
    GPIO_InitStruct.Pin = BUS_WAIT_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUS_WAIT_PORT, &GPIO_InitStruct); 
    HAL_GPIO_WritePin(BUS_WAIT_PORT, BUS_WAIT_PIN_MASK, BUS_WAIT_ACTIVE);

    // /RFSH input on C15
    GPIO_InitStruct.Pin = BUS_RFSH_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUS_RFSH_PORT, &GPIO_InitStruct); 

    // Address bus pins
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    for(int i = 0; i < address_line_count; i++) {
        GPIOLine* line = &address_lines[i];
        GPIO_InitStruct.Pin = 1U << line->pin;
        HAL_GPIO_Init(line->gpio, &GPIO_InitStruct); 
    }
}

