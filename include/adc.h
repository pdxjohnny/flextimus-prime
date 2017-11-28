#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

typedef enum {
  ADC_STATUS_OK,
  ADC_STATUS_TIMEOUT,
  ADC_STATUS_INVALID_CONVERT_PIN,
  ADC_STATUS_CONVERSION_IN_PROGRESS,
  ADC_STATUS_NEED_CONVERSION_CALLBACK,
} adc_status_code;

typedef uint32_t adc_status_data_t;

typedef struct {
  adc_status_code code;
  adc_status_data_t data;
} adc_status_t;

#define ADC_ERROR(status)    (status.code != ADC_STATUS_OK)
extern adc_status_t ADC_OK;
extern adc_status_t ADC_TIMEOUT;
extern adc_status_t ADC_INVALID_CONVERT_PIN;
extern adc_status_t ADC_CONVERSION_IN_PROGRESS;
extern adc_status_t ADC_NEED_CONVERSION_CALLBACK;

typedef enum {
  ADC_CONVERT_PA1,
} adc_convert_t;

typedef adc_status_t adc_convertion_result;

extern adc_status_t (*adc_conversion_complete)(adc_convertion_result result);

adc_status_t adc_calibration();
adc_status_t adc_enable();
adc_status_t adc_read();
adc_status_t adc_select_conversion_pin(adc_convert_t pin_to_convert);
adc_status_t adc_watch_enable(adc_convert_t pin_to_convert,
    uint16_t vrefint_low, uint16_t vrefint_high);
adc_status_t adc_up();
adc_status_t adc_down();
adc_status_t adc_convert(adc_convert_t pin_to_convert);
adc_status_t adc_convert_async(adc_convert_t pin_to_convert,
    adc_status_t (*adc_conversion_complete)(adc_convertion_result result));

void adc_handler();

#endif /* _ADC_H_ */
