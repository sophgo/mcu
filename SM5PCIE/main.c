/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <stdio.h>
#include <tick.h>
#include <system.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <info.h>
#include <mcu.h>
#include <debug.h>

struct i2c_slave_ctx i2c1_slave_ctx;

int main(void)
{
	system_init();

	debug("BITMAIN SOPHONE SM5 PCIE BOARD -- %s\n", VERSION);

	i2c1_slave_ctx.id = 1;
	i2c_master_init(I2C1);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE);
	i2c_slave_start(&i2c1_slave_ctx);

	mcu_init();

	while (1) {
	}

	return 0;
}
