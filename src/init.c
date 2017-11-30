#include <interrupt_handler.h>
void init(void);
int main(void);
void button_irq_handler(void);
void Default_Handler(void);
void debug_printIPSR(void);
// The following are 'declared' in the linker script
extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;
// the section "vectors" is placed at the beginning of flash
// by the linker script
const void * Vectors[] __attribute__((section(".vectors"))) ={
	(void *)0x20001000, 	/* Top of stack (4k) */
	init,   		/* Reset Handler */
	Default_Handler,	/* NMI */
	Default_Handler,	/* Hard Fault */
	Default_Handler,	/* MemManage */
	Default_Handler,	/* Reserved  */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* SVC Call */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* PendSV */
	Default_Handler,	/* SysTick */
/* External interrupt handlers follow */
	Default_Handler, 	/* 0: WWDG */
	Default_Handler, 	/* 1: PVD and VDDIO2 supply comparator interrupt */
	Default_Handler, 	/* 2: RTC */
	Default_Handler, 	/* 3: FLASH */
	Default_Handler, 	/* 4: RCC */
	button_irq_handler, 	/* 5: EXTI0_1 */
	button_irq_handler, 	/* 6: EXTI2_3 */
	button_irq_handler, 	/* 7: EXTI4_15 */
	Default_Handler, 	/* 8: TSC */
	Default_Handler, 	/* 9: DMA_CH1 */
	Default_Handler, 	/* 10: DMA_CH2_3, DMA2_CH_1_2 */
	Default_Handler, 	/* 11: DMA_CH4_5_6_7, DMA2_CH3_4_5 */
	adc_handler,     	/* 12: ADC_COMP */
	Default_Handler, 	/* 13: TIM1_BRK_UP_TRG_COM */
	Default_Handler, 	/* 14: TIM1_CC */
	Default_Handler, 	/* 15: TIM2 */
	Default_Handler, 	/* 16: TIM3 */
	Default_Handler, 	/* 17: TIM6_DAC*/
	Default_Handler, 	/* 18: TIM7 */
	Default_Handler, 	/* 19: TIM14 */
	Default_Handler, 	/* 20: TIM15 */
	Default_Handler, 	/* 21: TIM16 */
	Default_Handler, 	/* 22: TIM17 */
	Default_Handler, 	/* 23: I2C1 */
	Default_Handler, 	/* 24: I2C2 */
	Default_Handler, 	/* 25: SPI1 */
	Default_Handler, 	/* 26: SPI2 */
	Default_Handler, 	/* 27: USART1 */
	Default_Handler, 	/* 28: USART2 */
	Default_Handler, 	/* 29: USART3_4_5_6_7_8 */
	Default_Handler, 	/* 30: CEC_CAN */
	Default_Handler 	/* 31: USB */
};
void init()
{
// do global/static data initialization
	unsigned char *src;
	unsigned char *dest;
	unsigned len;
	src= &INIT_DATA_VALUES;
	dest= &INIT_DATA_START;
	len= &INIT_DATA_END-&INIT_DATA_START;
	while (len--)
		*dest++ = *src++;
// zero out the uninitialized global/static variables
	dest = &BSS_START;
	len = &BSS_END - &BSS_START;
	while (len--)
		*dest++=0;
	main();
}


void Default_Handler()
{
	while(1);
}
