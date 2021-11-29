#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <common.h>
#include <debug.h>
#include <chip.h>
#include <tick.h>
#include <wdt.h>
#include <stdbool.h>

static volatile int is_chip_ready;
static int pcie_task;

void EXTI3_IRQHandler(void)
{
	if (!exti_interrupt_flag_get(PCIE_RESET_EXTI))
		return;

	if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN)) {
		if (is_chip_ready)
			chip_enable();
		debug("pcie e-reset raising edge\n");
	}
	else {
		chip_disable();
		tick_set_task_interval(pcie_task, 30);
		wdt_reset();
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

	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_3);
	exti_init(PCIE_RESET_EXTI, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	exti_interrupt_flag_clear(PCIE_RESET_EXTI);

	nvic_enable_irq(PCIE_RESET_NVIC);

	if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
		chip_enable();

	pcie_task = tick_register_task(pcie_process, 0);
}

void pcie_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	exti_interrupt_disable(PCIE_RESET_EXTI);
	exti_interrupt_flag_clear(PCIE_RESET_EXTI);

	is_chip_ready = 0;
}

