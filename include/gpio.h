#ifndef _GPIO_H_
#define _GPIO_H_

#include <flextimus.h>
#include <stdint.h>

/* A gpio_pin_t is just the uint16_t of the pin combined with a bit for which
 * GPIO peripheral the pin is on. */
typedef uint32_t gpio_pin_t;

#define GPIO_A            0x00010000
#define GPIO_B            0x00020000
#define GPIO_C            0x00040000

/* All GPIO pins fit within the lower 16 bits. */
#define GPIO_PIN_MASK     0x0000FFFF
#define GPIO_A2           (GPIO_A | GPIO_Pin_2)
#define GPIO_B0           (GPIO_B | GPIO_Pin_0)
#define GPIO_B1           (GPIO_B | GPIO_Pin_1)
#define GPIO_B3           (GPIO_B | GPIO_Pin_3)
#define GPIO_B4           (GPIO_B | GPIO_Pin_4)

void* gpio_perf(gpio_pin_t gpio_pin);
void gpio_clock(gpio_pin_t gpio_pin, FunctionalState NewState);
void gpio_up(gpio_pin_t gpio_pin);
void gpio_down(gpio_pin_t gpio_pin);
void gpio_on(gpio_pin_t gpio_pin);
void gpio_off(gpio_pin_t gpio_pin);
void gpio_input(gpio_pin_t gpio_pin);
int gpio_asserted(gpio_pin_t gpio_pin);

#endif /* _GPIO_H_ */
