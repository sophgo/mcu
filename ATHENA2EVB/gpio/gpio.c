/*
 * gpio.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <chip/chip.h>
#include <common/common.h>
#include <pin.h>
#include <power/power.h>
#include <project.h>
#include <stdbool.h>
#include <tick/tick.h>
#include <timer/timer.h>
#include <stdio.h>
#include <gpio/gpio.h>

struct gpio_nodes
	power_reset_signal = {PWR_RSTN_H_PORT, PWR_RSTN_H_PIN},
	power_management_signal = {PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN},
	power_on_signal = {PWR_ON_H_PORT, PWR_ON_H_PIN},
	power_wakeup_signal = {PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN},
	system_reset_signal = {SYS_RSTN_H_HASH_PORT, SYS_RSTN_H_HASH_PIN},
	pcie_mode_signal = {PCIE0_SEL_PORT, PCIE0_SEL_PIN};

struct gpio_nodes
	core_power_status_signal = {PG_VDDC_0V8_PORT, PG_VDDC_0V8_PIN};

void gpio_output(struct gpio_nodes output_signal, int status)
{
	switch (status) {
		case 0:
			gpio_clear(output_signal.port, output_signal.pin);
			break;
		case 1:
			gpio_set(output_signal.port, output_signal.pin);
			break;
		default:
			break;
	}
}

bool gpio_input(struct gpio_nodes input_signal)
{
	return gpio_get(input_signal.port, input_signal.pin);
}

int sys_rst_assert_on(void)
{
	chip_enable();
	return 0;
}

void sys_rst_assert_off(void)
{

}

int sys_rst_deassert_on(void)
{
	chip_disable();
	return 0;
}

void sys_rst_deassert_off(void)
{
	/* reset chip firstly when power off */
	chip_disable();
}
