#include "delay.h"
#include "gpio_helpers.h"
#include "bus_signals.h"
#include "bus_aggregate.h"

//----------------------------------------------------------------------------
// System bus aggregate operations

#define MEMORY_DELAY_MICROS     10
#define IO_DELAY_MICROS     2

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
    delay_us(1);
    set_GPIO_value(BUS_WR_PORT, BUS_WR_PIN_MASK, BUS_WR_ACTIVE);

    delay_us(MEMORY_DELAY_MICROS);

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

    delay_us(1);
    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_ACTIVE);
    delay_us(1);
    set_GPIO_value(BUS_MREQ_PORT, BUS_MREQ_PIN_MASK, BUS_MREQ_ACTIVE);

    delay_us(MEMORY_DELAY_MICROS);
    unsigned char d = BUS_get_DATA();
    __asm__ volatile("" ::: "memory"); // Force all memory operations before to come before and all after to come after.

    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_INACTIVE);
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

    delay_us(IO_DELAY_MICROS);

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

    delay_us(IO_DELAY_MICROS);
    unsigned char d = BUS_get_DATA();

    set_GPIO_value(BUS_RD_PORT, BUS_RD_PIN_MASK, BUS_RD_INACTIVE);
    set_GPIO_value(BUS_IORQ_PORT, BUS_IORQ_PIN_MASK, BUS_IORQ_INACTIVE);

    return d;
}

void BUS_acquire_bus(int releaseReset)
{
    set_GPIO_iotype(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN, GPIO_MODE_OUTPUT_PP);
    set_GPIO_value(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK, BUS_BUSRQ_ACTIVE);

    if(releaseReset)
        BUS_reset_finish();

    while(HAL_GPIO_ReadPin(BUS_BUSAK_PORT, BUS_BUSAK_PIN_MASK));
}

void BUS_release_bus(int startReset)
{
    set_GPIO_value(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN_MASK, BUS_BUSRQ_INACTIVE);

    if(startReset) {
        BUS_reset_start();
    } else {
        while(!HAL_GPIO_ReadPin(BUS_BUSAK_PORT, BUS_BUSAK_PIN_MASK));
    }

    set_GPIO_iotype(BUS_BUSRQ_PORT, BUS_BUSRQ_PIN, GPIO_MODE_INPUT);
}

