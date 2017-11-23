// Test code for 411 dev board
//
// INPUT
// Flex Sensor1 on GPIO A0
// Button 1 on GPIO B3 - (Pause Button)
// Button 2 on GPIO B4 - (Config Button)
//
// OUTPUT
// Buzzer on GPIO A2
// LED 1 on GPIO B0 - (Pause LED)
// LED 2 on GPIO B1 - (Config LED)


#include "stm32f042.h"

// Variable to hold pause status
int PauseState;

// 0 = OFF, 1 = Recording MAX, 2 = Recording MIN
int ConfigState;

// Variable to hold buzzer status
int BuzzerState;

void PinSetup()
{
  // Turn on PortA & PortB
  RCC_AHBENR |= BIT17;
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


/*
// IRQ handler for flex sensor interrupts
int FlexSensorIRQ()
{
  if (GPIOA_IDR & BIT0)
    Buzzer();

  else
    return 0;
}
*/
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
  else if (GPIOB_IDR & BIT4)
  {
    while (GPIOB_IDR & BIT4)
    {
      // Do nothing until released
    }
    Config();
  }
  else
    return 0;
}
/*
// IRQ handler for the timer (If we need it)
int TimerIRQ
{
  if (timer)
    Buzzer();
  else
    return 0;
}
*/
// Function to pause the alert system
int Pause()
{
  if (PauseState == 0)
  {
    PauseState = 1;
    GPIOB_ODR |= BIT0; // Turn on LED
  }
  else
  {
    PauseState = 0;
    GPIOB_ODR &= ~BIT0; // Turn off LED
  }
  return 0;
}

// Function to configure new min/max sensor values
int Config()
{
  if (ConfigState == 0) // Begin config
  {
    ConfigState = 1;
    GPIOB_ODR |= BIT1; // Turn on LED
    // Start watchdog?
  }
  else if (ConfigState == 1) // New MAX
  {
    ConfigState = 2;
    // Set new MAX value
    // Return to waiting
  }
  else // New MIN
  {
    ConfigState = 0;
    // Set new MIN
    GPIOB_ODR &= ~BIT1; // Turn off LED
    // Return to waiting
  }
  return 0;
}
/*
// Function to turn on buzzer
int Buzzer()
{
  if (BuzzerState == 0 && PauseState == 0)
  {
    GPIOA_ODR |= BIT2; // Turn on buzzer
    Timer();
  }
  else
  {
    GPIOA_ODR &= ~BIT2; // Turn off buzzer
    return 0;
  }
}

// Timer function (Maybe just use a delay?)
int Timer()
{
  // Do timer stuff
  return 0;
}
*/

void delay(int);

void delay(int dly)
{
  while( dly--);
}

int main()
{
  if (FLASH_ACR == 1)
  {
    initClock();
  }
  PinSetup();
  PauseState = 0;
  ConfigState = 0;
  BuzzerState = 0;
  while(1)
  {
    ButtonIRQ();
  }

  return 0;
}
