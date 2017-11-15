#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

typedef enum {
  ADC_CONVERT_PIN_1,
} adc_convert_t;

extern int (*adc_conversion_complete)(int result);

int adc_calibration();
int adc_enable();
int adc_read();
void adc_handler();
int adc_watch_enable(uint16_t vrefint_low, uint16_t vrefint_high);
int adc_up();
int adc_down();

#endif /* _ADC_H_ */
