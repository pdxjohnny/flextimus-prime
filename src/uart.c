#include <uart.h>
#include <stdio.h>
#include <processor.h>

uint32_t uart_init() {
  // All inputs except pin 2 (which is the transmit pin).
  writel(GPIOA_BASE + 0x00, 0x44444b44);
  writel(GPIOA_BASE + 0x04, 0x44444444);
  // Enable USART, Transmit, and Receive
  writel(USART1_BASE + 0x0c, CR1_ENABLED);

  if (!(readl(USART1_BASE + 0x00) & 0x020)) {
    return -1;
  }
}

void uart_send(uint8_t send_byte) {
  writel(USART1_BASE + 0x04, send_byte);
}

uint8_t uart_recv() {
  uint32_t status;
  // TODO: Add a timeout to avoid infinite loop
  do {
      status = readl(USART1_BASE + 0x00);
  } while((status & 0x020) == 0);
  return (readl(USART1_BASE + 0x04) & 0xFF);
}
