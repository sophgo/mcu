#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <common.h>
#include <debug.h>
#include <timer.h>
#include <chip.h>
#include <loop.h>

static volatile int is_chip_ready;

void exti2_3_isr(void)
{
	debug("pcie ep reset falling edge\r\n");
	chip_enable();
	timer_start(30000);
	is_chip_ready = 0;
	exti_reset_request(PCIE_RESET_EXTI);
}

static void pcie_process(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	if (is_chip_ready) {
		if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
			chip_enable();
	} else {
		is_chip_ready = timer_is_timeout();
	}
	nvic_enable_irq(PCIE_RESET_NVIC);
}

void pcie_init(void)
{
	exti_select_source(PCIE_RESET_EXTI, PCIE_RESET_PORT);
	exti_set_trigger(PCIE_RESET_EXTI, EXTI_TRIGGER_FALLING);
	exti_enable_request(PCIE_RESET_EXTI);
	is_chip_ready = 1;
	loop_add(pcie_process);
}

void pcie_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	nvic_clear_pending_irq(PCIE_RESET_NVIC);
	exti_disable_request(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	timer_stop();
	is_chip_ready = 0;
}

