/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : HD44780-based LCD implementation
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "hd44780.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------
void HD44780_SetDatabusGPIOMode(GPIOMode_TypeDef mode);
void HD44780_ToggleE_Write(void);
void HD44780_ToggleE_Read(void);

/**
  * @brief  This function sets up the LCD HD44780 hardware as mentioned in the
  *         hardware description in the header file.
  * @param  None
  * @retval None
  */
void HD44780_Setup(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  Delay(SystemCoreClock/8/25); // ~40 ms
  
  // Enable GPIO peripheral
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  // Configure GPIO pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = (HD44780_GPIO_Pin_RS |
                                 HD44780_GPIO_Pin_RW |
                                 HD44780_GPIO_Pin_E  );
  
  GPIO_Init(HD44780_GPIO_Port, &GPIO_InitStructure);
}

/**
  * @brief  This function sets up the databus for reading or writing. 
  * @param  mode: HD44780_DATABUS_GPIO_MODE_IN: mode of GPIO's set to input
  *               HD44780_DATABUS_GPIO_MODE_OUT: mode of GPIO's set to output
  * @retval None
  */
void HD44780_SetDatabusGPIOMode(GPIOMode_TypeDef mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   
  // Configure GPIO pins
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Mode = mode;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = (HD44780_GPIO_Pin_DB4 |
                                 HD44780_GPIO_Pin_DB5 |
                                 HD44780_GPIO_Pin_DB6 |
                                 HD44780_GPIO_Pin_DB7 );
  
  GPIO_Init(HD44780_GPIO_Port, &GPIO_InitStructure);
}

/**
  * @brief  This function powers on and initializes the HD44780 LCD module by
  *         writing data to 6 instruction registers. The first three
  *         instructions use a delay routine, because the datasheet mentions
  *         that the busyflag cannot be checked. The other three instructions
  *         do not need a delay, because in the function
  *         HD44780_WriteInstruction() the busy flag is checked before writing
  *         the instruction.
  *         After executing this function:
  *         - Function_set: 2 lines x 16, display on, cursor off, no blink
  *         - Return home, set DDRAM address 0 in address counter
  * @param  None
  * @retval None
  */
void HD44780_PowerOn(void)
{
  // Wait for more then 40 ms after Vdd rises to 4.5V
  Delay(SystemCoreClock/8/25); // ~40 ms

  // Function set
  HD44780_WriteInstruction(HD44780_FUNCTION_SET   |
                           HD44780_FUNCTION_SET_DL, false );
  
  Delay(SystemCoreClock/8/25000); // ~40 us
    
  // Function set
  HD44780_WriteInstruction(HD44780_FUNCTION_SET |
  		           HD44780_FUNCTION_SET_DL, false);
                           
  Delay(SystemCoreClock/8/25000); // ~40 us
  
  // Function set
  HD44780_WriteInstruction(HD44780_FUNCTION_SET |
  		           HD44780_FUNCTION_SET_DL, false);

  HD44780_WriteInstruction(HD44780_FUNCTION_SET, true);
  
  // Function set
  //HD44780_WriteInstruction(HD44780_FUNCTION_SET |
  //		           HD44780_FUNCTION_SET_N, true);
  
   
  
  // Display ON/OFF control
  //HD44780_WriteInstruction(HD44780_DISPLAY_ONOFF_CONTROL, false);

  // Display clear
  //HD44780_WriteInstruction(HD44780_CLEAR_DISPLAY, false);

  HD44780_WriteInstruction(HD44780_DISPLAY_ONOFF_CONTROL |
                           HD44780_DISPLAY_ONOFF_CONTROL_D |
			   HD44780_DISPLAY_ONOFF_CONTROL_C |
			   HD44780_DISPLAY_ONOFF_CONTROL_B, true);
  // Entry mode set
  // Set the LCD unit to increment the address counter and shift the cursor to
  // the right after each data transaction. The display does not shift.  
  HD44780_WriteInstruction(HD44780_ENTRY_MODE_SET |
                           HD44780_ENTRY_MODE_SET_ID, false);

  
}

/**
  * @brief  This function clears (0x00) the entire DDRAM and sets DDRAM address
  *         to 0x00.
  * @param  None
  * @retval None
  */
void HD44780_Clear(void)
{
  HD44780_WriteInstruction(HD44780_CLEAR_DISPLAY, true);
}

/**
  * @brief  This function sets the cursor to it's home position
  *         x=0, y=0. It also switches a shifted display back to an unshifted
  *         state.
  * @param  None
  * @retval None
  */
void HD44780_Home(void)
{
  HD44780_WriteInstruction(HD44780_RETURN_HOME, true);
}

/**
  * @brief  This function sets the cursor to position (x,y):
  *
  *         +----+----+---- ----+----+----+
  *         | 0,0| 1,0|         |38,0|39,0|
  *         +----+----+---- ----+----+----+
  *         | 0,1| 1,1|         |38,1|39,1|
  *         +----+----+---- ----+----+----+
  * 
  * @param  x: horizontal position (0-39)
  *         y: vertical position (0-1)
  * @retval None
  */
void HD44780_GotoXY(uint8_t x, uint8_t y)
{
  HD44780_WriteInstruction(HD44780_SET_DDRAM_ADDRESS + (y * 0x40) + x, true);
}

/**
  * @brief  This function writes an instruction to the HD44780. The RS and  
  *         RW lines are both reset to logic 0 and then first the high nibble
  *         of the instruction is written to the databus. 
  * @param  ins: an instruction that the LCD controller can interpret. For an
  *              overview of all instructions see the header file and/or the
  *              LCD controller datasheet:
  *              http://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
  * @retval None
  */
void HD44780_WriteInstruction(uint8_t ins, bool checkBusy)
{
  // Wait until HD44780 ready
  if (checkBusy)
    while(HD44780_ReadStatus(HD44780_STATUS_BF) != RESET){;}
  
  HD44780_SetDatabusGPIOMode(HD44780_DATABUS_GPIO_MODE_OUT);

  GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_RS |
                                    HD44780_GPIO_Pin_RW );
  
	// High nibble
	if((ins & 0x10) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	if((ins & 0x20) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	if((ins & 0x40) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	if((ins & 0x80) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	
	HD44780_ToggleE_Write();
	
	// Low nibble
	if((ins & 0x01) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	if((ins & 0x02) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	if((ins & 0x04) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	if((ins & 0x08) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	else                 {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	
	HD44780_ToggleE_Write();
}

/**
  * @brief  This function writes 8-bit data to the HD44780. Data will be
  *         visible at the current cursur position.
  * @param  d: 8-bit data to be written. Verify the characterset how this
  *            8-bit data translates to display characters, for instance:
  *            http://www.msc-ge.com/download/displays/dabla_allg/ge-c1602b-yyh-jt-r.pdf
  * @retval None
  */
void HD44780_WriteData(uint8_t d, bool checkBusy)
{
  // Wait until HD44780 ready
  if (checkBusy)
    while(HD44780_ReadStatus(HD44780_STATUS_BF) != RESET){;}
  
  HD44780_SetDatabusGPIOMode(HD44780_DATABUS_GPIO_MODE_OUT);
  
  GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_RW );
  GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_RS );
  
	// High nibble
	if((d & 0x10) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	if((d & 0x20) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	if((d & 0x40) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	if((d & 0x80) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}

  HD44780_ToggleE_Write();

	// Low nibble
	if((d & 0x01) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB4);}
	if((d & 0x02) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB5);}
	if((d & 0x04) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB6);}
	if((d & 0x08) == 0){GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	else               {GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_DB7);}
	
	HD44780_ToggleE_Write();
}

/**
  * @brief  This function reads 8-bit data from the HD44780. Data will be
  *         read from the current cursur position.
  * @param  None
  * @retval 8-bit data read from the current cursor position. Verify the
  *         characterset how this 8-bit data translates to display
  *         characters, for instance:
  *         http://www.msc-ge.com/download/displays/dabla_allg/ge-c1602b-yyh-jt-r.pdf
  */
uint8_t HD44780_ReadData(void)
{
  uint16_t data;
  uint8_t tmp=0x00;

  // Wait until HD44780 ready
  //while(HD44780_ReadStatus(HD44780_STATUS_BF) != RESET){;}

  HD44780_SetDatabusGPIOMode(HD44780_DATABUS_GPIO_MODE_IN);

  GPIO_SetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_RS |
                                  HD44780_GPIO_Pin_RW);
  
  HD44780_ToggleE_Read();
  
  // Read all bits at once
  data = GPIO_ReadInputData(HD44780_GPIO_Port);
  
  // High nibble
  if((data & HD44780_GPIO_Pin_DB4) != 0){tmp |= 0x10;}
  if((data & HD44780_GPIO_Pin_DB5) != 0){tmp |= 0x20;}
  if((data & HD44780_GPIO_Pin_DB6) != 0){tmp |= 0x40;}
  if((data & HD44780_GPIO_Pin_DB7) != 0){tmp |= 0x80;}

  HD44780_ToggleE_Read();

  // Read all bits at once
  data = GPIO_ReadInputData(HD44780_GPIO_Port);

  // Low nibble
  if((data & HD44780_GPIO_Pin_DB4) != 0){tmp |= 0x01;}
  if((data & HD44780_GPIO_Pin_DB5) != 0){tmp |= 0x02;}
  if((data & HD44780_GPIO_Pin_DB6) != 0){tmp |= 0x04;}
  if((data & HD44780_GPIO_Pin_DB7) != 0){tmp |= 0x08;}

  return(tmp);
}

/**
  * @brief  This function reads 8-bit status from the HD44780.
  * @param  bf_ac: HD44780_STATUS_BF   : function returns only the Busy Flag,
  *                                      all AC bits are cleared  
  *                HD44780_STATUS_AC   : function returns only the Address
  *                                      Counter, the BF bit is cleared
  *                HD44780_STATUS_BF_AC: function returns both the Busy Flag
  *                                      and the Address Counter
  * @retval 8-bit data read from the BF_AC register
  */
uint8_t HD44780_ReadStatus(uint8_t bf_ac)
{
  uint16_t data;
  uint8_t status=0x00;

  HD44780_SetDatabusGPIOMode(HD44780_DATABUS_GPIO_MODE_IN);

  GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_RS);
  GPIO_SetBits  (HD44780_GPIO_Port, HD44780_GPIO_Pin_RW);
  
  HD44780_ToggleE_Read();
  
  // Read all bits at once
  data = GPIO_ReadInputData(HD44780_GPIO_Port);
  
  // High nibble
  if((data & HD44780_GPIO_Pin_DB4) != 0){status |= 0x10;}
  if((data & HD44780_GPIO_Pin_DB5) != 0){status |= 0x20;}
  if((data & HD44780_GPIO_Pin_DB6) != 0){status |= 0x40;}
  if((data & HD44780_GPIO_Pin_DB7) != 0){status |= 0x80;}

  HD44780_ToggleE_Read();

  // Read all bits at once
  data = GPIO_ReadInputData(HD44780_GPIO_Port);

  // Low nibble
  if((data & HD44780_GPIO_Pin_DB4) != 0){status |= 0x01;}
  if((data & HD44780_GPIO_Pin_DB5) != 0){status |= 0x02;}
  if((data & HD44780_GPIO_Pin_DB6) != 0){status |= 0x04;}
  if((data & HD44780_GPIO_Pin_DB7) != 0){status |= 0x08;}
  
  // Return requested result
  if(bf_ac == HD44780_STATUS_BF)
  {
    return(status & 0x80);
  }
  else if(bf_ac == HD44780_STATUS_AC)
  {
    return(status & 0x7F);
  }
  else // HD44780_STATUS_BF_AC
  {
    return(status); 
  }  
}

/**
  * @brief  This function writes an ASCII character to the display at the 
  *         current cursor position.
  * @param  c: character to be displayed
  * @retval None
  */
void HD44780_Putc(uint8_t c)
{
  HD44780_WriteData(c, true);
}

/**
  * @brief  This function writes a string of characters to the HD44780.
  * @param  str: pointer to NULL termineted string to be displayed
  * @retval None
  */
void HD44780_Puts(uint8_t *str)
{
  while(*str)
  {
    HD44780_Putc(*str++);
  }
}

/**
  * @brief  This function reads an ASCII character from the display at the 
  *         current cursor position.
  * @param  None
  * @retval Character at current cursor position
  */
uint8_t HD44780_Getc(void)
{
  return(HD44780_ReadData());
}

/**
  * @brief  This function toggles the Enable line for an write operation.
  *         The lines RS, RW and Db4-7 must already have their valid values.
  *         Delays are used to make sure the following timings are respected:
  *         - Address setup time (tAS): ~200 ns
  *         - Enable line puls width (high level) (tPW): ~200 ns
  *         - Enable line total cycle time is 1200 ns       
  *         
  *         ------ -------------------------- ----------
  *         RS    X                          X
  *         ------ -------------------------- ----------
  *                |
  *         ------ |                          ----------
  *         R/W   \|                         /
  *         -------|------------------------------------
  *                |
  *                |<---tAS--->|<---tPW--->|<-------800 ns------->|
  *                            +-----------+                      +--
  *         E                  |           |                      |
  *         -------------------+           +----------------------+
  *
  *         ------------------------- ----------- ------
  *         DB4-7                    X   valid   X
  *         ------------------------- ----------- ------
  *
  * @param  None
  * @retval None
  */
void HD44780_ToggleE_Write(void)
{
  // Address setup time
  Delay(SystemCoreClock/8/5000000); // ~200 ns

	GPIO_SetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_E);

  // Enable line puls width (high level)
  Delay(SystemCoreClock/8/2220000); // ~200 ns

	GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_E);
  
  // Enable line total cycle time is 1200 ns
  //Delay(SystemCoreClock/8/1250000); // ~800 ns
}

/**
  * @brief  This function toggles the Enable line for a read operation.
  *         The lines RS and RW must have already have their valid values.
  *         After the function returns, the user has a minimum of 10 ns (tH)
  *         to read the data from DB4-7.
  *         Delays are used to make sure the following timings are respected:
  *         - Enable line total cycle time is 1200 ns       
  *         - Address setup time (tAS): ~200 ns
  *         - Enable line puls width (high level) (tPW): ~200 ns
  *      
  *                            ------ -------------------------- ----------
  *                            RS    X                          X
  *                            ------ -------------------------- ----------
  *                                   |
  *                            -------|-----------------------------------
  *                            R/W   /|                         \
  *                            ------ |                          -----------
  *                                   |
  *            |<-------800 ns------->|<---tAS--->|<---tPW--->|
  *         ---+                                  +-----------+            
  *         E  |                                  |           |            
  *            -----------------------------------+           +------------
  *                   
  *                            ------------------------- ----------- ------
  *                            DB4-7                    X   valid   X
  *                            ------------------------- ----------- ------
  *
  * @param  None
  * @retval None
  */
void HD44780_ToggleE_Read(void)
{
  // Enable line total cycle time is 1200 ns
  //Delay(SystemCoreClock/8/1250000); // ~800 ns

  // Address setup time
  Delay(SystemCoreClock/8/5000000); // ~200 ns

	GPIO_SetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_E);

  // Enable line puls width (high level)
  Delay(SystemCoreClock/8/2220000); // ~200 ns

	GPIO_ResetBits(HD44780_GPIO_Port, HD44780_GPIO_Pin_E);
}

