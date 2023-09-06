#include <stdlib.h>
#include <libopencm3/stm32/exti.h>
#include <pin.h>
#include <pcie.h>
#include <chip.h>
#include <libopencm3/cm3/nvic.h>
#include <debug.h>
#include <wdt.h>
#include <rst_key.h>

#define DEBOUNCE_COUNT	512

void rst_key_init(void)
{
	exti_select_source(RESET_KEY_EXTI, RESET_KEY_PORT);
	exti_set_trigger(RESET_KEY_EXTI, EXTI_TRIGGER_BOTH);
	exti_enable_request(RESET_KEY_EXTI);
	nvic_enable_irq(RST_KEY_NVIC);
}

void rst_key_isr(void)
{
    	unsigned int i;
	unsigned int hi = 0;
	unsigned int lo = 0;

	for (i = 0; i < DEBOUNCE_COUNT; ++i) {
		if (gpio_get(RESET_KEY_PORT, RESET_KEY_PIN))
			++hi;
		else
			++lo;
	}

	if (hi > lo) {
		set_chip_states(true);
		chip_enable();
		debug("rst key gpio level high\n");
	} else {
		set_chip_states(false);
		chip_disable();
		wdt_reset();
		debug("rst key gpio level low\n");
	}

	exti_reset_request(RESET_KEY_EXTI);
}
