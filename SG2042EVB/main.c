#define DEBUG 1
#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <project.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
#include <nct218.h>
#include <mcu.h>
#include <chip.h>

int main(void)
{
	system_init();
	debug("\nMANGO SG2042EVB\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);
	led_init();
	power_init();
	board_power_init();
	chip_init();
	slave_init();
	nct218_init();
	console_init();
	set_board_type(SG2042EVB);
	chip_enable();
	while (1) {
		board_power_init();
		console_poll();
		mcu_process();
		nct218_process();
	}
	return 0;
}
