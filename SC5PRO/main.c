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
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <tca9554.h>
#include <pin.h>
#include <timer.h>

int main(void)
{
	clock_init();
	system_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);

	debug("BITMAIN SOPHONE SC5H -- %s\r\n", VERSION);

	/* enable power supply of pcie switch */
	gpio_set(P0V9_E_EN_PORT, P0V9_E_EN_PIN);

	timer_mdelay(1);
	/* release reset of pcie switch */
	gpio_set(MCU_8533_RSTB_PORT, MCU_8533_RSTB_PIN);

	tca9554_init();
	mp5475_init();
	board_power_init();

	chip_init();

	printf("\r\npress \'u\' to start uart upgrade\r\n");

	while (1) {
		chip_update();
	}

	return 0;
}
