#ifndef _LED_H_
#define _LED_H_

typedef enum {
  LED_NUCLEO,
} led_selection;

void led_up(led_selection which_led);
void led_down(led_selection which_led);
void led_on(led_selection which_led);
void led_off(led_selection which_led);

#endif /* _LED_H_ */
