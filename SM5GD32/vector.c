
extern char __ld_stack_top[0];
extern void __ld_reset_handler(void);

/* real address to thumb mode */
#define RESET_HANDLER	((void *)((unsigned long)__ld_reset_handler + 1))

void default_isr(void)
{
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
void __attribute__((weak, alias("default_isr"))) WWDGT_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) LVD_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TAMPER_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) RTC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) FMC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) RCU_CTC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI3_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel3_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel5_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA0_Channel6_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) ADC0_1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USBD_HP_CAN0_TX_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USBD_LP_CAN0_RX0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN0_RX1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN0_EWMC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI5_9_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER0_BRK_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER0_UP_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER0_TRG_CMT_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER0_Channel_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER3_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C0_EV_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C0_ER_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C1_EV_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C1_ER_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SPI0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SPI1_I2S1ADD_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USART0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USART1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USART2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXTI10_15_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) RTC_Alarm_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USBD_WKUP_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER7_BRK_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER7_UP_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER7_TRG_CMT_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER7_Channel_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) ADC2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) EXMC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SDIO_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SPI2_I2S2ADD_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) UART3_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) UART4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER5_DAC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) TIMER6_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA1_Channel0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA1_Channel1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA1_Channel2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) DMA1_Channel3_4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN1_TX_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN1_RX0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN1_RX1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) CAN1_EWMC_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ2_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ3_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ4_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ5_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ6_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ0_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) SHRTIMER_IRQ1_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C2_EV_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C2_ER_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USART5_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) I2C2_WKUP_IRQHandler(void);
void __attribute__((weak, alias("default_isr"))) USART5_WKUP_IRQHandler(void);

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
	ADC0_1_IRQHandler,		/* 34:ADC0 and ADC1 */
	USBD_HP_CAN0_TX_IRQHandler,	/* 35:USBD HP or CAN0 TX */
	USBD_LP_CAN0_RX0_IRQHandler,	/* 36:USBD LP or CAN0 RX0 */
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
	ADC2_IRQHandler,		/* 63:ADC2 */
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
	DMA1_Channel3_4_IRQHandler,	/* 75:DMA1 Channel3 and Channel4 */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	CAN1_TX_IRQHandler,		/* 79:CAN1 TX */
	CAN1_RX0_IRQHandler,		/* 80:CAN1 RX0 */
	CAN1_RX1_IRQHandler,		/* 81:CAN1 RX1 */
	CAN1_EWMC_IRQHandler,		/* 82:CAN1 EWMC */
	0,				/* Reserved */
	0,				/* Reserved */
	SHRTIMER_IRQ2_IRQHandler,	/* 85:SHRTIMER IRQ2 */
	SHRTIMER_IRQ3_IRQHandler,	/* 86:SHRTIMER IRQ3 */
	SHRTIMER_IRQ4_IRQHandler,	/* 87:SHRTIMER IRQ4 */
	SHRTIMER_IRQ5_IRQHandler,	/* 88:SHRTIMER IRQ5 */
	SHRTIMER_IRQ6_IRQHandler,	/* 89:SHRTIMER IRQ6 */
	0,				/* Reserved */
	0,				/* Reserved */
	SHRTIMER_IRQ0_IRQHandler,	/* 92:SHRTIMER IRQ0 */
	SHRTIMER_IRQ1_IRQHandler,	/* 93:SHRTIMER IRQ1 */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	0,				/* Reserved */
	I2C2_EV_IRQHandler,		/* 98:I2C2 Event */
	I2C2_ER_IRQHandler,		/* 99:I2C2 Error */
	USART5_IRQHandler,		/* 100:USART5 */
	I2C2_WKUP_IRQHandler,		/* 101:I2C2 Wakeup */
	USART5_WKUP_IRQHandler,		/* 102:USART5 Wakeup */
	0,				/* Reserved */
};
