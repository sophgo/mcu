#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>
#include <tca9554.h>

#define PCIE_RESET_PORT	PCIE_RST_X_18_PORT
#define PCIE_RESET_PIN	PCIE_RST_X_18_PIN
#define PCIE_RESET_EXTI	PCIE_RST_X_18_EXTI
#define PCIE_RESET_NVIC	NVIC_EXTI2_3_IRQ

static volatile int is_chip_ready;

void chip_init(void)
{
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
}

static inline void sys_rst_disable(void)
{
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
	debug("pcie ep reset falling edge\r\n");
	sys_rst_disable();
	timer_start(30000);
	is_chip_ready = 0;
	exti_reset_request(PCIE_RESET_EXTI);
}

