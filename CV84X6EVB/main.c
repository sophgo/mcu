/*
 * main.c: CV84X6EVB 固件入口
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick/tick.h>
#include <system/system.h>
#include <timer/timer.h>
#include <pin.h>
#include <common/common.h>
#include <project.h>
#include <console/console.h>
#include <stdio.h>
#include <slave/slave.h>
#include <loop/loop.h>
#include <gpio/gpio.h>
#include <ct7451/ct7451.h>
#include <pmbus/pmbus.h>
#include <power_seq/power_seq.h>
#include <pwm/pwm.h>
#include <debug.h>

int main(void)
{
	system_init();

	debug("\n\n[ CV84X6EVB ]\n");
	debug("[ Firmware built on: %s - %s ]\n", __DATE__, __TIME__);

	power_seq_init();
	pwm_init();
	ct7451_init();
	slave_init();
	gpio_control_init();
	console_add();

	/* Never return */
	loop_start();

	return 0;
}