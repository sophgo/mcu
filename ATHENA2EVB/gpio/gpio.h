/*
 * gpio.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <stdbool.h>
#include <common/common.h>

extern struct gpio_nodes
	power_reset_signal,
	power_management_signal,
	power_on_signal,
	power_wakeup_signal,
	system_reset_signal,
	pcie_mode_signal,

#if DDR_TYPE == 0	/* DDR4 */

	power_led,
	mcu_status_led,

#endif			/* DDR4 */

	mcu_pwr_button;

extern struct gpio_nodes
	core_power_status_signal;

void gpio_output(struct gpio_nodes output_signal, int status);
bool gpio_input(struct gpio_nodes input_signal);
void signal_control(void);

#endif /* __SIGNAL_H__ */
