#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <isl91212.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <mon.h>
#include <loop.h>
#include <upgrade.h>
#include <project.h>
#include <slave.h>

#include <stdio.h>

int main(void)
{
	system_init();

	debug("Hello GD32E50x\n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif

	led_init();

	power_init();
	isl91212_init();
	power_on();

	board_init();
	set_board_type(SA5);

	chip_init();

	debug("%s %s working at %s mode\n",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application",
	      get_work_mode() == WORK_MODE_SOC ? "soc" : "pcie");

	mon_init();
	slave_init();

	chip_enable();

	loop_start();

	return 0;
}
