#include <led.h>
#include <adc.h>
#include "main.h"
#include "config.h"
#include "hd44780.h"

static uint32_t volts, millivolts;
static int blink_rate;

void assert_failed(uint8_t* file, uint32_t line) {
  while (1) {}
}

void delay(int dly) {
  while (dly--);
}

adc_status_t adc_convert_async_callback(adc_status_t adc_status) {
  volts = ADC_VOLTS(adc_status.data);
  millivolts = ADC_MILLIVOLTS(adc_status.data);
  led_off(BLINK_LED);
  return ADC_OK;
}

adc_status_t adc_up_callback() {
  adc_status_t adc_status;
  led_on(BLINK_LED);
  adc_status = adc_convert_async(ADC_CONVERT_PA0, adc_convert_async_callback);
  if (ADC_ERROR(adc_status)) {
    return adc_status;
  }
  return ADC_OK;
}

int main(void) {
  adc_status_t adc_status;

  blink_rate = 200000;

  led_up(BLINK_LED);

  adc_status = adc_up(adc_up_callback);
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

  __enable_irq();

  //LCD Test code. NOTE: WILL HANG FOREVER IF YOU DON'T HAVE AN LCD ATTACHED
  //TODO_MAX: Update documentation with physical set up. Replicate final schematic set up.

  /*HD44780_Setup();
  HD44780_PowerOn();
  HD44780_Puts((uint8_t *)"Hello");
  HD44780_GotoXY(2,1);
  HD44780_Puts((uint8_t *)"World!");
  delay(5000000);
  HD44780_Clear();*/

	while (1) {
    led_on(BLINK_LED);
    delay(blink_rate);
    led_off(BLINK_LED);
    delay(blink_rate);
    // PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
	}

  led_down(BLINK_LED);

  adc_status = adc_down();
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

	return 0;
}
