/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <system.h>
#include <i2c_master.h>
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <pin.h>
#include <timer.h>
#include <mon.h>
#include <common.h>
#include <upgrade.h>
#include <mcu.h>
#include <pic.h>
#include <tca6416a.h>
#include <project.h>
#include <power.h>
#include <se5.h>
#include <se6.h>
#include <sm7.h>
#include <loop.h>
#include <keyboard.h>
#include <eeprom.h>
#include <wdt.h>
#include <pcie.h>
#include <slt.h>
#include <tmp451.h>
#include <rst_key.h>
#include <mon_print.h>
#include <at24c128c-e2prom.h>
static struct i2c_slave_ctx i2c1_slave_ctx;
static struct i2c_slave_ctx i2c2_slave_ctx;

int main(void)
{
	clock_init();
	system_init();

	debug("\nBITMAIN SOPHONE SM7MINI\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER && check_app() == 0)
		app_start();
#endif

	led_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);

	/* check if i am se8 crtl board */
	if (is_se6ctrl_board())
		set_board_type(SM7MSE6M);
	/* check if i am in test board and if we need enter test mode */
	else if (is_test_mode()) {
		mcu_set_test_mode(true);

		/* convert MCU_INT from input to output */
		gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
		gpio_set_output_options(MCU_INT_PORT, GPIO_OTYPE_PP,
					GPIO_OSPEED_VERYHIGH, MCU_INT_PIN);
		gpio_mode_setup(MCU_INT_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
				MCU_INT_PIN);

		set_board_type(SM7M);

		i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2_BASE,
			    	I2C2_OA1, I2C2_OA2, I2C2_OA2_MASK);
		mcu_test_init(&i2c2_slave_ctx);
		nvic_enable_irq(NVIC_I2C2_IRQ);
		i2c_slave_start(&i2c2_slave_ctx);

		led_set_frequency(1000);
		while (1) {
			mcu_process();
			if (!mcu_get_test_mode())
				break;
		}

		nvic_disable_irq(NVIC_I2C2_IRQ);
		i2c_slave_stop(&i2c2_slave_ctx);
	} else {
		set_board_type(SM7M);
	}

	/* reset MCU_INT */
	gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
	gpio_mode_setup(MCU_INT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
			MCU_INT_PIN);

	tca6416a_probe();
	pic_probe();

	timer_udelay(50*1000);
	power_init();
	gpio_set(EN_12V_SYS_PORT, EN_12V_SYS_PIN);
	timer_udelay(1000);
	mp5475_init();
	power_on();
	chip_init();

	debug("%s %s working at ",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application");
	if (get_work_mode() == WORK_MODE_SOC) {
		sm7_init();
		debug("soc mode\n");
	}
	else if (get_work_mode() == WORK_MODE_PCIE)
		debug("pcie mode\n");
	else if (get_work_mode() == WORK_MODE_MIXED)
		debug("mix mode\n");
	else
		debug("unkown mode\n");

	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
		       I2C1_OA1, I2C1_OA2, I2C1_OA2_MASK);

	/* but chip reset still be asserted */
	if (get_work_mode() == WORK_MODE_SOC) {
		if (get_board_type() == SM7MSE6M) {
			se6_init();
			if (get_eeprom_type() == AT24C01D)
				at24c01d_init(&i2c1_slave_ctx);
			else
				at24c128c_init(&i2c1_slave_ctx);
		}
	}

	mon_init();
	mon_print_init();

	mcu_init(&i2c1_slave_ctx);
	eeprom_init(&i2c1_slave_ctx);
	wdt_init(&i2c1_slave_ctx);
	slt_init(&i2c1_slave_ctx);

	if (tca6416a_available())
		tca6416a_init(&i2c1_slave_ctx);

	if (get_board_type() == SM7MSE6M)
		kbd_init(&i2c1_slave_ctx);
	if (pic_available())
		pic_init(&i2c1_slave_ctx);

	tmp451_init(&i2c1_slave_ctx);

	/* start i2c slaves */
	i2c_slave_start(&i2c1_slave_ctx);

	if (get_work_mode() == WORK_MODE_SOC)
		chip_enable();
	else
		pcie_init();

	//rst_key_init();

	/* never return */
	loop_start();

	return 0;
}

void i2c1_isr(void)
{
	i2c_slave_isr(&i2c1_slave_ctx);
}

void i2c2_isr(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}
