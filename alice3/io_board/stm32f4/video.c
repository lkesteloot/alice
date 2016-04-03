#include <stm32f4xx_hal.h>
#include "defs.h"
#include "video.h"

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

#define PROP_READY_TIMEOUT_MILLIS 5000

void VIDEO_wait()
{
    if(ALICE3_VERSION == ALICE3_V3) {

        // Only on Alice3 V3 does ARM start before Propeller.
        // (On V1, ARM has 5-second delay while Propeller has 1-second delay)

        int started = HAL_GetTick();
        while(HAL_GPIO_ReadPin(PROP_READY_PORT, PROP_READY_PIN_MASK)) {
            int now = HAL_GetTick();
            if(now - started > PROP_READY_TIMEOUT_MILLIS) {
                printf("PANIC: timed out after waiting 5 seconds for PROP_READY\n");
                panic();
            }
        }
    }
}

