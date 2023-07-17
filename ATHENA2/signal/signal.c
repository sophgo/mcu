/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <chip.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <project.h>
#include <stdbool.h>
#include <tick.h>
#include <timer.h>
#include <stdio.h>
#include <signal.h>


#define KEY_POWER_ON_PRESS_VALID_VALUE		1

#define KEY_POWER_ON_PRESS_TIME_SHORT		200
#define KEY_POWER_ON_PRESS_TIME_LONG		5000


a2_output_signal_nodes const
		a2_power_reset		=	{	PWR_RSTN_H_PORT,		PWR_RSTN_H_PIN		},
		a2_power_management	=	{	PWR_BUTTON1_H_PORT,		PWR_BUTTON1_H_PIN	},
		a2_power_on			=	{	PWR_ON_H_PORT,			PWR_ON_H_PIN		},
		a2_power_wakeup		=	{	PWR_WAKEUP_H_PORT,		PWR_WAKEUP_H_PIN	},
		a2_system_reset		=	{	SYS_RSTN_H_HASH_PORT,	SYS_RSTN_H_HASH_PIN	},
		a2_pcie_mode		=	{	PCIE0_SEL_PORT,			PCIE0_SEL_PIN		};

a2_input_signal_nodes const
		a2_core_power_status	=	{	PG_VDDC_0V8_PORT,	PG_VDDC_0V8_PIN		},
		a2_mcu_boot1			=	{	MCU_BOOT1_PORT,		MCU_BOOT1_PIN		};


void board_output_signal(a2_output_signal_nodes a2_output_signal_node, int status)
{
	switch (status) {
		case 0:
			gpio_clear(a2_output_signal_node.port, a2_output_signal_node.pin);
			break;
		case 1:
			gpio_set(a2_output_signal_node.port, a2_output_signal_node.pin);
			break;
		default:
			break;
	}
}

bool board_input_signal(a2_input_signal_nodes a2_input_signal)
{
	return gpio_get(a2_input_signal.port, a2_input_signal.pin);
}

static bool is_key_power_on_pressed(void)
{
	if (gpio_get(KEY_POWER_ON_PORT, KEY_POWER_ON_PIN) == KEY_POWER_ON_PRESS_VALID_VALUE)
		return true;
	return false;
}

static void acp_key_power_on_button_control(void)
{
	uint16_t press_time, curren_time, last_time;

	curren_time = last_time = tick_get();
	while (is_key_power_on_pressed()) {
		curren_time = tick_get();
		if(curren_time - last_time > KEY_POWER_ON_PRESS_TIME_LONG)
			break;
	}
	press_time = curren_time - last_time;

	if (press_time > KEY_POWER_ON_PRESS_TIME_LONG) {
		if (!power_is_on) {
			power_on();
		}
		while (is_key_power_on_pressed())
			;
	} else if (press_time > KEY_POWER_ON_PRESS_TIME_SHORT) {
		if (power_is_on) {
			power_off();
		}
		while (is_key_power_on_pressed())
			;
	}
}

void acp_power_control(void)
{
	acp_key_power_on_button_control();
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
