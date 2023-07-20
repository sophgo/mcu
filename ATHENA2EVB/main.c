/*
 * main.c :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
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
#include <board.h>
#include <debug.h>

int main(void)
{
	system_init();

	debug("\n\n[ Athena2 EVB ]\n");
	debug("[ Firmware built on: %s - %s ]\n", __DATE__, __TIME__);

	power_init();
	board_init();
	chip_init();
	slave_init();
	console_add();

	/* Recommended to wait for at least 30ms before the power sequence */
	timer_delay_ms(30);
	power_on();

	/* never return */
	loop_start();

	return 0;
}
