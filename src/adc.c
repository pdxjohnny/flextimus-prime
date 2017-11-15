#include <adc.h>
#include "main.h"

int (*adc_conversion_complete)(int result);

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
int adc_calibration() {
	/* (1) Ensure that ADEN = 0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) {
		/* (2) Clear ADEN by setting ADDIS */
		ADC1->CR |= ADC_CR_ADDIS;
	}
	while ((ADC1->CR & ADC_CR_ADEN) != 0) {
		/* For robust implementation, add here time-out management */
	}
	/* (3) Clear DMAEN */
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
	/* (4) Launch the calibration by setting ADCAL */
	ADC1->CR |= ADC_CR_ADCAL;
	/* (5) Wait until ADCAL=0 */
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) {
		/* For robust implementation, add here time-out management */
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
int adc_enable() {
	/* (1) Ensure that ADRDY = 0 */
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)  {
		/* (2) Clear ADRDY */
		ADC1->ISR |= ADC_CR_ADRDY;
	}
	/* (3) Enable the ADC */
	ADC1->CR |= ADC_CR_ADEN;
	/* (4) Wait until ADC ready */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {
		/* For robust implementation, add here time-out management */
	}
}


/* Interrupt when then voltage on the ADC pin exits a sepcified range.
 *
 * Source: A.7.14 Analog watchdog code example
 */
int adc_watch_enable(uint16_t vrefint_low, uint16_t vrefint_high) {
  /* (1) Select the continuous mode and configure the Analog watchdog to monitor
   * only CH17 */
  ADC1->CFGR1 |= ADC_CFGR1_CONT | (17 << 26) | ADC_CFGR1_AWDEN |
    ADC_CFGR1_AWDSGL;
  /* (2) Define analog watchdog range : 16b-MSW is the high limit and 16b-LSW is
   * the low limit */
  ADC1->TR = (vrefint_high << 16) + vrefint_low;
  /* (3) Enable interrupt on Analog Watchdog */
  ADC1->IER = ADC_IER_AWDIE;
}

/* Waits for the conversion to complete by polling the ISR to see if the End Of
 * Conversion (EOC) bit is set.
 */
int adc_read() {
  /* Wait end of conversion */
  while ((ADC1->ISR & ADC_ISR_EOC) == 0) {
    /* For robust implementation, add here time-out management */
  }
  /* Store the ADC conversion result */
  return ADC1->DR;
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
int adc_convert() {
  /* (1) Select HSI14 by writing 00 in CKMODE (reset value) */
  // ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
  /* (2) Select CHSEL0, CHSEL9, CHSEL10 andCHSEL17 for VRefInt */
  ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL9
   | ADC_CHSELR_CHSEL10 | ADC_CHSELR_CHSEL17;
  /* (3) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater
   * than 17.1us */
  ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
  /* TODO Find out if this is really needed */
  /* (4) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) */
  ADC->CCR |= ADC_CCR_VREFEN;
  /* Performs the AD conversion. Start the ADC conversion */
  ADC1->CR |= ADC_CR_ADSTART;
  /* Store the ADC conversion result */
  return adc_read();
}

/* Interrupt handler for ADC. Called when a conversion is complete.
 */
void adc_handler() {
  if (adc_conversion_complete != NULL) {
  }
}

/* Ask the ADC to start a conversion and cause an interrupt when it is done.
 *
 * Source: A.7.13 Auto Off and wait mode sequence code example
 */
void adc_convert_async(adc_convert_t pin_to_convert) {
  /* (1) Select HSI14 by writing 00 in CKMODE (reset value) */
  ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
  /* (2) Select the external trigger on TIM15_TRGO and falling edge,
   * the continuous mode, scanning direction and auto off */
  ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0 | ADC_CFGR1_AUTOFF | ADC_CFGR1_OVRMOD;
  switch (pin_to_convert) {
    case ADC_CONVERT_PIN_1:
      ADC1->CFGR1 |= ADC_CFGR1_EXTSEL_2;
    default:
      // ADC needs a valid pin to convert
      assert_param(0);
  }
  /* (3) Select CHSEL1, CHSEL9, CHSEL10 and CHSEL17 */
  ADC1->CHSELR = ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL2
    | ADC_CHSELR_CHSEL3 | ADC_CHSELR_CHSEL4;
  /* (4) Enable interrupts on EOC, EOSEQ and overrrun */
  ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE;
}

int adc_up() {
  adc_calibration();
  adc_enable();

  // TODO perhaps use the adc_watch_enable function
}

int adc_down() {
	/* A.7.3 ADC disable sequence code example */
	/* (1) Stop any ongoing conversion */
	ADC1->CR |= ADC_CR_ADSTP;
	/* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
	while ((ADC1->CR & ADC_CR_ADSTP) != 0) {
		/* For robust implementation, add here time-out management */
	}
	/* (3) Disable the ADC */
	ADC1->CR |= ADC_CR_ADDIS;
	/* (4) Wait until the ADC is fully disabled */
	while ((ADC1->CR & ADC_CR_ADEN) != 0) {
		/* For robust implementation, add here time-out management */
	}
}
