#include <adc.h>
#include <gpio.h>
#include <flextimus.h>
#include "config.h"
#include "hd44780.h"
#include <stm32f042.h>

/* The state of Flextimus Prime. This controls the main loop. */
typedef enum {
  FLEXTIMUS_PRIME_ON,
  FLEXTIMUS_PRIME_OFF,
  FLEXTIMUS_PRIME_SLEEP,
} flextimus_prime_state_t;

/* The ADC state type lets us keep track of what we should be doing with the ADC
 * when we get interrupts. */
typedef enum {
  ADC_IDLE,
  ADC_READY,
  ADC_CONVERTED,
} adc_state_t;

/* One global to rule them all */
struct {
  bool paused;
  bool configuring;
  bool buzzer_timedout;
  unsigned int buzzer_timeout;
  struct {
    uint32_t max;
    uint32_t min;
    uint32_t volts;
    uint32_t millivolts;
    adc_state_t state;
  } adc;
  flextimus_prime_state_t state;
} flextimus_prime;

void adc_null_callback() {}

void assert_failed(uint8_t* file, uint32_t line) {
  while (1) {}
}

void flextimus_prime_default_bounds() {
  flextimus_prime.adc.min = DEFAULT_MIN;
  flextimus_prime.adc.max = DEFAULT_MAX;
}

void flextimus_prime_init() {
  flextimus_prime.paused = false;
  flextimus_prime.configuring = false;
  flextimus_prime.buzzer_timeout = 0;
  flextimus_prime.buzzer_timedout = false;
  flextimus_prime.adc.max = DEFAULT_MAX;
  flextimus_prime.adc.min = DEFAULT_MIN;
  flextimus_prime.adc.state = ADC_IDLE;
}

void configure_gpios() {
	// Power up PORTA
	RCC_AHBENR |= BIT17;
	// Power up PORTB
	RCC_AHBENR |= BIT18;

  // Set Inputs
  /*
  gpio_input(PAUSE_BUTTON);
  gpio_input(CONFIG_BUTTON);
  */
  GPIOB_MODER |= BIT6; // BTN1 - Pause
  GPIOB_MODER &= ~BIT7; // "" B3
  GPIOB_MODER |= BIT8; // BTN2 - Config
  GPIOB_MODER &= ~BIT9; // "" B4

  // Set Outputs
  /*
  gpio_up(BUZZER);
  gpio_up(PAUSE_LED);
  gpio_up(CONFIG_LED);
  */
  GPIOA_MODER |= BIT4; // Buzzer
  GPIOA_MODER &= ~BIT5; // "" A2
  GPIOB_MODER |= BIT0; // LED1 - Pause
  GPIOB_MODER &= ~BIT1; // "" B0
  GPIOB_MODER |= BIT2; // LED2 - Config
  GPIOB_MODER &= ~BIT3; // "" B1
}

int main(void) {
  bool running = true;
  adc_status_t adc_status;
  debouncer_t debounce_pause, debounce_config;

  debounce_init(&debounce_pause);
  debounce_init(&debounce_config);

  __disable_irq();

  configure_gpios();

  adc_status = adc_up(FLEX_SENSOR, adc_null_callback);
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

  //LCD Test code. NOTE: WILL HANG FOREVER IF YOU DON'T HAVE AN LCD ATTACHED
  //TODO_MAX: Update documentation with physical set up. Replicate final schematic set up.

  /*HD44780_Setup();
  HD44780_PowerOn();
  HD44780_Puts((uint8_t *)"Hello");
  HD44780_GotoXY(2,1);
  HD44780_Puts((uint8_t *)"World!");
  delay(5000000);
  HD44780_Clear();*/

  gpio_off(BUZZER);
  gpio_off(PAUSE_LED);
  gpio_off(CONFIG_LED);

  while (running) {
    /* Read from ADC forever */
    adc_status = adc_read();
    /* Set the volts and millivolts for our debugging convenience in GDB */
    flextimus_prime.adc.volts = ADC_VOLTS(adc_status.data);
    flextimus_prime.adc.millivolts = ADC_MILLIVOLTS(adc_status.data);
    /* Button press handlers */
    if (gpio_asserted_debounce(PAUSE_BUTTON, &debounce_pause)) {
      flextimus_prime_pause_pressed();
    } else if (gpio_asserted_debounce(CONFIG_BUTTON, &debounce_config)) {
      flextimus_prime_config_pressed();
    }
    /* Configuration */
    if (flextimus_prime.configuring == true) {
      if (adc_status.data > flextimus_prime.adc.max) {
        flextimus_prime.adc.max = adc_status.data;
      } else if (adc_status.data < flextimus_prime.adc.min) {
        flextimus_prime.adc.min = adc_status.data;
      }
    } else {
      /* If we are within range OR set to defaults OR timed out then turn off the
       * buzzer */
      if ((flextimus_prime.adc.max == DEFAULT_MAX ||
          flextimus_prime.adc.min == DEFAULT_MIN) ||
          ((adc_status.data < flextimus_prime.adc.max) &&
          (adc_status.data > flextimus_prime.adc.min)) ||
          flextimus_prime.buzzer_timedout == true) {
        // gpio_off(BUZZER);
        gpio_off(CONFIG_LED);
      } else if (((adc_status.data > flextimus_prime.adc.max) ||
          (adc_status.data < flextimus_prime.adc.min))) {
        if (!flextimus_prime.buzzer_timedout == true) {
          ++flextimus_prime.buzzer_timeout;
        }
        if (flextimus_prime.paused == true) {
          // gpio_on(BUZZER);
          gpio_on(CONFIG_LED);
          flextimus_prime.buzzer_timedout = false;
          flextimus_prime.buzzer_timeout = 0;
        }
      }
    }
    switch (flextimus_prime.state) {
    case FLEXTIMUS_PRIME_SLEEP:
      PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
    case FLEXTIMUS_PRIME_OFF:
      running = false;
    case FLEXTIMUS_PRIME_ON:
    default:
      continue;
    }
  }

  gpio_down(BUZZER);
  gpio_down(PAUSE_LED);
  gpio_down(CONFIG_LED);

  gpio_down(PAUSE_BUTTON);
  gpio_down(CONFIG_BUTTON);

  adc_status = adc_down(FLEX_SENSOR);
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

  /* The fall of Flextimus Prime */
  assert_param(NULL);

  return 0;
}

// Function to pause the alert system
void flextimus_prime_pause_pressed() {
  if (flextimus_prime.paused == false) {
    flextimus_prime.paused = true;
    gpio_on(PAUSE_LED);
  } else {
    flextimus_prime.paused = false;
    gpio_off(PAUSE_LED);
  }
}

void flextimus_prime_config_pressed() {
  adc_status_t adc_status;

  if (flextimus_prime.configuring == false) {
    /* Start configuring */
    flextimus_prime_default_bounds();
    flextimus_prime.buzzer_timeout = 0;
    flextimus_prime.buzzer_timedout = false;
    flextimus_prime.configuring = true;
    // gpio_on(CONFIG_LED);
  } else {
    flextimus_prime.configuring = false;
    // gpio_off(CONFIG_LED);
  }
}

// IRQ handler for both button interrupts
void button_irq_handler() {
  if (gpio_asserted_irq(PAUSE_BUTTON)) {
    flextimus_prime_pause_pressed();
  } else if (gpio_asserted_irq(CONFIG_BUTTON)) {
    flextimus_prime_config_pressed();
  }
}
