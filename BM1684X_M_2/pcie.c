#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <common.h>
#include <debug.h>
#include <chip.h>
#include <tick.h>
#include <wdt.h>
#include <rst_key.h>
#include <pic.h>

static volatile int is_chip_ready;
static int pcie_task;

/* practical value, 2048 cycle used 1.2ms */
#define DEBOUNCE_COUNT	512

void pcie_reset_isr(void)
{
	unsigned int i;
	unsigned int hi = 0;
	unsigned int lo = 0;

	for (i = 0; i < DEBOUNCE_COUNT; ++i) {
		if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
			++hi;
		else
			++lo;
	}

	if (hi > lo) {
		if (is_chip_ready){
			// gpio_set(PCIEE_RST_X_MCU_PORT,PCIEE_RST_X_MCU_PIN);
			chip_enable(); 
		}
		debug("pcie e-reset raising edge\n");
	} else {
		is_chip_ready = false;
		chip_disable();
		// gpio_clear(PCIEE_RST_X_MCU_PORT,PCIEE_RST_X_MCU_PIN);
		tick_set_task_interval(pcie_task, 30);
		wdt_reset();
		debug("pcie e-reset falling edge\n");
	}

	exti_reset_request(PCIE_RESET_EXTI);
}

void exti4_15_isr(void)
{
	if (exti_get_flag_status(PCIE_RESET_EXTI))
		pcie_reset_isr();

	// if (exti_get_flag_status(RESET_KEY_EXTI))
	// 	rst_key_isr();
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
	is_chip_ready = true;
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

void set_chip_states(int state)
{
	is_chip_ready = state;
}
