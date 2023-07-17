/* Sophgo Technology. Created by Donghao on 7/13/23. */


#ifndef __SE9_H__
#define __SE9_H__


#include <common.h>


extern se9_output_nodes const
		mcu_usb_hub_reset,
		fan_power,
		bluetooth_power,
		wifi_power,
		sd_card_power_select,
		flight_mode,
		lte_5g_power,
		lte_5g_reset;


void se9_output_signal(se9_output_nodes se9_output_node, int status);


#endif /* __SE9_H__ */