#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <project.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
#include <mcu.h>
#include <chip.h>
#include <ct7451.h>
#include <pcie.h>

int main(void)
{
	system_init();
	dbg_printf("\nBM2044REVB\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	led_init();
   	power_init();
	chip_init();
	slave_init();
	console_init();
	set_board_type(BM2044REVB);
	while (1) {
		board_power_control();
		mcu_process();
		ct7451_process();
		console_poll();
	}

	return 0;
}
