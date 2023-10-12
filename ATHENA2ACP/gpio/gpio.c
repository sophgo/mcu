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

bool relay_system_reset_signal, relay_key_power_on_signal;

/* Output GPIO */
const struct gpio_nodes
	power_reset_signal = {PWR_RSTN_H_PORT, PWR_RSTN_H_PIN},
	power_wakeup_signal = {PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN},
	pcie_mode_signal = {PCIE0_SEL_PORT, PCIE0_SEL_PIN},

	mcu_usb_hub_reset_signal = {MCU_HUB_RESET_HASH_PORT, MCU_HUB_RESET_HASH_PIN},
	fan_power_signal = {OVER_TEMP_PORT, OVER_TEMP_PIN},
	bluetooth_power_signal = {BT_M2_EN_PORT, BT_M2_EN_PIN},
	wifi_power_signal = {W_DISABLE1_CTRL_PORT, W_DISABLE1_CTRL_PIN},
	sd_card_power_select_signal = {SD_PW_SEL_PORT, SD_PW_SEL_PIN},
	flight_mode_signal = {FLIGHT_MODE_HASH_PORT, FLIGHT_MODE_HASH_PIN},
	lte_5g_power_signal = {GPIO_LTE_PWR_EN_PORT, GPIO_LTE_PWR_EN_PIN},
	lte_5g_reset_signal = {GPIO_LTE_RST_PORT, GPIO_LTE_RST_PIN},

	power_led = {POWER_LED_PORT, POWER_LED_PIN},
	mcu_ssd_alarm_led = {MCU_SSD_ALARM_LED_PORT, MCU_SSD_ALARM_LED_PIN},
	mcu_ssd_status_led = {MCU_SSD_STATUS_LED_PORT, MCU_SSD_STATUS_LED_PIN},
	mcu_alarm_led = {MCU_ALARM_LED_PORT, MCU_ALARM_LED_PIN},
	mcu_status_led = {MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN},

	/* Output Signal Pass Through */
	power_management_signal = {PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN},
	power_on_signal = {PWR_ON_H_PORT, PWR_ON_H_PIN};

/* Input GPIO */
const struct gpio_nodes
	core_power_status_signal = {PG_VDDC_0V8_PORT, PG_VDDC_0V8_PIN},
	/* Input Signal Pass Through */
	system_reset_signal = {SYS_RSTN_H_HASH_PORT, SYS_RSTN_H_HASH_PIN},
	key_power_on_signal = {KEY_POWER_ON_PORT, KEY_POWER_ON_PIN};

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

void signal_passthrough(void)
{
	relay_system_reset_signal = gpio_input(system_reset_signal);
	relay_key_power_on_signal = gpio_input(key_power_on_signal);
	gpio_output(power_on_signal, relay_system_reset_signal);
	gpio_output(power_management_signal, relay_key_power_on_signal);
}

void signal_control(void)
{
	signal_passthrough();
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
