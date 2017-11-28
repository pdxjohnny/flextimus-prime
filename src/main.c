/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <adc.h>
#include "main.h"
#include "hd44780.h"

/** @addtogroup STM32F0xx_StdPeriph_Templates
  * @{
  */


void delay(int dly) {
  while (dly--);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */
  uint32_t temp;
  adc_status_t adc_status;
  /* Add your application code here */
  
  // RCC_CFGR |= BIT14;
  // Power up PORTB && PORTA && PORTF (NUCLEO)
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN;

  	GPIOA->MODER |= GPIO_MODER_MODER3_0; // make bit3  an output
  	GPIOA->MODER &= ~GPIO_MODER_MODER3_1; // make bit3  an output

  //LCD Test code. NOTE: WILL HANG FOREVER IF YOU DON'T HAVE AN LCD ATTACHED
  //TODO_MAX: Update documentation with physical set up. Replicate final schematic set up.

  /*HD44780_Setup();
  HD44780_PowerOn();
  HD44780_Puts((uint8_t *)"Hello");
  HD44780_GotoXY(2,1);
  HD44780_Puts((uint8_t *)"World!");
  delay(5000000);
  HD44780_Clear();*/  


  adc_status = adc_up();
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

  __enable_irq();

	while (1) {
		GPIOB->ODR |= GPIO_MODER_MODER1_1;
		delay(500000);
    adc_status = adc_convert(ADC_CONVERT_PA1);
    if (ADC_ERROR(adc_status)) {
      assert_failed(__FILE__, __LINE__);
    }
    temp = adc_status.data;
		GPIOB->ODR &= ~GPIO_MODER_MODER1_1;
		delay(500000);
	}

  adc_status = adc_down();
  if (ADC_ERROR(adc_status)) {
    assert_failed(__FILE__, __LINE__);
  }

  /* TODO go to sleep
   *
   * Calls the ARM `WFI` instruction.
   *
   * WFI (Wait For Interrupt) makes the processor suspend execution (Clock is
   * stopped) until one of the following events take place:
   * - An IRQ interrupt
   * - An FIQ interrupt
   * - A Debug Entry request made to the processor.
   */
  // cpu_sleep();

	return 0;
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  while (1) {}
}

/**
  * @}
  */


