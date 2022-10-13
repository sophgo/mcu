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
int main(void)
{
	system_init();
	debug("\nMANGO SG2042EVB\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);

	board_power_init();
	slave_init();
	console_init();
	while (1) {
		console_poll();
		board_power_init();
	}

	return 0;
}
