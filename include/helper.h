/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Helper funcions used throughout all targets
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void  Delay(const int d);
void  USART_Setup(void);
void  USART_Putstr(char *str);
void  USART_Clearscreen(void);
char *USART_itoa(int16_t i, char *p);

#endif /* _HELPER_H_ */
