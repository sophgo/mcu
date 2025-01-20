#include <stdlib.h>
#include <libopencm3/stm32/exti.h>
#include <pin.h>
#include <pcie.h>
#include <chip.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <debug.h>
#include <wdt.h>
#include <rst_key.h>
#include <timer.h>

#define DEBOUNCE_COUNT	512

// void rst_key_init(void)
// {
// 	exti_select_source(RESET_KEY_EXTI, RESET_KEY_PORT);
// 	exti_set_trigger(RESET_KEY_EXTI, EXTI_TRIGGER_RISING);
// 	exti_enable_request(RESET_KEY_EXTI);
// 	nvic_enable_irq(RST_KEY_NVIC);
// }

// void rst_key_isr(void)
// {
//     unsigned int i;
// 	unsigned int hi = 0;
// 	unsigned int lo = 0;

// 	for (i = 0; i < DEBOUNCE_COUNT; ++i) {
// 		if (gpio_get(RESET_KEY_PORT, RESET_KEY_PIN))
// 			++hi;
// 		else
// 			++lo;
// 	}

// 	if (hi > lo) {
// 		debug("rst key gpio level high\n");
// 		asm volatile ("MSR faultmask, %0" : : "r" (1) : "memory"); //disable all irq

// 		// scb_reset_system();
// 		asm volatile ("dsb 0xF":::"memory");		/* Ensure all outstanding memory accesses included
// 							buffered write are completed before reset */
//   		MMIO32(0xE000ED0C) = ((0x5FAUL << 16U) | (1U << 2U));
// 		asm volatile ("dsb 0xF":::"memory");		/* Ensure completion of memory access */

//   		for (;;) {			/* wait until reset */
// 			asm volatile ("nop");
// 		}
// 	} 
// }
