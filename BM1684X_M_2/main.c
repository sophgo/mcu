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
#include <mpm3695.h>
#include <loop.h>
#include <keyboard.h>
#include <eeprom.h>
#include <wdt.h>
#include <pcie.h>
#include <console.h>
#include <ct7451.h>
#include <mon_print.h>
#include <mcu-e2prom.h>
#include <pwm.h>

static struct i2c_slave_ctx i2c1_slave_ctx;
static struct i2c_slave_ctx i2c2_slave_ctx;

int main(void)
{
	clock_init();
	system_init();

	timer_mdelay(100);

	debug("\nSOPHONE BM1684X_M_2\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER && check_app() == 0)
		app_start();
#endif

	led_init();
	led_set_frequency(0);
	i2c_master_init(I2C1);
	i2c_master_init(I2C2);
	debug("i2c init done\n");
	mp5475_init();
	debug("mp5475_init done\n");

	timer_udelay(50*1000);
	power_init();
	debug("power_init done\n");
	timer_udelay(1000);
	// mp5475_init();
	debug("enter power\n");
	power_on();
	debug("power done\n");
	chip_init();
	debug("%s %s working at ",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application");
	if (get_work_mode() == WORK_MODE_SOC)
		debug("soc mode\n");
	else if (get_work_mode() == WORK_MODE_PCIE)
		debug("pcie mode\n");
	else if (get_work_mode() == WORK_MODE_MIXED)
		debug("mix mode\n");
	else
		debug("unkown mode\n");

	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
		       I2C1_OA1, I2C1_OA2, I2C1_OA2_MASK);

	mon_init();
	//mon_print_init();

	set_board_type(BM1684X_M_2);

	mcu_init(&i2c1_slave_ctx);
	mcu_eeprom_init(&i2c1_slave_ctx);
	wdt_init(&i2c1_slave_ctx);

	ct7451_init(&i2c1_slave_ctx);

	// if (get_work_mode() == WORK_MODE_SOC) {
	// 	sm7_init();
	// }

	/* start i2c slaves */
	i2c_slave_start(&i2c1_slave_ctx);
	debug("\n\ni2c_slave_start done\n");
	if (get_work_mode() == WORK_MODE_SOC){
		chip_enable_and_ddr_pg();
		debug("\n\nchip_enable_and_ddr_pg done\n");
	}
	else
		pcie_init();

	// debug("\n\nenter init_mpm3695\n");
	init_mpm3695();
	init_pwm();
	console_add();
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
