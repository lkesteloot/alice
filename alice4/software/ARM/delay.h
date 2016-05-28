#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdio.h>

void delay_ms(int ms);
void delay_us(uint32_t us);
void delay_100ms(unsigned char count);
void delay_init(void) ;

#endif /* __DELAY_H__ */
