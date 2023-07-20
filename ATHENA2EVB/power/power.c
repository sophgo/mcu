/*
 * power.c :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_gpio.h>
#include <timer.h>
#include <power.h>
#include <debug.h>
#include <common.h>
#include <upgrade.h>
#include <power_node.h>
#include <system.h>
#include <stdio.h>
#include <stdbool.h>
#include <timer.h>
#include <chip.h>
#include <slt.h>
#include <loop.h>
#include <gpio.h>
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

static int node_on(struct board_power_nodes const *node, uint8_t board_type)
{
	__disable_irq();

	int err = 0;

//	debug("%-20s ", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		gpio_set(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_CHECK) {
		err = node_check(node);
	} else {
		power_on_func func = (power_on_func)node->param[0];

		if (func)
			err = func();
	}

//	debug("[%c]", err ? 'X' : 'O');

	if (err == 0 && node->delay) {
		timer_delay_us(node->delay);
	}
//	debug(" [Delay: %d]\n", node->delay);

	__enable_irq();

	return err;
}

static void node_off(struct board_power_nodes const *node)
{
//	debug("%s\n", node->name);

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
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i, get_board_type());
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
	return err;
}

void node_seq_off(struct board_power_nodes const *node, unsigned int num)
{
	int i;

	for (i = num - 1; i >= 0; --i)
		node_off(node + i);
}

void power_on(void)
{
	int err = node_seq_on(board_power_node, ARRAY_SIZE(board_power_node));

	if (err) {
		debug("power on failed node: %d\n", err);
	} else {
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
}

void board_power_control(void)
{
	/* Main loop query */
	switch (get_board_type()) {
	case ATHENA2EVB:
		break;
	default:
		break;
	}
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
