#ifndef __GPIO_HELPERS__
#define __GPIO_HELPERS__

#include <stm32f4xx_hal.h>

static inline void set_GPIO_value(GPIO_TypeDef* gpio, int mask, int value)
{
    unsigned long int data = value ? mask : 0;
    gpio->ODR = (gpio->ODR & ~mask) | data;
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
}

static inline void set_GPIO_iotype(GPIO_TypeDef* gpio, int pin, unsigned int iotype)
{
    long unsigned int mask = ~(3U << (pin * 2));
    long unsigned int value = iotype << (pin * 2);
    gpio->MODER = (gpio->MODER & mask) | value;
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.
}

#endif /* __GPIO_HELPERS__ */
