/*
 * Blinky: Switch the MCU to highest speed and blink the LED attached
 * to port B, bit 3
*/
#include <processor.h>

void delay(int dly) {
  while (dly--);
}

void clock_init() {
  // This is potentially a dangerous function as it could
  // result in a system with an invalid clock signal - result: a stuck system
  // Set the PLL up
  // First ensure PLL is disabled
  RCC_CR &= ~BIT24;
  while( (RCC_CR & BIT25)); // wait for PLL ready to be cleared
  // set PLL multiplier to 12 (yielding 48MHz)
  // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
  // inserted into Flash memory interface
  FLASH_ACR |= BIT0;
  FLASH_ACR &=~(BIT2 | BIT1);

  // Turn on FLASH prefetch buffer
  FLASH_ACR |= BIT4;
  RCC_CFGR &= ~(BIT21 | BIT20 | BIT19 | BIT18);
  RCC_CFGR |= (BIT21 | BIT19 );

  // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
  RCC_CFGR |= BIT14;

  // Turn the PLL back on again
  RCC_CR |= BIT24;
  // Set PLL as system clock source
  RCC_CFGR |= BIT1;
}

void config_pins() {
	// Power up PORTB
	RCC_AHBENR |= BIT18;
}

int main() {
	unsigned int i = 0;

	clock_init();
	config_pins();

	GPIOB_MODER |= BIT6; // make bit3  an output
	GPIOB_MODER &= ~BIT7; // make bit3  an output

	while(1) {
		GPIOB_ODR |= BIT3;
		delay(2000000);
		GPIOB_ODR &= ~BIT3;
		delay(2000000);
	}

	return 0;
}
