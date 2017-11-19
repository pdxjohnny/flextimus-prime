/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : HD44780-based LCD implementation
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************
   Hardware:
                                
                        5V        HD44780-based LCD
                        |        +---------+
            GND|--[10k]-+        |         |
                    |   |  GND|--+ 1 Vss   |
   ------------+    |   +--------+ 2 Vdd   |
               |    +------------+ 3 Vo    |
            PA5+-----------------+ 4 RS    |
            PA6+-----------------+ 5 RW    |
            PA7+-----------------+ 6 E     |
               |              nc-+ 7 DB0   |
               |              nc-+ 8 DB1   |
               |              nc-+ 9 DB2   |
               |              nc-+10 DB3   |
            PA1+-----------------+11 DB4   |
            PA2+-----------------+12 DB5   |
            PA3+-----------------+13 DB6   |
            PA4+-----------------+14 DB7   |
               |     Vdd--[180]--+15 A     |
   ------------+     GND|--------+16 K     |
                                 |         |
                                 +---------+

  http://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller

******************************************************************************/
#ifndef _HD44780_H_
#define _HD44780_H_

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define HD44780_GPIO_Port    (GPIOA)

#define HD44780_GPIO_Pin_DB4 (GPIO_Pin_1)  /* PA1: Data bus line 4 */
#define HD44780_GPIO_Pin_DB5 (GPIO_Pin_2)  /* PA2: Data bus line 5 */
#define HD44780_GPIO_Pin_DB6 (GPIO_Pin_3)  /* PA3: Data bus line 6 */
#define HD44780_GPIO_Pin_DB7 (GPIO_Pin_4)  /* PA4: Data bus line 7 */

#define HD44780_GPIO_Pin_RS  (GPIO_Pin_5)  /* PA5: Register Select */
#define HD44780_GPIO_Pin_RW  (GPIO_Pin_6)  /* PA6: Read/Write      */
#define HD44780_GPIO_Pin_E   (GPIO_Pin_7)  /* PA7: Register Select */

// Instruction Register overview
#define HD44780_CLEAR_DISPLAY            ((uint8_t)(0x01))
         
#define HD44780_RETURN_HOME              ((uint8_t)(0x02))
         
#define HD44780_ENTRY_MODE_SET           ((uint8_t)(0x04))
#define HD44780_ENTRY_MODE_SET_ID        ((uint8_t)(0x02))
#define HD44780_ENTRY_MODE_SET_SH        ((uint8_t)(0x01))

#define HD44780_DISPLAY_ONOFF_CONTROL    ((uint8_t)(0x08))
#define HD44780_DISPLAY_ONOFF_CONTROL_D  ((uint8_t)(0x04))
#define HD44780_DISPLAY_ONOFF_CONTROL_C  ((uint8_t)(0x02))
#define HD44780_DISPLAY_ONOFF_CONTROL_B  ((uint8_t)(0x01))

#define HD44780_CURSOR_DISPLAY_SHIFT     ((uint8_t)(0x10))
#define HD44780_CURSOR_DISPLAY_SHIFT_SC  ((uint8_t)(0x08))
#define HD44780_CURSOR_DISPLAY_SHIFT_RL  ((uint8_t)(0x04))

#define HD44780_FUNCTION_SET             ((uint8_t)(0x20))
#define HD44780_FUNCTION_SET_DL          ((uint8_t)(0x10))
#define HD44780_FUNCTION_SET_N           ((uint8_t)(0x08))
#define HD44780_FUNCTION_SET_F           ((uint8_t)(0x04))

#define HD44780_SET_CGRAM_ADDRESS        ((uint8_t)(0x40))

#define HD44780_SET_DDRAM_ADDRESS        ((uint8_t)(0x80))

// Display status bits
#define HD44780_STATUS_BF                (0x0)
#define HD44780_STATUS_AC                (0x1)
#define HD44780_STATUS_BF_AC             (0x2)

// Databus direction
#define HD44780_DATABUS_GPIO_MODE_IN     (GPIO_Mode_IN)
#define HD44780_DATABUS_GPIO_MODE_OUT    (GPIO_Mode_OUT)

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void HD44780_Setup(void);
void HD44780_PowerOn(void);
void HD44780_Clear(void);
void HD44780_Home(void);
void HD44780_GotoXY(uint8_t x, uint8_t y);
void HD44780_WriteInstruction(uint8_t ins, bool checkBusy);
void HD44780_WriteData(uint8_t d, bool checkBusy);
uint8_t HD44780_ReadData(void);
uint8_t HD44780_ReadStatus(uint8_t bf_ac);

// Convenience functions
void HD44780_Putc(uint8_t c);
void HD44780_Puts(uint8_t *str);
uint8_t HD44780_Getc(void);

#endif /* _HD44780_H_ */
