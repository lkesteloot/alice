#include <stdio.h>
#include "defs.h"
#include "bus_signals.h"
#include "io_service.h"


//----------------------------------------------------------------------------
// Alice 3 Bus IO communication protocol

#define RESPONSE_STREAM_EMPTY 0x00

volatile unsigned char command_bytes[1 + 5 + 128 + 2]; // largest is status + write + sector + 16-bit checksum
volatile unsigned char command_length;

int IOSERVICE_get_command(volatile unsigned char **p)
{
    *p = command_bytes;
    return command_length;
}

unsigned char gNextByteForReading = RESPONSE_STREAM_EMPTY;
volatile unsigned char response_bytes[1 + 128 + 2]; // largest is status + sector + 16-bit checksum
volatile unsigned char response_staging_length;
volatile unsigned char response_length;
volatile unsigned char response_index;
volatile unsigned char response_waiting;

void IOSERVICE_start_response()
{
    response_staging_length = 0;
}

void IOSERVICE_append_response_byte(unsigned char r)
{
    if(response_staging_length >= sizeof(response_bytes)) {
        printf("PANIC: overflowed response buffer\n");
        panic();
    }
    response_bytes[response_staging_length++] = r;
}

// Caller must protect with disable_interrupts() / enable_interrupts()
void IOSERVICE_finish_response()
{
    gNextByteForReading = response_bytes[0];
    response_index = 1;
    response_length = response_staging_length;
    response_waiting = 1;
}

int IOSERVICE_get_response_staging_length()
{
    return response_staging_length;
}

int IOSERVICE_is_response_empty()
{
    return !response_waiting;
}

// Caller must protect with disable_interrupts()/enable_interrupts() if necessary
void IOSERVICE_clear_command()
{
    command_length = 0;
}

void BUS_set_DATA(unsigned char data);
void IOSERVICE_clear_response()
{
    response_length = 0;
    response_index = 0;
    response_waiting = 0;
    gNextByteForReading = RESPONSE_STREAM_EMPTY;
    BUS_set_DATA(gNextByteForReading);
}


//----------------------------------------------------------------------------
// Bus read and write response ISRs

#define BUS_IO_MASK (BUS_IORQ_PIN_MASK | BUS_RD_PIN_MASK | BUS_WR_PIN_MASK | BUS_A7_PIN_MASK)

#define IO_BOARD_ADDR   0
#define IO_BOARD_ADDR_PINS   (IO_BOARD_ADDR & BUS_A7_PIN_MASK)

const unsigned int gREAD_IO_Signals = BUS_WR_INACTIVE | BUS_IORQ_ACTIVE | BUS_RD_ACTIVE | IO_BOARD_ADDR_PINS;
const unsigned int gWRITE_IO_Signals = BUS_WR_ACTIVE | BUS_IORQ_ACTIVE | BUS_RD_INACTIVE | IO_BOARD_ADDR_PINS;
const unsigned int gIORQ_Signals = BUS_IORQ_ACTIVE | IO_BOARD_ADDR_PINS;

volatile int gUnclaimedWrite = 0;
volatile unsigned int gUnclaimedWriteAddressPins;
volatile unsigned int gUnclaimedWriteData;
volatile int gUnclaimedRead = 0;
volatile unsigned int gUnclaimedReadAddressPins;
volatile int gReadWasAlreadyInactive = 0;
volatile int gVideoSnoopedWrite = -1;

unsigned int BUS_get_shuffled_ADDRESS()
{
    unsigned int A = GPIOA->IDR;
    unsigned int B = GPIOB->IDR;
    unsigned int C = GPIOC->IDR;
    __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.
    unsigned int address = ((A & 0x6000) << 1) | ((B & 0x0700) >> 8) | ((B & 0x0018) << 7) | (C & 0x33F8);
    return address;
}

#if 0

MREQ interrupt:
    // At this point it's at least 64ns after MREQ asserted, RD or RFSH would be active as far as I can tell from the timing charts 
    set WAIT
    if(RFSH active)
        unset WAIT
        wait on /MREQ inactive
        set /WAIT
        return
    get A
    if RD active:
        set D output from A
        unset /WAIT
        wait on /MREQ inactive
        set D as input
        set /WAIT
        return
    // else WE cycle
    unset /WAIT
    wait on /WR active
    get D
    store D at A
    wait on /MREQ inactive
    set /WAIT
    return

IORQ interrupt:
    // At this point it's at least 64ns after MREQ asserted, RD or WR would be active as far as I can tell from the timing charts 
    set WAIT
    get A
    if RD active:
        set D output from response queue
        unset /WAIT
        wait on /MREQ inactive
        set D as input
        set /WAIT
        return
    // else WE cycle
    unset /WAIT
    wait on /WR active
    get D
    store D in command queue
    wait on /MREQ inactive
    set /WAIT
    return

#endif

// RD line interrupt handler
void EXTI1_IRQHandler(void)
{
    if(DEBUG_BUS_ISR)
        set_GPIO_value(GPIOC, GPIO_PIN_15, 1);

    if((BUS_SIGNAL_CHECK_PORT->IDR & BUS_IO_MASK) == gREAD_IO_Signals) {

        // Put this here even before clearing interrupt so it happens
        // as soon as possible.
        BUS_set_DATA(gNextByteForReading);
        BUS_set_DATA_as_output();

        if((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_INACTIVE)
            gReadWasAlreadyInactive = 1;

        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.

        while((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_ACTIVE); /* busy wait for RD to rise */
        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.
        BUS_set_DATA_as_input();

        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);

        if(response_index >= response_length) {
            response_index = 0;
            response_length = 0;
            response_waiting = 0;
            gNextByteForReading = RESPONSE_STREAM_EMPTY;
        } else if(response_length > 0) {
            gNextByteForReading = response_bytes[response_index++];
        } 

    } else if((ALICE3_VERSION == ALICE3_V3) && ALICE3_V3_ARM_IS_RAM &&
        ((BUS_MREQ_PORT->IDR & BUS_MREQ_PIN_MASK) == BUS_MREQ_ACTIVE)) {

        // Memory read if Alice is V3 and ARM is RAM
        unsigned int address = BUS_get_shuffled_ADDRESS();
        unsigned char byte = ccmram_get(address);

        BUS_set_DATA(byte);
        BUS_set_DATA_as_output();

        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.

        while((BUS_SIGNAL_CHECK_PORT->IDR & BUS_RD_PIN_MASK) == BUS_RD_ACTIVE); /* busy wait for RD to rise */
        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.
        BUS_set_DATA_as_input();

        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);

    } else {

        if((ALICE3_VERSION == ALICE3_V3) && ALICE3_V3_ARM_IS_RAM) {
	    // Only record it if there isn't a notification of a
	    // previous one pending
            if(DEBUG_BUS_ISR) {
                if(!gUnclaimedRead) {
                    gUnclaimedRead = 1;
                    unsigned int A = GPIOA->IDR;
                    unsigned int B = GPIOB->IDR;
                    unsigned int C = GPIOC->IDR;
                    gUnclaimedReadAddressPins = (A << 16 & 0xff0000) | (B << 8 & 0xff00) | (C ^ 0xff);
                }
            }
        }

        __HAL_GPIO_EXTI_CLEAR_IT(BUS_RD_PIN_MASK);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);
    }
    if(DEBUG_BUS_ISR)
        set_GPIO_value(GPIOC, GPIO_PIN_15, 0);
}

// WR line interrupt handler
void EXTI2_IRQHandler(void)
{
    if(DEBUG_BUS_ISR) {
        set_GPIO_value(GPIOC, GPIO_PIN_15, 1);
        __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.
    }

    // Gather all port data
    unsigned int MREQ_port_value = BUS_MREQ_PORT->IDR;
    unsigned int signal_port_value = BUS_SIGNAL_CHECK_PORT->IDR;
    unsigned char data = BUS_get_DATA();
    unsigned int A = GPIOA->IDR;
    unsigned int B = GPIOB->IDR;
    unsigned int C = GPIOC->IDR;

    __HAL_GPIO_EXTI_CLEAR_IT(BUS_WR_PIN_MASK);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);

    __asm__ volatile("" ::: "memory"); // Force all memory instructions before to come before and all after to come after.

    if((signal_port_value & BUS_IORQ_PIN_MASK) == BUS_IORQ_ACTIVE) {

        if((signal_port_value & BUS_A7_PIN_MASK) == IO_BOARD_ADDR) {
            command_bytes[command_length++] = data;
        } else {
            gVideoSnoopedWrite = data;
            // snoop video writes
        }

    } else if((ALICE3_VERSION == ALICE3_V3) && ALICE3_V3_ARM_IS_RAM) {

        // Memory write
        if((MREQ_port_value & BUS_MREQ_PIN_MASK) == BUS_MREQ_ACTIVE) {

            unsigned int address = BUS_compute_shuffled_ADDRESS(A, B, C);
            ccmram_set(address, data);

        } else {

	    // Only record it if there isn't a notification of a
	    // previous one pending
            if(DEBUG_BUS_ISR) {
                if(!gUnclaimedWrite) {
                    gUnclaimedWriteAddressPins = (A << 16 & 0xff0000) | (B << 8 & 0xff00) | (C ^ 0xff);
                    gUnclaimedWriteData = data;
                }
                gUnclaimedWrite++;
            }
        }
    }

    if(DEBUG_BUS_ISR)
        set_GPIO_value(GPIOC, GPIO_PIN_15, 0);
}

