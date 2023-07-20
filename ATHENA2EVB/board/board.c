/*
 * board.c :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <system.h>
#include <board.h>
#include <gpio.h>
#include <common.h>
#include <project.h>
#include <power.h>

bool core_power_status;

void board_init(void)
{
	set_board_type(ATHENA2EVB);

	gpio_output(power_reset_node, true);
	gpio_output(power_management_node, true);
	gpio_output(power_on_node, false);
	gpio_output(power_wakeup_node, true);
	gpio_output(system_reset_node, true);
	gpio_output(pcie_mode_node, false);

	core_power_status = gpio_input(core_power_status_node);
}
