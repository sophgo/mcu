/*
 * gpio.c :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <chip.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <project.h>
#include <stdbool.h>
#include <tick.h>
#include <timer.h>
#include <stdio.h>
#include <gpio.h>

#define KEY_POWER_ON_PRESS_VALID_VALUE		1
#define KEY_POWER_ON_PRESS_TIME_SHORT		200
#define KEY_POWER_ON_PRESS_TIME_LONG		5000

struct gpio_nodes
	power_reset_node = {PWR_RSTN_H_PORT, PWR_RSTN_H_PIN},
	power_management_node = {PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN},
	power_on_node = {PWR_ON_H_PORT, PWR_ON_H_PIN},
	power_wakeup_node = {PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN},
	system_reset_node = {SYS_RSTN_H_HASH_PORT, SYS_RSTN_H_HASH_PIN},
	pcie_mode_node = {PCIE0_SEL_PORT, PCIE0_SEL_PIN};

struct gpio_nodes
	core_power_status_node = {PG_VDDC_0V8_PORT, PG_VDDC_0V8_PIN};

void gpio_output(struct gpio_nodes gpio_node, int status)
{
	switch (status) {
	case 0:
		gpio_clear(gpio_node.port, gpio_node.pin);
		break;
	case 1:
		gpio_set(gpio_node.port, gpio_node.pin);
		break;
	default:
		break;
	}
}

bool gpio_input(struct gpio_nodes gpio_node)
{
	return gpio_get(gpio_node.port, gpio_node.pin);
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
