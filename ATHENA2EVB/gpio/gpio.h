/*
 * gpio.h :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <stdbool.h>
#include <common.h>

extern struct gpio_nodes
	power_reset_node,
	power_management_node,
	power_on_node,
	power_wakeup_node,
	system_reset_node,
	pcie_mode_node;

extern struct gpio_nodes
	core_power_status_node;

void gpio_output(struct gpio_nodes output_signal_node, int status);
bool gpio_input(struct gpio_nodes input_signal);

#endif /* __SIGNAL_H__ */
