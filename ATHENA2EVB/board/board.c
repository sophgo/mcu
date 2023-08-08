/*
 * board.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <system/system.h>
#include <board/board.h>
#include <gpio/gpio.h>
#include <common/common.h>
#include <project.h>
#include <power/power.h>

bool core_power_status;

void board_init(void)
{
	set_board_type(ATHENA2EVB);

	gpio_output(power_reset_node, false);
	gpio_output(power_management_node, false);
	gpio_output(power_on_node, false);
	gpio_output(power_wakeup_node, false);
	gpio_output(system_reset_node, false);

	gpio_output(pcie_mode_node, false);

	core_power_status = gpio_input(core_power_status_node);
}
