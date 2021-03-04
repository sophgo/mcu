/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include <system.h>
#include <board_power.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <isl68224.h>
#include <tca9554.h>
#include <pin.h>
#include <timer.h>
#include <mon.h>
#include <tca9548a.h>
#include <dbgi2c.h>
#include <console.h>
#include <upgrade.h>

struct i2c_slave_ctx i2c3_slave_ctx;
#define I2C3_OA1	0x60
#define I2C3_OA2	0x60
/* mask 3 lsb, aka 0x60, 0x61 ... 0x67 */
#define I2C3_OA2_MASK	0x03

int main(void)
{
	clock_init();

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER) {
		if (check_app() == 0)
			app_start();
	}
#endif

	system_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);
	i2c_master_init(I2C3);

	debug("BITMAIN SOPHONE SC5PRO\n");

	tca9548a_init();

	/* enable power supply of pcie switch */
	gpio_set(P0V9_E_EN_PORT, P0V9_E_EN_PIN);

	timer_mdelay(1);
	/* release reset of pcie switch */
	gpio_set(MCU_8533_RSTB_PORT, MCU_8533_RSTB_PIN);

	tca9554_init();
	mp5475_init();
	board_power_init();

	chip_init();
	mon_init();

	i2c_slave_init(&i2c3_slave_ctx, (void *)I2C3_BASE,
		       I2C3_OA1, I2C3_OA2, I2C3_OA2_MASK);

	dbgi2c_init(&i2c3_slave_ctx);
	console_init();

	i2c_slave_start(&i2c3_slave_ctx);

	while (1) {
		chip_update();
		mon_process();
		console_poll();
	}

	return 0;
}

/* i2c3 interrupt handler */

void i2c3_isr(void)
{
	i2c_slave_isr(&i2c3_slave_ctx);
}

