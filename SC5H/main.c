/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <stdstub.h>
#include <tick.h>
#include <system.h>
#include <power.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <info.h>
#include <upgrade.h>
#include <mcu.h>
#include <debug.h>
#include <pin.h>
#include <eeprom.h>

static inline void led_off(void);

static inline void led_init(void)
{
	gpio_mode_setup(LED_PORT,
			GPIO_MODE_OUTPUT,
			GPIO_PUPD_PULLUP,
			LED_PIN);
	led_off();
}

static inline void led_on(void)
{
	gpio_set(LED_PORT, LED_PIN);
}

static inline void led_off(void)
{
	gpio_clear(LED_PORT, LED_PIN);
}

static inline void sys_reset_set(int state);

static inline void pcie_reset_init(void)
{
	gpio_mode_setup(PCIE_RESET_PORT,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			PCIE_RESET_PIN);
}

static inline int pcie_reset_state(void)
{
	return gpio_get(PCIE_RESET_PORT, PCIE_RESET_PIN) ? 1 : 0;
}

static inline void sys_reset_init(void)
{
	gpio_mode_setup(SYS_RESET_PORT,
			GPIO_MODE_OUTPUT,
			GPIO_PUPD_PULLDOWN,
			SYS_RESET_PIN);
	sys_reset_set(0);
}

static inline void sys_reset_set(int state)
{
	if (state)
		gpio_set(SYS_RESET_PORT, SYS_RESET_PIN);
	else
		gpio_clear(SYS_RESET_PORT, SYS_RESET_PIN);
}

void pcie_reset_poll(void)
{
	if (pcie_reset_state() == 0) {
		sys_reset_set(0);
		mdelay(30);
		while (pcie_reset_state() == 0)
			;
		sys_reset_set(1);
	}
}

struct i2c_slave_ctx i2c1_slave_ctx;

int main(void)
{
	stage = setup_stage();
	if (stage == RUN_STAGE_LOADER && check_app() == 0)
		app_start();

	system_init();
	tick_init();

	/* enable all gpio power and clock */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOH);
	/* i2c1, i2c2 */
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_I2C2);
	i2c_master_init(I2C1);
	i2c_master_init(I2C2);
	/* enable i2c1 interrupt */
	nvic_enable_irq(NVIC_I2C1_IRQ);
	/* enable uart1, uart1 tx,rx use PA2 and PA3 */
	/* PA2 -- TX, PA3 -- RX */
	std_stub_init();

	debug("BITMAIN SOPHONE SC5H -- %s\n", VERSION);

	i2c1_slave_ctx.id = 1;
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
			0x17, -1, 0);
	mcu_init();
	eeprom_init();
	i2c_slave_start(&i2c1_slave_ctx);

	/* mux i2c2 pin to PB10 -- SCL, PB11 -- SDA */
	/* mux i2c1 pin to PB8 -- SCL, PB9 -- SDA */
	gpio_set(GPIOB, GPIO8 | GPIO9 | GPIO10 | GPIO11);
	gpio_set_af(GPIOB, GPIO_AF6, GPIO10 | GPIO11);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_VERYHIGH,
			GPIO10 | GPIO11 | GPIO8 | GPIO9);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO10 | GPIO11 | GPIO8 | GPIO9);

	/* init user led */
	led_init();
	/* pcie reset received from pc, valid low */
	pcie_reset_init();
	/* sys reset to chip, valid high */
	sys_reset_init();
	power_init();
	power_on();
	led_on();

	while (1) {
		pcie_reset_poll();
		mcu_cmd_process();
	}

	return 0;
}
