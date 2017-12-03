#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Outputs */
#define PAUSE_LED           GPIO_B0
#define CONFIG_LED          GPIO_B1

/* Inputs */
#define FLEX_SENSOR         GPIO_A0
#define PAUSE_BUTTON        GPIO_B3
#define CONFIG_BUTTON       GPIO_B4
#define BUZZER              GPIO_A2

/* Buzzer only allowed to be on for so long while configuring */
#define BUZZER_TIMEOUT 1000

#define DEFAULT_MIN    2000
#define DEFAULT_MAX    2000

#endif /* _CONFIG_H_ */
