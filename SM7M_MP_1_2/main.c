#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <axp15060.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <mon.h>
#include <loop.h>
#include <upgrade.h>
#include <project.h>
#include <slave.h>
#include <tca6416a.h>
#include <pic.h>
#include <se5.h>
#include <sm5.h>
#include <pcie.h>
#include <rst_key.h>
#include <stdio.h>

int main(void)
{
	system_init();
	//rst_key_init();

	debug("\nBITMAIN SOPHONE  SM7M_MP_1_2  \n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif

	led_init();
	gpio_bit_set(EN_VDDIO5_PORT, EN_VDDIO5_PIN);
	if (tca6416a_probe())
		debug("\ntca6416a is active!\n");
	pic_probe();
	//power_init();
	axp15060_init();
	power_on();
	chip_init();
	board_init();

	debug("%s %s working at %s mode\n",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application",
	      get_work_mode() == WORK_MODE_SOC ? "soc" : "pcie");

	set_board_type(SM7M_MP_1_2);

	/* but chip reset still be asserted */
	if (get_work_mode() == WORK_MODE_SOC) {
		sm5_init();
	}

	mon_init();
	slave_init();

	if (get_work_mode() == WORK_MODE_SOC) {
		chip_enable();
	} else
		pcie_init();

	/* never return */
	debug("MCU firmware finished\n");
	loop_start();

	return 0;
}
