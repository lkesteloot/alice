#include <stm32f4xx_hal.h>
#include "reset_button.h"

//----------------------------------------------------------------------------
// Z80 reset button ("soft reset")

void RESET_BUTTON_init()
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    GPIO_InitStruct.Pin = RESET_BUTTON_PIN_MASK;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RESET_BUTTON_PORT, &GPIO_InitStruct); 
}


