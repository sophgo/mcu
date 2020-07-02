/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include <system.h>
#include <i2c_master.h>
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <pin.h>
#include <timer.h>
#include <mon.h>

int main(void)
{
	clock_init();
	system_init();

	i2c_master_init(I2C1);
	i2c_master_init(I2C2);

	debug("BITMAIN SOPHONE SM5MINI -- %s\r\n", VERSION);

	mp5475_init();
	chip_init();
	mon_init();

	printf("\r\npress \'u\' to start uart upgrade\r\n");

	while (true) {
		chip_update();
		mon_process();
	}

	return 0;
}
