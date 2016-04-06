#ifndef __BUS_SIGNALS_H__
#define __BUS_SIGNALS_H__

#include <stm32f4xx_hal.h>

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

#define BUS_RESET_PORT GPIOB
#define BUS_RESET_PIN_MASK GPIO_PIN_0
#define BUS_RESET_ACTIVE 0
#define BUS_RESET_INACTIVE BUS_RESET_PIN_MASK
#define BUS_RESET_DURATION_MS 10

typedef struct GPIOLine {
    GPIO_TypeDef* gpio;
    int pin; 
} GPIOLine;

extern GPIOLine address_lines[];
extern int address_line_count;

void BUS_init();
void BUS_set_DATA_as_output();
void BUS_set_DATA_as_input();
unsigned char BUS_get_DATA();
void BUS_set_ADDRESS(unsigned int a);
void BUS_reset_finish();
void BUS_set_ADDRESS_as_output();
void BUS_set_ADDRESS_as_input();
void BUS_reset_start();
void BUS_set_DATA(unsigned char data);
unsigned int shuffle_address(unsigned int a);

#endif /* __BUS_SIGNALS_H__ */
