#include <adc.h>
#include <gpio.h>
#include <flextimus.h>

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
static adc_status_t (*adc_conversion_complete)(adc_convertion_result result);
static adc_status_t (*adc_adrdy_handler)();
static adc_status_t (*adc_awd_handler)();

/* Create and returns a adc_status_t which has a code of ADC_STATUS_OK with the
 * data specified as an argument. */
adc_status_t adc_success(adc_status_data_t data) {
  adc_status_t status = {.code = ADC_STATUS_OK, .data = data};
  return status;
}

/* Waits for the conversion to complete by polling the ISR to see if the End Of
 * Conversion (EOC) bit is set. Use ADC_VOLTS and ADC_MILLIVOLTS with result.
 */
adc_status_t adc_read() {
  adc_status_t status;
  /* Wait end of conversion */
  ADC_WITH_TIMEOUT(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
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

/* Convert using the ADC and poll waiting for it to be done. */
adc_status_t adc_convert(gpio_pin_t pin_to_convert) {
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
  if (ADC_GetITStatus(ADC1, ADC_IT_ADRDY) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_ADRDY);
    if (adc_adrdy_handler != NULL) {
      adc_adrdy_handler();
    }
  } else if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET &&
      adc_conversion_complete != NULL) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    adc_conversion_complete(adc_read());
    /* ADC1 regular Software Stop Conv */
    ADC_StopOfConversion(ADC1);
    /* Disable interrupts on EOC (End Of Conversion) */
    ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
  } else if (ADC_GetITStatus(ADC1, ADC_IT_EOSEQ) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOSEQ);
  } else if (ADC_GetITStatus(ADC1, ADC_IT_AWD) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
    if (adc_awd_handler != NULL) {
      adc_awd_handler();
    }
  } else if (ADC_GetITStatus(ADC1, ADC_IT_EOSMP) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOSMP);
  } else if (ADC_GetITStatus(ADC1, ADC_IT_OVR) == SET) {
    ADC_ClearITPendingBit(ADC1, ADC_IT_OVR);
  }
  adc_within_interrupt = false;
  adc_converting = false;
}

/* Ask the ADC to start a conversion and cause an interrupt when it is done.
 *
 * Source: A.7.13 Auto Off and wait mode sequence code example
 */
adc_status_t adc_convert_async(gpio_pin_t pin_to_convert,
    adc_status_t (*set_adc_conversion_complete)(adc_convertion_result result)) {
  adc_status_t status;
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

  /* Start converting */
  status = adc_start_converting();
  if (ADC_ERROR(status)) {
    return status;
  }
  return status;
}

adc_status_t adc_awd_config(gpio_pin_t gpio_pin, int start, int stop,
    adc_status_t (*set_adc_awd_handler)()) {
  /* Set the callback handler for when the watchdog interrupt is generated */
  adc_awd_handler = set_adc_awd_handler;
  assert_param(adc_awd_handler);

  /* Set the bounds of the watchdog */
  // ADC_AnalogWatchdogThresholdsConfig(ADC1, 3102, 1861);
  ADC_AnalogWatchdogThresholdsConfig(ADC1, start, stop);

  /* Enable the ADC1 single channel and overrun mode */
  ADC_AnalogWatchdogSingleChannelCmd(ADC1, ENABLE);
  ADC_OverrunModeCmd(ADC1, ENABLE);

  /* Enable the ADC1 analog watchdog */
  ADC_AnalogWatchdogCmd(ADC1, ENABLE);

  /* Select the channel for the gpioo_pin we are using */
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_GPIO_TO_CHANNEL(gpio_pin));

  /* Enable AWD interrupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
}

adc_status_t adc_up(gpio_pin_t gpio_pin,
    adc_status_t (*set_adc_adrdy_handler)()) {
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  NVIC_InitTypeDef    NVIC_InitStructure;

  adc_conversion_complete = NULL;
  adc_adrdy_handler = set_adc_adrdy_handler;
  assert_param(adc_adrdy_handler);

  /* Enable the GPIO pin */
  gpio_clock(gpio_pin, ENABLE);

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure ADC Channel for the requested GPIO as analog input */
  GPIO_InitStructure.GPIO_Pin = gpio_pin & GPIO_PIN_MASK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(gpio_perf(gpio_pin), &GPIO_InitStructure);

  /* ADC1 DeInit */
  ADC_DeInit(ADC1);

  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);

  /* Configure ADC1 in continuous mode with a resolution of 12 bits */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* Convert the ADC1 Channel for the reuested GPIO with 239.5 cycle sample time
   * TODO change to correct channel based off of gpio_pins. */
  ADC_ChannelConfig(ADC1, ADC_GPIO_CHSEL(gpio_pin),
      ADC_SampleTime_239_5Cycles);

  /* Enable ADC ready interrupt */
  ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);

  /* Configure and enable ADC1 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the ADC1 Calibration */
  ADC_GetCalibrationFactor(ADC1);

  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);

  return ADC_OK;
}

adc_status_t adc_down(gpio_pin_t gpio_pin) {
  NVIC_InitTypeDef    NVIC_InitStructure;

  /* ADC1 Interrupts disable */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Disable the GPIO pin */
  gpio_clock(gpio_pin, DISABLE);

  /* ADC1 Periph clock disable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);

  return ADC_OK;
}
