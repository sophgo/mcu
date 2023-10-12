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

#define MCU_PWR_BUTTON_VALID_VALUE		1
#define MCU_PWR_BUTTON_PRESS_TIME		200
#define MCU_STATUS_LED_INTERVAL			250

bool last_mcu_pwr_button_status = false;

struct gpio_nodes
	power_reset_signal = {PWR_RSTN_H_PORT, PWR_RSTN_H_PIN},
	power_management_signal = {PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN},
	power_on_signal = {PWR_ON_H_PORT, PWR_ON_H_PIN},
	power_wakeup_signal = {PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN},
	system_reset_signal = {SYS_RSTN_H_HASH_PORT, SYS_RSTN_H_HASH_PIN},
	pcie_mode_signal = {PCIE0_SEL_PORT, PCIE0_SEL_PIN},

#if DDR_TYPE == 0	/* DDR4 */
	power_led = {POWER_LED_PORT, POWER_LED_PIN},
	mcu_status_led = {MCU_STATUS_LED_PORT, MCU_STATUS_LED_PIN},
#endif			/* DDR4 */

	mcu_pwr_button = {MCU_PWR_BUTTON_PORT, MCU_PWR_BUTTON_PIN};

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

static void mcu_pwr_button_control(void)
{
	uint16_t valid_time, current_time, last_time;

	current_time = last_time = tick_get();
	while (gpio_input(mcu_pwr_button) != last_mcu_pwr_button_status) {
		current_time = tick_get();
		if(current_time - last_time > MCU_PWR_BUTTON_PRESS_TIME)
			break;
	}

	valid_time = current_time - last_time;

	if (valid_time > MCU_PWR_BUTTON_PRESS_TIME) {
		bool current_status = gpio_input(mcu_pwr_button);
		if (gpio_input(mcu_pwr_button)) {
                        power_off();
		} else {
                        power_on();
		}
		last_mcu_pwr_button_status = current_status;
	}
}

#if DDR_TYPE == 0	/* DDR4 */

static unsigned long mcu_status_led_last_time;
int mcu_status_led_status = 0;

static void mcu_status_led_control(void)
{
	unsigned long current_time;

	current_time = tick_get();

	if (current_time - mcu_status_led_last_time > MCU_STATUS_LED_INTERVAL) {
		switch (mcu_status_led_status) {
			case 0:
				gpio_output(mcu_status_led, true);
				mcu_status_led_status = 1;
				break;
			case 1:
				gpio_output(mcu_status_led, false);
				mcu_status_led_status = 0;
				break;
			default:
				break;
		}
		mcu_status_led_last_time = current_time;
	}
}

static void power_led_control(void)
{
	switch ((int)gpio_input(core_power_status_signal)) {
		case 0:
			gpio_output(power_led, false);
			break;
		case 1:
			gpio_output(power_led, true);
			break;
		default:
			break;
	}
}

#endif			/* DDR4 */

void signal_control(void)
{
	mcu_pwr_button_control();

#if DDR_TYPE == 0	/* DDR4 */

	mcu_status_led_control();
	power_led_control();

#endif			/* DDR4 */

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
