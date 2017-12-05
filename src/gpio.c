#include <util.h>
#include <gpio.h>
#include <flextimus.h>

void* gpio_perf(gpio_pin_t gpio_pin) {
  void *which_gpio = NULL;
  if (gpio_pin & GPIO_A) {
    which_gpio = GPIOA;
  } else if (gpio_pin & GPIO_B) {
    which_gpio = GPIOB;
  } else if (gpio_pin & GPIO_C) {
    which_gpio = GPIOC;
  }
  assert_param(IS_GPIO_ALL_PERIPH(which_gpio));
  return which_gpio;
}

void gpio_clock(gpio_pin_t gpio_pin, FunctionalState NewState) {
  /* RCC peripheral clock enable */
  if (gpio_pin & GPIO_A) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, NewState);
  } else if (gpio_pin & GPIO_B) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, NewState);
  } else if (gpio_pin & GPIO_C) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, NewState);
  }
}

/* Enables a GPIO pin as an ouput */
void gpio_up(gpio_pin_t gpio_pin) {
  GPIO_InitTypeDef GPIO_InitStructure;

  gpio_clock(gpio_pin, ENABLE);

  /* Configure pins in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = (gpio_pin & GPIO_PIN_MASK);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* Turns off GPIO pin which was either an output or input */
void gpio_down(gpio_pin_t gpio_pin) {
  gpio_clock(gpio_pin, DISABLE);
}

void gpio_on(gpio_pin_t gpio_pin) {
  ((GPIO_TypeDef *)gpio_perf(gpio_pin))->BSRR = gpio_pin & GPIO_PIN_MASK;
}

void gpio_off(gpio_pin_t gpio_pin) {
  ((GPIO_TypeDef *)gpio_perf(gpio_pin))->BRR = gpio_pin & GPIO_PIN_MASK;
}

/* Enables a GPIO pin as an input using the EXTI lines */
void gpio_input(gpio_pin_t gpio_pin) {
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  uint16_t gpio_pin_number = bit_index(gpio_pin & GPIO_PIN_MASK);

  gpio_clock(gpio_pin, ENABLE);

  /* Configure gpio_pin as input floating */
  GPIO_InitStructure.GPIO_Pin = gpio_pin & GPIO_PIN_MASK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init((GPIO_TypeDef *)gpio_perf(gpio_pin), &GPIO_InitStructure);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Connect EXTI0 Line to PA0 pin */
  if (gpio_pin & GPIO_A) {
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, gpio_pin_number);
  } else if (gpio_pin & GPIO_B) {
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, gpio_pin_number);
  } else if (gpio_pin & GPIO_C) {
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, gpio_pin_number);
  }

  /* Configure the button from this line */
  EXTI_InitStructure.EXTI_Line = gpio_pin & GPIO_PIN_MASK;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  if (gpio_pin_number >= 0 && gpio_pin_number <= 1) {
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  } else if (gpio_pin_number >= 2 && gpio_pin_number <= 3) {
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
  } else if (gpio_pin_number >= 4 && gpio_pin_number <= 15) {
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  }
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

bool gpio_asserted(gpio_pin_t gpio_pin) {
  return !!(GPIO_ReadInputDataBit(gpio_perf(gpio_pin), gpio_pin & GPIO_PIN_MASK));
}

bool gpio_asserted_irq(gpio_pin_t gpio_pin) {
  if (EXTI_GetITStatus(gpio_pin & GPIO_PIN_MASK) != RESET) {
    EXTI_ClearITPendingBit(gpio_pin & GPIO_PIN_MASK);
    return !!(gpio_asserted(gpio_pin));
  }
  return !!(false);
}

void debounce_init(debouncer_t *debounce) {
  debounce->tick = 0;
  debounce->value = 0;
}

bool gpio_asserted_debounce(gpio_pin_t gpio_pin, debouncer_t *debounce) {
  if (debounce->tick > DEBOUNCE_DELAY) {
    debounce->tick = 0;
  }
  if (debounce->tick == 0) {
    return !!(GPIO_ReadInputDataBit(gpio_perf(gpio_pin),
          gpio_pin & GPIO_PIN_MASK));
  }
  ++(debounce->tick);
  return !!(false);
}
