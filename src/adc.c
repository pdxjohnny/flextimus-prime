#include <adc.h>
#include <gpio.h>
#include "main.h"

static bool adc_within_interrupt;
static bool adc_converting;

/* Staticly allocated statuses go here. */
adc_status_t ADC_OK = {
  .code = ADC_STATUS_OK,
  .data = 0,
};
adc_status_t ADC_TIMEOUT = {
  .code = ADC_STATUS_TIMEOUT,
  .data = 0,
};
adc_status_t ADC_INVALID_CONVERT_PIN = {
  .code = ADC_STATUS_INVALID_CONVERT_PIN,
  .data = 0,
};
adc_status_t ADC_CONVERSION_IN_PROGRESS = {
  .code = ADC_STATUS_CONVERSION_IN_PROGRESS,
  .data = 0,
};
adc_status_t ADC_NEED_CONVERSION_CALLBACK = {
  .code = ADC_STATUS_NEED_CONVERSION_CALLBACK,
  .data = 0,
};

/* Callback function for when the ADC End Of Conversion interrupt is triggered.
 * This is caused by a called to adc_convert_async
 */
adc_status_t (*adc_conversion_complete)(adc_convertion_result result);
adc_status_t (*adc_adrdy_handler)();

/* Create and returns a adc_status_t which has a code of ADC_STATUS_OK with the
 * data specified as an argument. */
adc_status_t adc_success(adc_status_data_t data) {
  adc_status_t status = {.code = ADC_STATUS_OK, .data = data};
  return status;
}


/* Poll bit with timeout. If we are in an interrupt we only poll once. */
adc_status_t adc_wait(__IO uint32_t *reg, uint32_t and_with,
    uint32_t what_it_should_be) {
  for (unsigned int i = 0; (*reg & and_with) != what_it_should_be; i++) {
    /* If we are in an interrupt and have checked once already then we need to
     * exit with a timeout to allow other interrupts to be serviced as quickly
     * as possible. */
    if ((adc_within_interrupt == true && i > 1) || i > ADC_TIMEOUT_TICKS) {
      return ADC_TIMEOUT;
    }
	}
	return ADC_OK;
}

/* Chooses which pin on the STM32F042 we want to convert from using the ADC.
 * Returns
 *  ADC_OK
 *    Valid pin has been selected for conversion
 *  ADC_INVALID_CONVERT_PIN
 *    Caller passed in an invalid pin
 */
adc_status_t adc_select_conversion_pin(adc_convert_t pin_to_convert) {
  switch (pin_to_convert) {
    case ADC_CONVERT_PA0:
      ADC1->CHSELR |= ADC_CHSELR_CHSEL0;
      return ADC_OK;
    default:
      break;
  }
  return ADC_INVALID_CONVERT_PIN;
}

/* Waits for the conversion to complete by polling the ISR to see if the End Of
 * Conversion (EOC) bit is set. Use ADC_VOLTS and ADC_MILLIVOLTS with result.
 */
adc_status_t adc_read() {
  adc_status_t status;
  /* Wait end of conversion */
	// status = adc_wait(&ADC1->ISR, ADC_ISR_EOC, ADC_ISR_EOC);
  // if (ADC_ERROR(status)) {
  //   return status;
  // }
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {};
  /* Get ADC1 converted data and compute the voltage */
  return adc_success((ADC_GetConversionValue(ADC1) * 3300) / 0xFFF);
}

/* Tell the other functions through the global variable `adc_converting` that we
 * have started a conversion. If `adc_converting` is already true then we return
 * an error saying the ADC is already converting.
 */
static adc_status_t adc_start_converting() {
  if (adc_converting == true) {
    return ADC_CONVERSION_IN_PROGRESS;
  }
  adc_converting = true;
  /* ADC1 regular Software Start Conv */
  ADC_StartOfConversion(ADC1);
  return ADC_OK;
}

/* 13.4.7 Single conversion mode (CONT=0)
 *
 * In Single conversion mode, the ADC performs a single sequence of conversions,
 * converting all the channels once. This mode is selected when CONT=0 in the
 * ADC_CFGR1 register.
 *
 * Conversion is started by either:
 * - Setting the ADSTART bit in the ADC_CR register
 * - Hardware trigger event
 *
 * Inside the sequence, after each conversion is complete:
 * - The converted data are stored in the 16-bit ADC_DR register
 * - The EOC (end of conversion) flag is set
 * - An interrupt is generated if the EOCIE bit is set
 *
 * After the sequence of conversions is complete:
 * - The EOSEQ (end of sequence) flag is set
 * - An interrupt is generated if the EOSEQIE bit is set
 *
 * Then the ADC stops until a new external trigger event occurs or the ADSTART
 * bit is set again.
 *
 * Note: To convert a single channel, program a sequence with a length of 1.
 *
 * Source: A.7.5 Single conversion sequence code example - Software trigger
 */
adc_status_t adc_convert(adc_convert_t pin_to_convert) {
  adc_status_t status;
  /* Start converting if we are not already converting */
  status = adc_start_converting();
  if (ADC_ERROR(status)) {
    return status;
  }
  /* Store the ADC conversion result */
  status = adc_read();
  /* ADC1 regular Software Stop Conv */
  ADC_StopOfConversion(ADC1);
  adc_converting = false;
  return status;
}

/* Interrupt handler for ADC. Called when the ADC becomes ready or a conversion
 * is complete.
 */
void adc_handler() {
  adc_within_interrupt = true;
  if (ADC_GetITStatus(ADC1, ADC_IT_ADRDY) && adc_adrdy_handler != NULL) {
    adc_adrdy_handler();
  } else if (ADC_GetITStatus(ADC1, ADC_IT_EOC) &&
      adc_conversion_complete != NULL) {
    adc_conversion_complete(adc_read());
  }
  adc_within_interrupt = false;
  adc_converting = false;
}

/* Ask the ADC to start a conversion and cause an interrupt when it is done.
 *
 * Source: A.7.13 Auto Off and wait mode sequence code example
 */
adc_status_t adc_convert_async(adc_convert_t pin_to_convert,
    adc_status_t (*set_adc_conversion_complete)(adc_convertion_result result)) {
  adc_status_t status;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Make sure that we have a conversion callback */
  if (set_adc_conversion_complete == NULL) {
    return ADC_NEED_CONVERSION_CALLBACK;
  }
  adc_conversion_complete = set_adc_conversion_complete;
  /* 13.7.2 Auto-off mode (AUTOFF):
   *  When AUTOFF=1, the ADC is always powered off when not converting and
   *  automatically wakes-up when a conversion is started (by software or
   *  hardware trigger). A startup-time is automatically inserted between the
   *  trigger event which starts the conversion and the sampling time of the
   *  ADC. The ADC is then automatically disabled once the sequence of
   *  conversions is complete.
   */
  ADC_AutoPowerOffCmd(ADC1, ENABLE);
  /* 13.7.1 Wait mode conversion
   *  When the WAIT bit is set to 1 in the ADC_CFGR1 register, a new conversion
   *  can start only if the previous data has been treated, once the ADC_DR
   *  register has been read or if the EOC bit has been cleared. This is a way
   *  to automatically adapt the speed of the ADC to the speed of the system
   *  that reads the data.
   */
  ADC_WaitModeCmd(ADC1, ENABLE);
  /* (4) Enable interrupts on EOC (End Of Conversion) */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);








  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

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

  /* Start converting */
  status = adc_start_converting();
  if (ADC_ERROR(status)) {
    return status;
  }
  return status;
}

adc_status_t adc_up(gpio_pin_t gpio_pin,
    adc_status_t (*adc_set_adrdy_handler)()) {
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  NVIC_InitTypeDef    NVIC_InitStructure;

  adc_conversion_complete = NULL;
  adc_adrdy_handler = adc_set_adrdy_handler;

  gpio_clock(gpio_pin);

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure ADC Channel11 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC1 DeInit */
  ADC_DeInit(ADC1);

  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);

  /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */
  ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_239_5Cycles);

  /* Analog watchdog config ******************************************/
  /* Configure the ADC Thresholds between 1.5V and 2.5V (1861, 3102) */
  ADC_AnalogWatchdogThresholdsConfig(ADC1, 3102, 1861);

  /* Enable the ADC1 single channel */
  ADC_AnalogWatchdogSingleChannelCmd(ADC1, ENABLE);

  ADC_OverrunModeCmd(ADC1, ENABLE);
  /* Enable the ADC1 analog watchdog */
  ADC_AnalogWatchdogCmd(ADC1, ENABLE);

  /* Select a single ADC1 channel 11 */
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_AnalogWatchdog_Channel_11);

  /* Enable AWD interrupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

  /* Configure and enable ADC1 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the ADC1 Calibration */
  ADC_GetCalibrationFactor(ADC1);

  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait the ADRDY flag */
  // while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));
  ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);

  /* ADC1 regular Software Start Conv */
  ADC_StartOfConversion(ADC1);

  // TODO perhaps use the adc_watch_enable function
  return ADC_OK;
}

adc_status_t adc_down() {
  adc_status_t status;
	/* A.7.3 ADC disable sequence code example */
	/* (1) Stop any ongoing conversion */
	ADC1->CR |= ADC_CR_ADSTP;
	/* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
	status = adc_wait(&ADC1->CR, ADC_CR_ADSTP, 0);
  if (ADC_ERROR(status)) {
    return status;
  }
	/* (3) Disable the ADC */
	ADC1->CR |= ADC_CR_ADDIS;
	/* (4) Wait until the ADC is fully disabled */
	status = adc_wait(&ADC1->CR, ADC_CR_ADEN, 0);
  if (ADC_ERROR(status)) {
    return status;
  }
  return ADC_OK;
}
