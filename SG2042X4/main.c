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
#include <wdt.h>
#include <chip.h>
#include <loop.h>

int main(void)
{
	system_init();
	debug("\nMANGO SG2042X4\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);
	led_init();
	power_init();
	board_power_control();
	chip_init();
	slave_init();
	board_init();
	wdt_tick_task_enable();
	console_add();
	/* never return */
	loop_start();
	return 0;
}
