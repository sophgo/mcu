/*
 * board.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <system/system.h>
#include <board/board.h>
#include <gpio/gpio.h>
#include <adc/adc.h>
#include <common/common.h>
#include <project.h>
#include <power/power.h>

bool core_power_status;

void board_init(void)
{
	gpio_output(power_reset_signal, false);
	gpio_output(power_wakeup_signal, true);
	gpio_output(pcie_mode_signal, false);

	core_power_status = gpio_input(core_power_status_signal);

	gpio_output(power_led, false);
	gpio_output(mcu_ssd_alarm_led, true);
	gpio_output(mcu_ssd_status_led, false);
	gpio_output(mcu_alarm_led, true);
	gpio_output(mcu_status_led, false);

	gpio_output(mcu_usb_hub_reset_signal, true);
	gpio_output(fan_power_signal, true);
	gpio_output(bluetooth_power_signal, false);
	gpio_output(wifi_power_signal, false);
	gpio_output(sd_card_power_select_signal, false);
	gpio_output(flight_mode_signal, false);
	gpio_output(lte_5g_power_signal, false);
	gpio_output(lte_5g_reset_signal, true);
}
