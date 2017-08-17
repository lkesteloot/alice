#include <stm32f4xx_hal.h>
#include "leds.h"
#include "defs.h"

#if ALICE3_VERSION == ALICE3_V1

#define LED_PIN_MASK GPIO_PIN_13
#define LED_PORT GPIOC

#else // ALICE3_VERSION == ALICE3_V3

#define LED_PIN_MASK GPIO_PIN_2
#define LED_PORT GPIOB

#endif

void LED_set_panic(int on)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, on);
}

void LED_beat_heart()
{
    static int heartbeat_level = 1;
    static unsigned int previous_heartbeat_tick = 0;

    unsigned int now = HAL_GetTick();
    if(heartbeat_level == 1) {
        if(now - previous_heartbeat_tick > 350) {
            heartbeat_level = 0;
            HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    } else {
        if(now - previous_heartbeat_tick > 650) {
            heartbeat_level = 1;
            HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, heartbeat_level);
            previous_heartbeat_tick = now;
        }
    }
}

void LED_set_info(int on)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, on);
}

void LED_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = LED_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct); 

    HAL_GPIO_WritePin(LED_PORT, LED_PIN_MASK, 1);
}


