#ifndef __UART_H__
#define __UART_H__

void SERIAL_try_to_transmit_buffers();
void SERIAL_flush();
void SERIAL_enqueue_one_char(char c);
extern void uart_received(char c);
void SERIAL_init();
void SERIAL_poll_continue();

#endif /* __UART_H__ */
