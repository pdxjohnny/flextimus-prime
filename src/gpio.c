#include <gpio.h>
#include "main.h"

static GPIO_TypeDef* gpio_perf(gpio_pin_t gpio_pin) {
  if (gpio_pin & GPIO_A == GPIO_A) {
    return GPIOA;
  } else if (gpio_pin & GPIO_B == GPIO_B) {
    return GPIOB;
  } else if (gpio_pin & GPIO_C == GPIO_C) {
    return GPIOC;
  }
}

void gpio_clock(gpio_pin_t gpio_pin) {
  /* RCC peripheral clock enable */
  if (gpio_pin & GPIO_A == GPIO_A) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  } else if (gpio_pin & GPIO_B == GPIO_B) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  } else if (gpio_pin & GPIO_C == GPIO_C) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  }
}

void gpio_up(gpio_pin_t gpio_pin) {
  GPIO_InitTypeDef GPIO_InitStructure;

  gpio_clock(gpio_pin);

  /* Configure pins in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = (gpio_pin & GPIO_PIN_MASK);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void gpio_on(gpio_pin_t gpio_pin) {
  gpio_perf(gpio_pin)->BSRR = gpio_pin & GPIO_PIN_MASK;
}

void gpio_off(gpio_pin_t gpio_pin) {
  gpio_perf(gpio_pin)->BRR = gpio_pin & GPIO_PIN_MASK;
}

void gpio_input(void) {
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure PA0 in interrupt mode */
  // EXTI0_Config();
  /* Simulate a falling edge applied on EXTI8 line */
  // EXTI_GenerateSWInterrupt(EXTI_Line8);
}

int gpio_asserted(gpio_pin_t gpio_pin) {
  return GPIO_ReadInputDataBit(gpio_perf(gpio_pin), gpio_pin & GPIO_PIN_MASK);
}
