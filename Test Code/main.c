
/* 
 * Blinky: Switch the MCU to highest speed and blink the LED attached 
 * to port B, bit 3
*/


#include "stm32f042.h"

void delay(int);

void delay(int dly)
{
  while( dly--);
}

void initClock()
{
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

// Do the following to push HSI clock out on PA8 (MCO)
// for measurement purposes.  Should be 8MHz or thereabouts (verified with oscilloscope)
/*
        RCC_CFGR |= ( BIT26 | BIT24 );
        RCC_AHBENR |= BIT17;
        GPIOA_MODER |= BIT17;
*/

        // and turn the PLL back on again
        RCC_CR |= BIT24;        
        // set PLL as system clock source 
        RCC_CFGR |= BIT1;
}

void configPins()
{
	// Power up PORTA
	RCC_AHBENR |= BIT17;
	// Power up PORTB
	RCC_AHBENR |= BIT18;

  // Set Inputs
  GPIOA_MODER |= BIT0; // Flex Sensor
  GPIOA_MODER &= ~BIT1; // "" A0
  GPIOB_MODER |= BIT6; // BTN1 - Pause
  GPIOB_MODER &= ~BIT7; // "" B3
  GPIOB_MODER |= BIT8; // BTN2 - Config
  GPIOB_MODER &= ~BIT9; // "" B4

  // Set Outputs
  GPIOA_MODER |= BIT4; // Buzzer
  GPIOA_MODER &= ~BIT5; // "" A2
  GPIOB_MODER |= BIT0; // LED1 - Pause
  GPIOB_MODER &= ~BIT1; // "" B0
  GPIOB_MODER |= BIT2; // LED2 - Config
  GPIOB_MODER &= ~BIT3; // "" B1


}	
/*
int main()
{
  if (FLASH_ACR == 1)
  {
	  initClock();
  }
	unsigned i=0;
	configPins();
	GPIOB_MODER |= BIT6; // make bit3 an input
	GPIOB_MODER &= ~BIT7; // make bit3 an input	
	GPIOB_MODER |= BIT0; // make bit0  an output
	GPIOB_MODER &= ~BIT1; // make bit0  an output
	while(1)
	{
		if (GPIOB_IDR &  BIT3)
    {
      GPIOB_ODR |= BIT0;
		  delay(2000000);
		  GPIOB_ODR &= ~BIT0;
		  delay(2000000);
    }
	} 
	return 0;
}
*/

int PauseState;

// IRQ handler for both button interrupts
int ButtonIRQ()
{
  if (GPIOB_IDR & BIT3)
  {
    while (GPIOB_IDR & BIT3)
    {
      // Do nothing until released
    }
    Pause();
  }
//  else if (GPIOB_IDR & BIT4)
//    Config();
  else
    return 0;
}

// Function to pause the alert system
int Pause()
{
  if (PauseState == 0)
  {
    PauseState = 1;
    GPIOB_ODR |= BIT0; // Turn on LED
	  //delay(3000000); 
  }
  else
  {
    PauseState = 0;
    GPIOB_ODR &= ~BIT0; // Turn off LED
	  //delay(3000000); 
  }
  return 0;
}



int main()
{
   if (FLASH_ACR == 1)
  {
	  initClock();
  }
  configPins();
  PauseState = 0;
  while(1)
  {
    ButtonIRQ();
  }

  return 0;
}
