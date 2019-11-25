#define SCB_BASE	(0xe000ed00)
#define SCB_VTOR	(SCB_BASE + 8)

	.syntax unified
	.cpu cortex-m0plus
	.fpu softvfp
	.thumb

	.section .loader, "ax"
loader_start:
	# get loader running address
	mov	r3, pc
	subs	r3, r3, 4
	/* save r0, parameters, donnot modify r4 untill main */
	mov	r4, r0
	
	mov	r0, r3

	ldr	r1, =__ld_vector_offset
	/* vector load address */
	add	r0, r0, r1
	/* get program running address */
	ldr	r1, =__ld_program_start
	/* now r0 is program lma address, copy source */
	/* now r1 is program vma address, copy destination */
	/* get program end */
	ldr	r2, =__ld_program_end
	

copy_program_start:
	cmp	r2, r1
	beq	copy_program_end
	ldr	r3, [r0]
	str	r3, [r1]
	adds	r0, r0, #4
	adds	r1, r1, #4
	b	copy_program_start
copy_program_end:

	ldr	r0, =__ld_bss_start
	ldr	r1, =__ld_bss_end
	ldr	r2, =0
zero_bss_start:
	cmp	r0, r1
	beq	zero_bss_end
	str	r2, [r0]
	adds	r0, #4
	b	zero_bss_start
zero_bss_end:

	/* b	zero_bss_end */
	
	/* reconfig scb->vtor */
	ldr	r0, =SCB_VTOR
	ldr	r1, =reset
	str	r1, [r0]
	dsb
	isb

	/* jump to start */
	ldr	r0, =upgrader_start
	adds	r0, r0, 1
	bx	r0
	
loader_end:
	b	loader_end

/* r0 load address */
/* r1 stack address */

	.section .vector, "ax"
	.global reset
reset:
	/* make sure the upper instructions occupy 4bytes */
	.word	__ld_ram_end
	.word	upgrader_start
	.word	isr_nmi				/* nmi */
	.word	isr_hardfault		/* hard fault */
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	isr_svc
	.word	0
	.word	0
	.word	isr_pendsv
	.word	isr_systick		/* System Timer	*/
	.word	isr_wwdg		/* Window WatchDog */
	.word	isr_pvd			/* PVD through EXTI Line detection */
	.word	isr_rtc			/* RTC through the EXTI line */
	.word	isr_flash		/* FLASH */
	.word	isr_rcc			/* RCC */
	.word	isr_exti0_1		/* EXTI Line 0 and 1 */
	.word	isr_exti2_3		/* EXTI Line 2 and 3 */
	.word	isr_exti4_15		/* EXTI Line 4 to 15 */
	.word	0			/* Reserved */
	.word	isr_dma1chan1		/* DMA1 Channel 1 */
	.word	isr_dma1chan2_3		/* DMA1 Channel 2 and Channel 3 */
	.word	isr_dma1chan4_7		/* DMA1 Channel 4, Channel 5, Channel 6 and Channel 7 */
	.word	isr_acd1		/* ADC1, COMP1 and COMP2 */
	.word	isr_lptim1		/* LPTIM1 */
	.word	isr_usart4_5		/* USART4 and USART 5 */
	.word	isr_tim2		/* TIM2 */
	.word	isr_tim3		/* TIM3 */
	.word	isr_tim6		/* TIM6 and DAC */
	.word	isr_tim7 		/* TIM7 */
	.word	0			/* Reserved */
	.word	isr_tim21		/* TIM21 */
	.word	isr_i2c3		/* I2C3 */
	.word	isr_tim22		/* TIM22 */
	.word	isr_i2c1		/* I2C1 */
	.word	isr_i2c2		/* I2C2 */
	.word	isr_spi1		/* SPI1 */
	.word	isr_spi2		/* SPI2 */
	.word	isr_usart1		/* USART1 */
	.word	isr_usart2		/* USART2 */
	.word	isr_lpuart1		/* LPUART1 */
	.word	0			/* Reserved */
	.word	0			/* Reserved */

	.section .text, "ax"
	.global upgrader_start
upgrader_start:
	ldr	r0, =__ld_ram_end
	mov	sp, r0
	/* setup project */
	ldr	r0, =project
	str	r4, [r0]
	bl	main
end:
	b	end
