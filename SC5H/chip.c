#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>

static volatile int is_chip_ready;

void chip_init(void)
{
	gpio_mode_setup(PCIE_RESET_PORT,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			PCIE_RESET_PIN);
	gpio_mode_setup(SYS_RESET_PORT,
			GPIO_MODE_OUTPUT,
			GPIO_PUPD_PULLDOWN,
			SYS_RESET_PIN);
	gpio_clear(SYS_RESET_PORT, SYS_RESET_PIN);

	/* enable PCIE RESET PIN interrupt */
	rcc_periph_clock_enable(RCC_SYSCFG);
	nvic_enable_irq(PCIE_RESET_NVIC);
	exti_select_source(PCIE_RESET_EXTI, PCIE_RESET_PORT);
	exti_set_trigger(PCIE_RESET_EXTI, EXTI_TRIGGER_FALLING);
	exti_enable_request(PCIE_RESET_EXTI);

	is_chip_ready = 1;
}

void chip_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	nvic_clear_pending_irq(PCIE_RESET_NVIC);
	exti_disable_request(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	timer_stop();
	is_chip_ready = 0;
}

void chip_update(void)
{
	if (is_chip_ready) {
		if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
			gpio_set(SYS_RESET_PORT, SYS_RESET_PIN);
	} else {
		is_chip_ready = timer_is_timeout();
	}
}

void exti4_15_isr(void)
{
	debug("pcie ep reset falling edge\r\n");
	gpio_clear(SYS_RESET_PORT, SYS_RESET_PIN);
	timer_start(30000);
	is_chip_ready = 0;
	exti_reset_request(PCIE_RESET_EXTI);
}

