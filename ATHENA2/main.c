/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system/system.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <project.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
#include <chip.h>
#include <loop.h>
#include <se9.h>
#include <board.h>
#include <include.h>


int main(void)
{
	system_init();
	power_init();

	/* Distinguish Athena2EVB and Athena2ACP */
	board_init();

	chip_init();
	slave_init();
	console_add();

    /* Recommended to wait for at least 30ms before the power sequence */
//	debug("Initialization Complete! Wait for 30 ms to power on.\n\n");
	timer_delay_ms(30);
	power_on();

	/* never return */
//	debug("\nWelcome to A2 MCU CLI:\n\n");
	loop_start();

	return 0;
}
