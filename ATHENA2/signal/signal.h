/* Sophgo Technology. Created by Donghao on 7/13/23. */


#ifndef __SIGNAL_H__
#define __SIGNAL_H__


#include <stdbool.h>
#include <common.h>


extern a2_output_signal_nodes const
		a2_power_reset,
		a2_power_management,
		a2_power_on,
		a2_power_wakeup,
		a2_system_reset,
		a2_pcie_mode;


extern a2_input_signal_nodes const
		a2_core_power_status,
		a2_mcu_boot1;


void board_output_signal(a2_output_signal_nodes a2_output_signal_node, int status);
bool board_input_signal(a2_input_signal_nodes a2_input_signal);
void acp_power_control(void);


#endif /* __SIGNAL_H__ */