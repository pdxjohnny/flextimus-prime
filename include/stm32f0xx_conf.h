/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_conf.h 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Library configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0XX_CONF_H
#define __STM32F0XX_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Comment the line below to disable peripheral header file inclusion */
#include "stm32f0xx_adc.h"
#include "stm32f0xx_can.h"
#include "stm32f0xx_cec.h"
#include "stm32f0xx_crc.h"
#include "stm32f0xx_crs.h"
#include "stm32f0xx_comp.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_dbgmcu.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_exti.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_wwdg.h"
#include "stm32f0xx_misc.h"  /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
#define USE_FULL_ASSERT    1

#define ADC_CR_ADRDY        (1 << 0)

#endif /* __STM32F0XX_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
