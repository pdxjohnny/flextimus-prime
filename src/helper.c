/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Helper funcions used throughout all targets
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "helper.h"

/**
  * @brief  This function implements a delay.
  *         If the optimization level is set to -O3, this function takes 8
  *         cycles. To create a delay of 1 second, use the following function
  *         call: Delay(SystemCoreClock/8);
  * @param  d: number delay loops (1 loop takes 8/SystemCoreClock sec.)
  * @retval None
  */
void Delay(const int d)
{
  volatile int i;

  for(i=d; i>0; i--){ ; }

  return;
}
