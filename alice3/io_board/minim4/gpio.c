-- input

GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x0;          // INPUT

-- output push-pull
GPIOA->MODER = (GPIOA->MODER & ~0xffff) | 0x5555;       // OUTPUT
// one-time output setup:
GPIOA->SPEEDR = (GPIOA->SPEEDR & ~0xffff) | 0x0000;     // LOW
GPIOA->OTYPER = (GPIOA->OTYPER & 0xff) | 0x0000;        // PUSH_PULL
GPIOA->PUPDR = 0x0000;                                  // no PUPD


configure IO direction mode
set speed if output
set pullup or pulldown

      /* In case of Output or Alternate function mode selection */
      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
        /* Configure the IO Speed */
        temp = GPIOx->OSPEEDR; 
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
        temp |= (GPIO_Init->Speed << (position * 2));
        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position) ;
        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << position);
        GPIOx->OTYPER = temp;
      }

      /* Activate the Pull-up or Pull down resistor for the current IO */
      temp = GPIOx->PUPDR;
      temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
      temp |= ((GPIO_Init->Pull) << (position * 2));
      GPIOx->PUPDR = temp;
