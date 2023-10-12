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

extern const struct gpio_nodes

	/* Power Control Signal */
	power_reset_signal,
	power_wakeup_signal,
	pcie_mode_signal,

	/* Peripheral Control Signal */
	mcu_usb_hub_reset_signal,
	fan_power_signal,
	bluetooth_power_signal,
	wifi_power_signal,
	sd_card_power_select_signal,
	flight_mode_signal,
	lte_5g_power_signal,
	lte_5g_reset_signal,

	/* LED */
	power_led,
	mcu_ssd_alarm_led,
	mcu_ssd_status_led,
	mcu_alarm_led,
	mcu_status_led,

	/* Signal Pass Through */
	power_management_signal,
	power_on_signal;

extern const struct gpio_nodes
	core_power_status_signal,
	system_reset_signal,
	key_power_on_signal;

void gpio_output(struct gpio_nodes output_signal, int status);
bool gpio_input(struct gpio_nodes input_signal);
void signal_control(void);

#endif /* __SIGNAL_H__ */
