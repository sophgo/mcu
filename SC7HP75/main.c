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
#include <chip.h>
#include <mon.h>
#include <upgrade.h>
#include <project.h>
#include <pca9848.h>
#include <mp5475.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
int main(void)
{
	system_init();

	debug("\nBITMAIN SOPHONE SC7HP75\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("bitmain\n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif
	pca9848_init();
	mp5475_init();
	board_power_init();
#if 0
	timer_mdelay(30);
	sys_rst_enable();
#endif
	chip_init();
	mon_init();
	slave_init();
	console_init();

	//sys_rst_enable();
	while (1) {
		chip_update();
		if (chip_enable())
			mon_process();

		console_poll();
	}

	return 0;
}
