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
#include <project.h>
#include <power.h>
#include <loop.h>
#include <eeprom.h>
#include <wdt.h>
#include <tmp451.h>
#include <tca6416a.h>
#include <keyboard.h>
#include <wol.h>

struct i2c_slave_ctx i2c1_slave_ctx;

int main(void)
{
	clock_init();
	system_init();

	debug("\nBITMAIN SOPHONE SE5LITE\n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER && check_app() == 0)
		app_start();
#endif

	led_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);

	power_init();
	chip_init();

	debug("%s %s\n",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application");

	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
		       I2C1_OA1, I2C1_OA2, I2C1_OA2_MASK);

	set_board_type(SE5LITE);

	wol_init();
	mon_init();

	/* wait reset key high */
	timer_mdelay(10);

	mcu_init(&i2c1_slave_ctx);
	tca6416a_init(&i2c1_slave_ctx);
	tmp451_init(&i2c1_slave_ctx);
	eeprom_init(&i2c1_slave_ctx);
	wdt_init(&i2c1_slave_ctx);
	kbd_init(&i2c1_slave_ctx);

	/* start i2c slaves */
	i2c_slave_start(&i2c1_slave_ctx);

	chip_power_on_enable();

	/* never return */
	loop_start();

	return 0;
}

void i2c1_isr(void)
{
	i2c_slave_isr(&i2c1_slave_ctx);
}
