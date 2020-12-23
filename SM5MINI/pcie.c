#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <common.h>
#include <debug.h>
#include <chip.h>
#include <tick.h>

static volatile int is_chip_ready;
static int pcie_task;

void exti4_15_isr(void)
{
	if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN)) {
		if (is_chip_ready)
			chip_enable();
		debug("pcie e-reset raising edge\n");
	}
	else {
		chip_disable();
		tick_set_task_interval(pcie_task, 30);
		debug("pcie e-reset falling edge\n");
	}

	exti_reset_request(PCIE_RESET_EXTI);
}

static void pcie_process(void)
{
	/* one shot task */
	tick_set_task_interval(pcie_task, 0);
	is_chip_ready = true;
	if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
		chip_enable();
}

void pcie_init(void)
{
	is_chip_ready = 1;
	exti_select_source(PCIE_RESET_EXTI, PCIE_RESET_PORT);
	exti_set_trigger(PCIE_RESET_EXTI, EXTI_TRIGGER_BOTH);
	exti_enable_request(PCIE_RESET_EXTI);
	nvic_enable_irq(PCIE_RESET_NVIC);

	if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
		chip_enable();

	pcie_task = tick_register_task(pcie_process, 0);
}

void pcie_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	nvic_clear_pending_irq(PCIE_RESET_NVIC);
	exti_disable_request(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	is_chip_ready = 0;
}

