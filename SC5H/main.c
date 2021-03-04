/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include <system.h>
#include <power.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <upgrade.h>
#include <mcu.h>
#include <debug.h>
#include <eeprom.h>
#include <led.h>
#include <chip.h>

struct i2c_slave_ctx i2c1_slave_ctx;

int main(void)
{
	int uart_recv;

	clock_init();

	if (get_stage() == RUN_STAGE_LOADER) {
		if (check_app() == 0)
			app_start();
	}

	system_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);

	debug("BITMAIN SOPHONE SC5H\r\n");

	i2c1_slave_ctx.id = 1;
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
			0x17, 0x6a, 0);
	mcu_init();
	eeprom_init();
	i2c_slave_start(&i2c1_slave_ctx);

	power_on();
	led_on();

	/* start listening pcie ep reset */
	chip_listen();

	printf("\r\npress \'u\' to start uart upgrade\r\n");

	while (1) {
		chip_update();
		mcu_cmd_process();
		uart_recv = uart_read();
		if (uart_recv == 'u') {
			printf("\r\nentering uart upgrader\r\n");
			uart_upgrade_start();
		}
	}

	return 0;
}
