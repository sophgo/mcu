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
#include <tca6416a.h>
#include <pic.h>
#include <se5.h>
#include <sm5.h>
#include <pcie.h>

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

	tca6416a_probe();
	pic_probe();

	power_init();
	isl91212_init();
	power_on();
	chip_init();
	board_init();

	debug("%s %s working at %s mode\n",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application",
	      get_work_mode() == WORK_MODE_SOC ? "soc" : "pcie");

	/* auto detect or correct board type
	 * based on some characteristic on motherboard
	 */
	if (get_work_mode() == WORK_MODE_PCIE)
		set_board_type(SM5P);
	else if (pic_available())
		set_board_type(SE5);
	else {
		if (tca6416a_available()) {
			set_board_type(
				get_declared_board_type() == SE5 ?
				SE5 : SM5S);
		} else {
			/* on test boards */
			set_board_type(SA5);

			/* change alert pin from input mode to output mode */
			gpio_bit_reset(ALERT_PORT, ALERT_PIN);
			gpio_init(ALERT_PORT, GPIO_MODE_OUT_PP,
				  GPIO_OSPEED_2MHZ, ALERT_PIN);
		}
	}

	/* but chip reset still be asserted */
	if (get_work_mode() == WORK_MODE_SOC) {
		if (get_board_type() == SE5)
			se5_init();
		else
			sm5_init();
	}

	mon_init();
	slave_init();

	if (get_work_mode() == WORK_MODE_SOC)
		chip_enable();
	else
		pcie_init();

	/* never return */
	loop_start();

	return 0;
}
