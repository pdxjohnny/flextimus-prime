#include "main.h"

void led_up() {
  // Power up PORTB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  GPIOB->MODER |= GPIO_MODER_MODER3_0; // make bit3  an output
  GPIOB->MODER &= ~GPIO_MODER_MODER3_1; // make bit3  an output
}

void led_on() {
  GPIOB->ODR |= GPIO_MODER_MODER1_1;
}

void led_off() {
  GPIOB->ODR &= ~GPIO_MODER_MODER1_1;
}
