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
	gpio_output(power_reset_signal, false);
	gpio_output(power_management_signal, false);
	gpio_output(power_on_signal, false);
	gpio_output(power_wakeup_signal, false);
	gpio_output(system_reset_signal, false);

	gpio_output(pcie_mode_signal, false);

#if DDR_TYPE == 0	/* DDR4 */
	gpio_output(power_led, false);
	gpio_output(mcu_status_led, true);
#endif			/* DDR4 */

	core_power_status = gpio_input(core_power_status_signal);
}
