/*
 * power.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_gpio.h>
#include <timer/timer.h>
#include <power/power.h>
#include <debug.h>
#include <common/common.h>
#include <upgrade/upgrade.h>
#include <power/power_node/power_node.h>
#include <system/system.h>
#include <stdio.h>
#include <stdbool.h>
#include <timer/timer.h>
#include <chip/chip.h>
#include <i2c-slaves/slt/slt.h>
#include <loop/loop.h>
#include <gpio/gpio.h>
#include <project.h>

/* in us */
#define NODE_CHECK_TIMEOUT	(10 * 1000)

int power_is_on;

static int node_check(struct board_power_nodes const *node)
{
	return 0;

	int err = 0;
	uint32_t port = node->param[0];
	uint16_t pin = node->param[1];

	timer_start(NODE_CHECK_TIMEOUT);

	while (gpio_get(port, pin) == 0) {
		if (timer_is_timeout()) {
			err = -1;
			break;
		}
	}
	timer_stop();

	return err;
}

static int node_on(struct board_power_nodes const *node)
{
	__disable_irq();

	int err = 0;

	debug("%-20s ", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		gpio_set(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_CHECK) {
		err = node_check(node);
	} else {
		power_on_func func = (power_on_func)node->param[0];

		if (func)
			err = func();
	}

	debug("[%c]", err ? 'X' : 'O');

	if (err == 0 && node->delay) {
		timer_delay_us(node->delay);
	}

	debug("[Delay: %d]\n", node->delay);

	__enable_irq();

	return err;
}

static void node_off(struct board_power_nodes const *node)
{
	debug("%s\n", node->name);

	/* skip check nodes */
	if (node->type == NODE_TYPE_ENABLE) {
		gpio_clear(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_FUNCTION) {
		power_off_func func = (power_off_func)node->param[1];

		if (func)
			func();
	}
}

int node_seq_on(struct board_power_nodes const *node, unsigned int num)
{
	debug("\n");
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i);
		if (err)
			break;
	}

	/* do not power off */
#if 0
	if (err) {
		--i;
		while (i >= 0) {
			node_off(node + i);
			--i;
		}
	}
#endif
	debug("\n");
	return err;
}

void node_seq_off(struct board_power_nodes const *node, unsigned int num)
{
	debug("\n");
	int i;

	for (i = num - 1; i >= 0; --i) {
		node_off(node + i);
		timer_delay_ms(1000);
	}
	debug("\n");
}

void power_on(void)
{
	int err = node_seq_on(board_power_node, ARRAY_SIZE(board_power_node));

	if (err) {
		debug("power on failed node: %d\n", err);
	} else {
		//led_set_frequency(1);
		gpio_output(power_led, true);
		gpio_output(mcu_ssd_status_led, true);
		gpio_output(mcu_status_led, true);
		gpio_output(mcu_alarm_led, false);
		gpio_output(mcu_ssd_alarm_led, false);

		/* Wait for 20 ms to reset 5G */
		timer_delay_ms(20);
		gpio_output(lte_5g_reset, false);
		gpio_output(mcu_usb_hub_reset, false);

		chip_enable();
		power_is_on = true;
	}
}

void power_off(void)
{
	slt_reset();
	chip_disable();
	power_is_on = false;

	node_seq_off(board_power_node, ARRAY_SIZE(board_power_node));

	gpio_output(power_led, false);
	gpio_output(mcu_ssd_status_led, false);
	gpio_output(mcu_status_led, false);
	gpio_output(mcu_ssd_alarm_led, true);
	gpio_output(mcu_ssd_alarm_led, true);

	gpio_output(lte_5g_reset, true);
}

void board_power_control(void)
{
	/* Main loop query */
	power_control();
}

void power_init(void)
{
	power_is_on = false;
	loop_add(board_power_control);
}

int power_status(void)
{
	return power_is_on;
}
