/*
 * main.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick/tick.h>
#include <system/system.h>
#include <timer/timer.h>
#include <power/power.h>
#include <pin.h>
#include <common/common.h>
#include <project.h>
#include <console/console.h>
#include <stdio.h>
#include <slave/slave.h>
#include <chip/chip.h>
#include <loop/loop.h>
#include <board/board.h>
#include <i2c-slaves/ct7451/ct7451.h>
#include <debug.h>

int main(void)
{
	system_init();

	debug("\n\n[ Athena2 ACP ]\n");
	debug("[ Firmware built on: %s - %s ]\n", __DATE__, __TIME__);

	power_init();
	board_init();
	chip_init();
	slave_init();
	ct7451_init();
	console_add();

	/* Recommended to wait for at least 30ms before the power sequence */
	timer_delay_ms(30);
	power_on();

	/* Never return */
	loop_start();

	return 0;
}
