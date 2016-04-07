#include "delay.h"
#include <stm32f4xx_hal.h>

void delay_ms(int ms)
{
    HAL_Delay(ms);
}

void delay_100ms(unsigned char count)
{
    HAL_Delay(count * 100);
}

//----------------------------------------------------------------------------
// From: https://kbiva.wordpress.com/2013/03/25/microsecond-delay-function-for-stm32/

void delay_init(void) 
{
  if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) 
  {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
}
 
uint32_t DWT_Get(void)
{
  return DWT->CYCCNT;
}
 
static __inline
uint8_t DWT_Compare(int32_t tp)
{
  return (((int32_t)DWT_Get() - tp) < 0);
}
 
void delay_us(uint32_t us) // microseconds
{
  int32_t tp = DWT_Get() + us * (SystemCoreClock/1000000);
  while (DWT_Compare(tp));
}


