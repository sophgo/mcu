#include <stdint.h>
#include <system.h>
extern char __ld_stack_top[0];
extern void __ld_reset_handler(void);

/* real address to thumb mode */
#define RESET_HANDLER	((void *)((unsigned long)__ld_reset_handler + 1))

// void default_isr(void)
// {
// 	while (1)
// 		;
// }

void default_isr(void)
{
	dbg_printf("default isr\r\n");
	while (1)
		;
}

void __attribute__((weak, alias("default_isr"))) Reset_Handler(void);
void __attribute__((weak, alias("default_isr"))) NMI_Handler(void);
void __attribute__((weak, alias("default_isr"))) HardFault_Handler(void);
void __attribute__((weak, alias("default_isr"))) MemManage_Handler(void);
void __attribute__((weak, alias("default_isr"))) BusFault_Handler(void);
void __attribute__((weak, alias("default_isr"))) UsageFault_Handler(void);
void __attribute__((weak, alias("default_isr"))) SVC_Handler(void);
void __attribute__((weak, alias("default_isr"))) DebugMon_Handler(void);
void __attribute__((weak, alias("default_isr"))) PendSV_Handler(void);
void __attribute__((weak, alias("default_isr"))) SysTick_Handler(void);
void __attribute__((weak, alias("default_isr"))) WWDGT_IRQHandler(void); 			/* 0; !< window watchdog timer interrupt */
void __attribute__((weak, alias("default_isr"))) LVD_IRQHandler(void); 				/* 1; !< LVD through EXTI line detect interrupt */
void __attribute__((weak, alias("default_isr"))) TAMPER_IRQHandler(void); 			/* 2; !< tamper and timestamp through EXTI line detect */
void __attribute__((weak, alias("default_isr"))) RTC_IRQHandler(void); 				/* 3; !< RTC wakeup through EXTI line interrupt */
void __attribute__((weak, alias("default_isr"))) FMC_IRQHandler(void);				/* 4; !< FMC interrupt */
void __attribute__((weak, alias("default_isr"))) RCU_CTC_IRQHandler(void);			/* 5; !< RCU and CTC interrupt */
void __attribute__((weak, alias("default_isr"))) EXTI0_IRQHandler(void);			/* 6; !< EXTI line 0 interrupts */
void __attribute__((weak, alias("default_isr"))) EXTI1_IRQHandler(void);			/* 7; !< EXTI line 0 interrupts */
void __attribute__((weak, alias("default_isr"))) EXTI2_IRQHandler(void);			/* 8; !< EXTI line 0 interrupts */
void __attribute__((weak, alias("default_isr"))) EXTI3_IRQHandler(void);			/* 9; !< EXTI line 0 interrupts */
void __attribute__((weak, alias("default_isr"))) EXTI4_IRQHandler(void);			/* 10; !< EXTI line 0 interrupts */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel0_IRQHandler(void);	/* 11; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel1_IRQHandler(void);	/* 12; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel2_IRQHandler(void);	/* 13; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel3_IRQHandler(void);	/* 14; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel4_IRQHandler(void); 	/* 15; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel5_IRQHandler(void);	/* 16; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel6_IRQHandler(void);	/* 17; !< DMA0 channel0 Interrupt */
void __attribute__((weak, alias("default_isr"))) ADC_IRQHandler(void);				/* 18; !< ADC interrupt */
void __attribute__((weak, alias("default_isr"))) CAN0_TX_IRQHandler(void);			/* 19; !< CAN0 TX interrupt */
void __attribute__((weak, alias("default_isr"))) CAN0_RX0_IRQHandler(void);			/* 20; !< CAN0 RX0 interrupt */
void __attribute__((weak, alias("default_isr"))) CAN0_RX1_IRQHandler(void);			/* 21; !< CAN0 RX1 interrupt */
void __attribute__((weak, alias("default_isr"))) CAN0_EWMC_IRQHandler(void);		/* 22; !< CAN0 EWMC interrupt */
void __attribute__((weak, alias("default_isr"))) EXTI5_9_IRQHandler(void);			/* 23; !< EXTI[9:5] interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER0_BRK_IRQHandler(void);		/* 24; !< TIMER0 break and TIMER8 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER0_UP_IRQHandler(void);		/* 25; !< TIMER0 update and TIMER9 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER0_TRG_CMT_IRQHandler(void);	/* 26; !< TIMER0 trigger and commutation  and TIMER10 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER0_Channel_IRQHandler(void);	/* 27; !< TIMER0 channel capture compare interrupt  */
void __attribute__((weak, alias("default_isr"))) TIMER1_IRQHandler(void);			/* 28; !< TIMER1 interrupt */
void __attribute__((weak, alias("default_isr"))) TIMER2_IRQHandler(void);			/* 29; !< TIMER2 interrupt */
void __attribute__((weak, alias("default_isr"))) TIMER3_IRQHandler(void);			/* 30; !< TIMER3 interrupts */
void __attribute__((weak, alias("default_isr"))) I2C0_EV_IRQHandler(void);			/* 31; !< I2C0 event interrupt */
void __attribute__((weak, alias("default_isr"))) I2C0_ER_IRQHandler(void);			/* 32; !< I2C0 error interrupt */
void __attribute__((weak, alias("default_isr"))) I2C1_EV_IRQHandler(void);			/* 33; !< I2C1 event interrupt */
void __attribute__((weak, alias("default_isr"))) I2C1_ER_IRQHandler(void);			/* 34; !< I2C1 error interrupt */
void __attribute__((weak, alias("default_isr"))) SPI0_IRQHandler(void);				/* 35; !< SPI0 interrupt */
void __attribute__((weak, alias("default_isr"))) SPI1_I2S1ADD_IRQHandler(void);		/* 36; !< SPI1 interrupt */
void __attribute__((weak, alias("default_isr"))) USART0_IRQHandler(void);			/* 37; !< USART0 interrupt */
void __attribute__((weak, alias("default_isr"))) USART1_IRQHandler(void);			/* 38; !< USART1 interrupt */
void __attribute__((weak, alias("default_isr"))) USART2_IRQHandler(void);			/* 39; !< USART2 interrupt */
void __attribute__((weak, alias("default_isr"))) EXTI10_15_IRQHandler(void);		/* 40; !< EXTI[15:10] interrupts */
void __attribute__((weak, alias("default_isr"))) RTC_Alarm_IRQHandler(void);		/* 41; !< RTC alarm interrupt */
void __attribute__((weak, alias("default_isr"))) USBD_WKUP_IRQHandler(void);		/* 42; !< USBFS wakeup interrupt */
void __attribute__((weak, alias("default_isr"))) TIMER7_BRK_IRQHandler(void);		/* 43; !< TIMER7 break and TIMER11 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER7_UP_IRQHandler(void);		/* 44; !< TIMER7 update and TIMER12 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER7_TRG_CMT_IRQHandler(void);	/* 45; !< TIMER7 trigger and commutation and TIMER13 interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER7_Channel_IRQHandler(void);	/* 46; !< TIMER7 channel interrupts */
void __attribute__((weak, alias("default_isr"))) DMA0_Channel7_IRQHandler(void);	/* 47; !< DMA0 channel7 interrupt */
void __attribute__((weak, alias("default_isr"))) EXMC_IRQHandler(void);				/* 48; !< EXMC interrupt */
void __attribute__((weak, alias("default_isr"))) SDIO_IRQHandler(void);				/* 49; !< SDIO interrupt */
void __attribute__((weak, alias("default_isr"))) TIMER4_IRQHandler(void);			/* 50; !< TIMER4 interrupt */
void __attribute__((weak, alias("default_isr"))) SPI2_I2S2ADD_IRQHandler(void);		/* 51; !< SPI2interrupt */
void __attribute__((weak, alias("default_isr"))) UART3_IRQHandler(void);			/* 52; !< UART3 interrupt */
void __attribute__((weak, alias("default_isr"))) UART4_IRQHandler(void);			/* 53; !< UART4 interrupt */
void __attribute__((weak, alias("default_isr"))) TIMER5_DAC_IRQHandler(void);		/* 54; !< TIMER5 and DAC0 DAC1 underrun error interrupts */
void __attribute__((weak, alias("default_isr"))) TIMER6_IRQHandler(void);			/* 55; !< TIMER6 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel0_IRQHandler(void);	/* 56; !< DMA1 channel0 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel1_IRQHandler(void);	/* 57; !< DMA1 channel1 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel2_IRQHandler(void);	/* 58; !< DMA1 channel2 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel3_IRQHandler(void);	/* 59; !< DMA1 channel3 interrupts */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel4_IRQHandler(void);	/* 60; !< DMA1 channel4 interrupts */
void __attribute__((weak, alias("default_isr"))) ENET_IRQHandler(void);				/* 61; !< ENET interrupt */
void __attribute__((weak, alias("default_isr"))) ENET_WKUP_IRQHandler(void);		/* 62; !< ENET wakeup through EXTI line interrupt */
void __attribute__((weak, alias("default_isr"))) CAN1_TX_IRQHandler(void);			/* 63; !< CAN1 TX interrupt */
void __attribute__((weak, alias("default_isr"))) CAN1_RX0_IRQHandler(void);			/* 64; !< CAN1 RX0 interrupt */
void __attribute__((weak, alias("default_isr"))) CAN1_RX1_IRQHandler(void);			/* 65; !< CAN1 RX1 interrupt */
void __attribute__((weak, alias("default_isr"))) CAN1_EWMC_IRQHandler(void);		/* 66; !< CAN1 EWMC interrupt */
void __attribute__((weak, alias("default_isr"))) USBFS_IRQHandler(void);			/* 67; !< USBFS interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel5_IRQHandler(void);	/* 68; !< DMA1 channel5 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel6_IRQHandler(void);	/* 69; !< DMA1 channel6 interrupt */
void __attribute__((weak, alias("default_isr"))) DMA1_Channel7_IRQHandler(void);	/* 70; !< DMA1 channel7 interrupt */
void __attribute__((weak, alias("default_isr"))) USART5_IRQHandler(void);			/* 71; !< USART5 interrupt */
void __attribute__((weak, alias("default_isr"))) I2C2_EV_IRQHandler(void);			/* 72; !< I2C2 event interrupt */
void __attribute__((weak, alias("default_isr"))) I2C2_ER_IRQHandler(void);			/* 73; !< I2C2 error interrupt */
void __attribute__((weak, alias("default_isr"))) USBHS_EP1_Out_IRQHandler(void);	/* 74; !< USBHS endpoint 1 out interrupt */
void __attribute__((weak, alias("default_isr"))) USBHS_EP1_In_IRQHandler(void);		/* 75; !< USBHS endpoint 1 in interrupt */
void __attribute__((weak, alias("default_isr"))) USBHS_WKUP_IRQHandler(void);		/* 76; !< USBHS wakeup interrupt */
void __attribute__((weak, alias("default_isr"))) USBHS_IRQHandler(void);			/* 77; !< USBHS interrupt */
void __attribute__((weak, alias("default_isr"))) DCI_IRQHandler(void);				/* 78; !< DCI interrupt */
void __attribute__((weak, alias("default_isr"))) TRNG_IRQHandler(void);				/* 80; !< TRNG interrupt */
void __attribute__((weak, alias("default_isr"))) FPU_IRQHandler(void);				/* 81; !< FPU interrupt */
void __attribute__((weak, alias("default_isr"))) UART6_IRQHandler(void);			/* 82; !< UART6 interrupt */
void __attribute__((weak, alias("default_isr"))) UART7_IRQHandler(void);			/* 83; !< UART7 interrupt */
void __attribute__((weak, alias("default_isr"))) SPI3_IRQHandler(void);				/* 84; !< SPI3 interrupt */
void __attribute__((weak, alias("default_isr"))) SPI4_IRQHandler(void);				/* 85; !< SPI4 interrupt */
void __attribute__((weak, alias("default_isr"))) SPI5_IRQHandler(void);				/* 86; !< SPI5 interrupt */
void __attribute__((weak, alias("default_isr"))) TLI_IRQHandler(void);				/* 88; !< TLI interrupt */
void __attribute__((weak, alias("default_isr"))) TLI_ER_IRQHandler(void);			/* 89; !< TLI error interrupt */
void __attribute__((weak, alias("default_isr"))) IPA_IRQHandler(void);				/* 90; !< IPA interrupt */

/* we run on sram, but burned into flash, the startup code should run in flash
 * so Reset_Handler address should located at flash
 */

void *
__attribute__((section(".vector"), unused))
exception_table[] = {
	__ld_stack_top,		/* Top of Stack */
	RESET_HANDLER,		/* Reset Handler */
	NMI_Handler,		/* NMI Handler */
	HardFault_Handler,	/* Hard Fault Handler */
	MemManage_Handler,	/* MPU Fault Handler */
	BusFault_Handler,	/* Bus Fault Handler */
	UsageFault_Handler,	/* Usage Fault Handler */
	0,			/* Reserved */
	0,			/* Reserved */
	0,			/* Reserved */
	0,			/* Reserved */
	SVC_Handler,		/* SVCall Handler */
	DebugMon_Handler,	/* Debug Monitor Handler */
	0,			/* Reserved */
	PendSV_Handler,		/* PendSV Handler */
	SysTick_Handler,		/* SysTick Handler */
	WWDGT_IRQHandler,		/* 16:Window Watchdog Timer */
	LVD_IRQHandler,			/* 17:LVD through EXTI Line detect */
	TAMPER_IRQHandler,		/* 18:Tamper through EXTI Line detect */
	RTC_IRQHandler,			/* 19:RTC through EXTI Line */
	FMC_IRQHandler,			/* 20:FMC */
	RCU_CTC_IRQHandler,		/* 21:RCU and CTC */
	EXTI0_IRQHandler,		/* 22:EXTI Line 0 */
	EXTI1_IRQHandler,		/* 23:EXTI Line 1 */
	EXTI2_IRQHandler,		/* 24:EXTI Line 2 */
	EXTI3_IRQHandler,		/* 25:EXTI Line 3 */
	EXTI4_IRQHandler,		/* 26:EXTI Line 4 */
	DMA0_Channel0_IRQHandler,	/* 27:DMA0 Channel0 */
	DMA0_Channel1_IRQHandler,	/* 28:DMA0 Channel1 */
	DMA0_Channel2_IRQHandler,	/* 29:DMA0 Channel2 */
	DMA0_Channel3_IRQHandler,	/* 30:DMA0 Channel3 */
	DMA0_Channel4_IRQHandler,	/* 31:DMA0 Channel4 */
	DMA0_Channel5_IRQHandler,	/* 32:DMA0 Channel5 */
	DMA0_Channel6_IRQHandler,	/* 33:DMA0 Channel6 */
	ADC_IRQHandler,		/* 34:ADC0 and ADC1 */
	CAN0_TX_IRQHandler,	/* 35:USBD HP or CAN0 TX */
	CAN0_RX0_IRQHandler,	/* 36:USBD LP or CAN0 RX0 */
	CAN0_RX1_IRQHandler,		/* 37:CAN0 RX1 */
	CAN0_EWMC_IRQHandler,		/* 38:CAN0 EWMC */
	EXTI5_9_IRQHandler,		/* 39:EXTI5 to EXTI9 */
	TIMER0_BRK_IRQHandler,		/* 40:TIMER0 Break */
	TIMER0_UP_IRQHandler,		/* 41:TIMER0 Update */
	TIMER0_TRG_CMT_IRQHandler,	/* 42:TIMER0 Trigger and Commutation */
	TIMER0_Channel_IRQHandler,	/* 43:TIMER0 Channel Capture Compare */
	TIMER1_IRQHandler,		/* 44:TIMER1 */
	TIMER2_IRQHandler,		/* 45:TIMER2 */
	TIMER3_IRQHandler,		/* 46:TIMER3 */
	I2C0_EV_IRQHandler,		/* 47:I2C0 Event */
	I2C0_ER_IRQHandler,		/* 48:I2C0 Error */
	I2C1_EV_IRQHandler,		/* 49:I2C1 Event */
	I2C1_ER_IRQHandler,		/* 50:I2C1 Error */
	SPI0_IRQHandler,		/* 51:SPI0 */
	SPI1_I2S1ADD_IRQHandler,	/* 52:SPI1 or I2S1ADD */
	USART0_IRQHandler,		/* 53:USART0 */
	USART1_IRQHandler,		/* 54:USART1 */
	USART2_IRQHandler,		/* 55:USART2 */
	EXTI10_15_IRQHandler,		/* 56:EXTI10 to EXTI15 */
	RTC_Alarm_IRQHandler,		/* 57:RTC Alarm */
	USBD_WKUP_IRQHandler,		/* 58:USBD Wakeup */
	TIMER7_BRK_IRQHandler,		/* 59:TIMER7 Break */
	TIMER7_UP_IRQHandler,		/* 60:TIMER7 Update */
	TIMER7_TRG_CMT_IRQHandler,	/* 61:TIMER7 Trigger and Commutation */
	TIMER7_Channel_IRQHandler,	/* 62:TIMER7 Channel Capture Compare */
	DMA0_Channel7_IRQHandler,		/* 63:DMA0 channel7 */
	EXMC_IRQHandler,		/* 64:EXMC */
	SDIO_IRQHandler,		/* 65:SDIO */
	TIMER4_IRQHandler,		/* 66:TIMER4 */
	SPI2_I2S2ADD_IRQHandler,	/* 67:SPI2 or I2S2ADD */
	UART3_IRQHandler,		/* 68:UART3 */
	UART4_IRQHandler,		/* 69:UART4 */
	TIMER5_DAC_IRQHandler,		/* 70:TIMER5 or DAC */
	TIMER6_IRQHandler,		/* 71:TIMER6 */
	DMA1_Channel0_IRQHandler,	/* 72:DMA1 Channel0 */
	DMA1_Channel1_IRQHandler,	/* 73:DMA1 Channel1 */
	DMA1_Channel2_IRQHandler,	/* 74:DMA1 Channel2 */
	DMA1_Channel3_IRQHandler,	/* 75:DMA1 Channel3 */
	DMA1_Channel4_IRQHandler,	/* 76:DMA1 Channel4 */
	ENET_IRQHandler,			/* 77:ENET */
	ENET_WKUP_IRQHandler,		/* 78:ENET Wakeup */
	CAN1_TX_IRQHandler,		/* 79:CAN1 TX */
	CAN1_RX0_IRQHandler,		/* 80:CAN1 RX0 */
	CAN1_RX1_IRQHandler,		/* 81:CAN1 RX1 */
	CAN1_EWMC_IRQHandler,		/* 82:CAN1 EWMC */
	USBFS_IRQHandler,				/* 83:USBFS */
	DMA1_Channel5_IRQHandler,				/* 84:DMA1 Channel5 */
	DMA1_Channel6_IRQHandler,	/* 85:SHRTIMER IRQ2 */
	DMA1_Channel7_IRQHandler,	/* 86:SHRTIMER IRQ3 */
	USART5_IRQHandler,	/* 87:SHRTIMER IRQ4 */
	I2C2_EV_IRQHandler,	/* 88:SHRTIMER IRQ5 */
	I2C2_ER_IRQHandler,	/* 89:SHRTIMER IRQ6 */
	USBHS_EP1_Out_IRQHandler,				/* 90:USBHS Endpoint 1 Out */
	USBHS_EP1_In_IRQHandler,				/* 91:USBHS Endpoint 1 In */
	USBHS_WKUP_IRQHandler,	/* 92:SHRTIMER IRQ0 */
	USBHS_IRQHandler,	/* 93:SHRTIMER IRQ1 */
	DCI_IRQHandler,		/* 94:DCI */
	0,				/* Reserved */
	TRNG_IRQHandler,				/* 96:TRNG */
	FPU_IRQHandler,				/* 97:FPU */
	UART6_IRQHandler,				/* 98:UART6 */
	UART7_IRQHandler,		/* 99:UART7 */
	SPI3_IRQHandler,		/* 100:SPI3 */
	SPI4_IRQHandler,		/* 101:SPI4 */
	SPI5_IRQHandler,		/* 102:SPI5 */
	0,						/* Reserved */
	TLI_IRQHandler,		/* 104:USART5 Wakeup */
	TLI_ER_IRQHandler,	/* 105:USART5 Wakeup */
	IPA_IRQHandler,		/* 106:USART5 Wakeup */
};
