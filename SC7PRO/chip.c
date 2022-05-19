#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>
#include <tca9554.h>
#include <stdio.h>
#include <common.h>


/* wait debug i2c ready */
#define CHIP_BOOT_TIME	1500


static volatile int is_chip_ready;
static volatile int is_chip_enabled;

int chip_is_enabled(void)
{
	if (is_chip_enabled)
		return tick64_get() > CHIP_BOOT_TIME;
	else
		return false;
}

void chip_init(void)
{
	gpio_exti_source_select(PCIE_RESET_PORT, PCIE_RESET_EXTI);
	exti_init(PCIE_RESET_EXTI, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_flag_clear(PCIE_RESET_EXTI);
	exti_interrupt_enable(PCIE_RESET_EXTI);
	is_chip_ready = 1;
}

void chip_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	exti_interrupt_disable(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	timer_stop();
	is_chip_ready = 0;
}

#define BN_SYS_RST_ENABLE(n)					\
	do {							\
		gpio_set(B ## n ## _SYS_RST_N_PORT,		\
			 B ## n ## _SYS_RST_N_PIN);		\
	} while (0)


#define BN_SYS_RST_DISABLE(n)					\
	do {							\
		gpio_clear(B ## n ## _SYS_RST_N_PORT,		\
			   B ## n ## _SYS_RST_N_PIN);		\
	} while (0)

static inline void sys_rst_enable(void)
{
	BN_SYS_RST_ENABLE(1);
	BN_SYS_RST_ENABLE(2);
	BN_SYS_RST_ENABLE(3);
	BN_SYS_RST_ENABLE(4);
	BN_SYS_RST_ENABLE(5);
	BN_SYS_RST_ENABLE(6);
	BN_SYS_RST_ENABLE(7);
	BN_SYS_RST_ENABLE(8);
	/* count from 0 when first time enabled */
	
	if (!is_chip_enabled)
		tick64_set(0);

	is_chip_enabled = true;
}

static inline void sys_rst_disable(void)
{
	is_chip_enabled = false;

	BN_SYS_RST_DISABLE(1);
	BN_SYS_RST_DISABLE(2);
	BN_SYS_RST_DISABLE(3);
	BN_SYS_RST_DISABLE(4);
	BN_SYS_RST_DISABLE(5);
	BN_SYS_RST_DISABLE(6);
	BN_SYS_RST_DISABLE(7);
	BN_SYS_RST_DISABLE(8);
}

void chip_update(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	if (is_chip_ready) {
		if (gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN))
			sys_rst_enable();
	} else {
		is_chip_ready = timer_is_timeout();
	}
	nvic_enable_irq(PCIE_RESET_NVIC);

}

void exti2_3_isr(void)
{
	debug("pcie ep reset falling edge\n");
	sys_rst_disable();
	timer_start(30000);
	is_chip_ready = 0;
	exti_reset_request(PCIE_RESET_EXTI);
}

