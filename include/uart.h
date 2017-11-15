#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define CR1_ENABLED 0x0000200c

uint32_t uart_init();
void uart_send(uint8_t send_byte);
uint8_t uart_recv();

#endif /* _UART_H */
