#include <adc.h>
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
    if (adc_within_interrupt == true && i > 1) {
      return ADC_TIMEOUT;
    }
    /* TODO For robust implementation, add time-out management here. Or perhaps
     * relinquish execution with a call to a thread scheduler. */
    if (i > 100000) {
      return ADC_TIMEOUT;
    }
	}
  return ADC_OK;
}

/* Turns on the HSI14 RC oscillartor if it is not already ready. */
static adc_status_t adc_start_hsi14() {
  adc_status_t status;
  /* This code turns on and selects the HSI14 as clock source. */
  if ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {
    /* (1) Enable the peripheral clock of the ADC */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    /* (2) Start HSI14 RC oscillator */
    RCC->CR2 |= RCC_CR2_HSI14ON;
    /* (3) Wait HSI14 is ready */
    status = adc_wait(&RCC->CR2, RCC_CR2_HSI14RDY, RCC_CR2_HSI14RDY);
    if (ADC_ERROR(status)) {
      return status;
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

/* 13.4.1 Calibration (ADCAL)
 *
 * The ADC has a calibration feature. During the procedure, the ADC calculates a
 * calibration factor which is internally applied to the ADC until the next ADC
 * power-off. The application must not use the ADC during calibration and must
 * wait until it is complete.
 *
 * Calibration should be performed before starting A/D conversion. It removes
 * the offset error which may vary from chip to chip due to process variation.
 *
 * The calibration is initiated by software by setting bit ADCAL=1. Calibration
 * can only be initiated when the ADC is disabled (when ADEN=0). ADCAL bit stays
 * at 1 during all the calibration sequence. It is then cleared by hardware as
 * soon the calibration completes. After this, the calibration factor can be
 * read from the ADC_DR register (from bits 6 to 0).
 *
 * The internal analog calibration is kept if the ADC is disabled (ADEN=0). When
 * the ADC operating conditions change (VDDA changes are the main contributor to
 * ADC offset variations and temperature change to a lesser extend), it is
 * recommended to re-run a calibration cycle.
 *
 * The calibration factor is lost each time power is removed from the ADC (for
 * example when the product enters STANDBY mode).
 *
 * Calibration software procedure
 * 1. Ensure that ADEN=0 and DMAEN=0
 * 2. Set ADCAL=1
 * 3. Wait until ADCAL=0
 * 4. The calibration factor can be read from bits 6:0 of ADC_DR.
 *
 * Source: A.7.1 ADC Calibration code example
 */
adc_status_t adc_calibration() {
  adc_status_t status;
	/* (1) Ensure that ADEN = 0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) {
		/* (2) Clear ADEN by setting ADDIS */
		ADC1->CR |= ADC_CR_ADDIS;
	}
	status = adc_wait(&ADC1->CR, ADC_CR_ADEN, 0);
  if (ADC_ERROR(status)) {
    return status;
  }
	/* (3) Clear DMAEN */
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
	/* (4) Launch the calibration by setting ADCAL */
	ADC1->CR |= ADC_CR_ADCAL;
	/* (5) Wait until ADCAL=0 */
	status = adc_wait(&ADC1->CR, ADC_CR_ADCAL, 0);
  if (ADC_ERROR(status)) {
    return status;
  }
}

/* 13.4.2 ADC on-off control (ADEN, ADDIS, ADRDY)
 *
 * At MCU power-up, the ADC is disabled and put in power-down mode (ADEN=0).
 * As shown in Figure 28, the ADC needs a stabilization time of tSTAB before it
 * starts converting accurately.
 *
 * Two control bits are used to enable or disable the ADC:
 * - Set ADEN=1 to enable the ADC. The ADRDY flag is set as soon as the ADC is
 *   ready for operation.
 * - Set ADDIS=1 to disable the ADC and put the ADC in power down mode. The ADEN
 *   and ADDIS bits are then automatically cleared by hardware as soon as the
 *   ADC is fully disabled.
 *
 * Conversion can then start either by setting ADSTART=1 (refer to Section 13.5:
 * Conversion on external trigger and trigger polarity (EXTSEL, EXTEN) on page
 * 238) or when an external trigger event occurs if triggers are enabled.
 *
 * Follow this procedure to enable the ADC:
 * 1. Clear the ADRDY bit in ADC_ISR register by programming this bit to 1.
 * 2. Set ADEN=1 in the ADC_CR register.
 * 3. Wait until ADRDY=1 in the ADC_ISR register and continue to write ADEN=1
 *    (ADRDY is set after the ADC startup time). This can be handled by
 *    interrupt if the interrupt is enabled by setting the ADRDYIE bit in the
 *    ADC_IER register.
 *
 * Source: A.7.2 ADC enable sequence code example
 */
adc_status_t adc_enable() {
  adc_status_t status;
	/* (1) Ensure that ADRDY = 0 */
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)  {
		/* (2) Clear ADRDY */
		ADC1->ISR |= ADC_CR_ADRDY;
	}
	/* (3) Enable the ADC */
	ADC1->CR |= ADC_CR_ADEN;
	/* (4) Wait until ADC ready */
	status = adc_wait(&ADC1->ISR, ADC_ISR_ADRDY, ADC_ISR_ADRDY);
  if (ADC_ERROR(status)) {
    return status;
  }
}

/* Interrupt when then voltage on the ADC pin exits a sepcified range.
 *
 * Source: A.7.14 Analog watchdog code example
 */
adc_status_t adc_watch_enable(adc_convert_t pin_to_convert,
    uint16_t vrefint_low, uint16_t vrefint_high) {
  /* (1) Select the continuous mode and configure the Analog watchdog to monitor
   * only CH17 */
  ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_AWDEN | ADC_CFGR1_AWDSGL;
  /* (2) Define analog watchdog range : 16b-MSW is the high limit and 16b-LSW is
   * the low limit */
  ADC1->TR = (uint32_t)((uint32_t)vrefint_high << (uint32_t)16) &
    (uint32_t)vrefint_low;
  /* (3) Enable interrupt on Analog Watchdog */
  ADC1->IER = ADC_IER_AWDIE;
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

/* Interrupt handler for ADC. Called when a conversion is complete.
 */
void adc_handler() {
  adc_within_interrupt = true;
  /* Only call the callback function if it points somewhere valid */
  if (adc_conversion_complete != NULL) {
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
  /* Make sure that we have a conversion callback */
  if (set_adc_conversion_complete == NULL) {
    return ADC_NEED_CONVERSION_CALLBACK;
  }
  adc_conversion_complete = set_adc_conversion_complete;
  /* Start converting if we are not already converting */
  status = adc_start_converting();
  if (ADC_ERROR(status)) {
    return status;
  }
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
  /* (3) Select channel for desired pin */
  status = adc_select_conversion_pin(pin_to_convert);
  if (ADC_ERROR(status)) {
    return status;
  }
  /* (4) Enable interrupts on EOC (End Of Conversion) */
  // ADC1->IER = ADC_IER_EOCIE;
  /* (1) Enable Interrupt on ADC in NVIC */
  NVIC_EnableIRQ(ADC1_COMP_IRQn);
  /* (2) Set priority for ADC in NVIC */
  NVIC_SetPriority(ADC1_COMP_IRQn, 0);
  return status;
}

adc_status_t adc_up() {
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;

  adc_conversion_complete = NULL;

  /* GPIOA Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure ADC Channel 0 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* Ensure HSI14 is powered and ready */
  adc_start_hsi14();
  /* ADCs DeInit */
  ADC_DeInit(ADC1);
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* Convert the ADC1 Channel 0 (PA0) with 239.5 Cycles as sampling time */
  ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);
  /* ADC Discontinuous mode, convert once then stop */
  ADC_DiscModeCmd(ADC1, ENABLE);
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

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
