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
bool PauseState;

// 0 = OFF, 1 = Recording MAX, 2 = Recording MIN
int ConfigState;

// Variable to hold buzzer status
bool BuzzerState;

void PinSetup()
{
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

// IRQ handler for flex sensor interrupts
int FlexSensorIRQ()
{
  if (interrupt from sensor)
    Buzzer();

  else
    return 0;
}

// IRQ handler for both button interrupts
int ButtonIRQ()
{
  if (pause button)
    Pause();
  else if (config button)
    Config();
  else
    return 0;
}

// IRQ handler for the timer
int TimerIRQ
{
  if (timer)
    Buzzer();
  else
    return 0;
}

// Function to pause the alert system
int Pause()
{
  if (PauseState == false)
  {
    PauseState = true;
    GPIOB_ODR |= BIT0; // Turn on LED
  }
  else
  {
    PauseState = false;
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
}

// Function to turn on buzzer
int Buzzer()
{
  if (BuzzerStatus == false && PauseStatus == false)
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

// Timer function
int Timer()
{
  // Do timer stuff
  return 0;
}

int main()
{

  return 0;
}
