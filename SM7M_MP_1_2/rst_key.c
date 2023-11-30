#include <stdlib.h>
#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <chip.h>
#include <debug.h>
#include <wdt.h>
#include <rst_key.h>

#define DEBOUNCE_COUNT	512

// void EXTI10_15_IRQHandler(void)
// {
// 	if (!exti_interrupt_flag_get(RESET_KEY_EXTI))
// 		return;

//     	unsigned int i;
// 	unsigned int hi = 0;
// 	unsigned int lo = 0;

// 	for (i = 0; i < DEBOUNCE_COUNT; ++i) {
// 		if (gpio_get(RESET_KEY_PORT, RESET_KEY_PIN))
// 			++hi;
// 		else
// 			++lo;
// 	}

// 	if (hi > lo) {
// 		chip_enable();
// 		debug("rst key gpio level high\n");
// 	} else {
// 		chip_disable();
// 		wdt_reset();
// 	}

// 	exti_reset_request(RESET_KEY_EXTI);
// }


// void rst_key_init(void)
// {
// 	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_12);
// 	exti_init(RESET_KEY_EXTI, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
// 	exti_interrupt_flag_clear(RESET_KEY_EXTI);
// 	nvic_enable_irq(EXTI10_15_IRQn);
// }
