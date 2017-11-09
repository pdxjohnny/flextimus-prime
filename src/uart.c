#include <uart.h>
#include <stdio.h>
#include <processor.h>

/*
Good sure here in  A.19.3 here: http://www.st.com/content/ccc/resource/technical/document/reference_manual/c2/f8/8a/f2/18/e6/43/96/DM00031936.pdf/files/DM00031936.pdf/jcr:content/translations/en.DM00031936.pdf
#define USART1_CR1	REGISTER_32(USART1_BASE + 0)
#define USART1_CR2	REGISTER_32(USART1_BASE + 4)
#define USART1_CR3	REGISTER_32(USART1_BASE + 8)
#define USART1_BRR	REGISTER_32(USART1_BASE + 0x0c)
#define USART1_GTPR	REGISTER_32(USART1_BASE + 0x10)
#define USART1_RTOR	REGISTER_32(USART1_BASE + 0x14)
#define USART1_RQR	REGISTER_32(USART1_BASE + 0x18)
#define USART1_ISR	REGISTER_32(USART1_BASE + 0x1c)
#define USART1_ICR	REGISTER_32(USART1_BASE + 0x20)
#define USART1_RDR	REGISTER_32(USART1_BASE + 0x24)
#define USART1_TDR	REGISTER_32(USART1_BASE + 0x28)
*/

uint32_t uart_init() {
  // All inputs except pin 2 (which is the transmit pin).
  writel(GPIOA_BASE + 0x00, 0x44444b44);
  writel(GPIOA_BASE + 0x04, 0x44444444);
  // Enable USART, Transmit, and Receive
  USART1_BRR |= CR1_ENABLED;

  /* (1) Oversampling by 16, 9600 baud */
  /* (2) 8 data bit, 1 start bit, 1 stop bit, no parity */
  USART1_BRR = 480000 / 96; /* (1) */
  // USART1_CR1 = USART_CR1_TE | USART_CR1_UE; /* (2) */

  if (!(USART1_CR1 & 0x020)) {
    return -1;
  }
}

void uart_send(uint8_t send_byte) {
  USART1_TDR = send_byte;
}

uint8_t uart_recv() {
  uint32_t status;
  // TODO: Add a timeout to avoid infinite loop
  while ((USART1_CR1 & 0x020) == 0);
  return (USART1_CR1 & 0xFF);
}
