/*
 * chip.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <chip/chip.h>
#include <pin.h>
#include <tick/tick.h>
#include <common/common.h>
#include <power/power.h>
#include <system/system.h>
#include <stdio.h>
#include <gpio/gpio.h>

static uint32_t uptime;
static uint32_t reset_times;
static int chip_enabled;
static int need_power_on;

uint32_t chip_reset_times(void)
{
	return reset_times;
}

uint32_t chip_uptime(void)
{
	return uptime;
}

void chip_disable(void)
{
	// TODO: This signal is being passed through to internal MCU
	//gpio_output(system_reset_signal, false);
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	// TODO: This signal is being passed through to internal MCU
	//gpio_output(system_reset_signal, true);
	chip_enabled = true;
}

void chip_reset(void)
{
	chip_disable();
	tick_delay_ms(30);
	uptime = 0;
	++reset_times;
	chip_enable();
}

void chip_popd_reset_early(void)
{
	power_off();
	chip_disable();
	tick_delay_ms(50);
}

void chip_popd_reset_end(void)
{
	power_on();
	uptime = 0;
	++reset_times;

	chip_enable();
}

static void chip_process(void)
{
	if (chip_enabled)
		++uptime;
}

void chip_init(void)
{
	tick_register_task(chip_process, 1000);
}

int chip_is_enabled(void)
{
	return chip_enabled;
}

void set_need_power_on(void)
{
	need_power_on = 1;
}

void clr_need_power_on(void)
{
	need_power_on = 0;
}

int get_need_power_on_status(void)
{
	return need_power_on;
}
