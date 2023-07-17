/* Sophgo Technology. Created by Donghao on 7/13/23. */


#ifndef __LED_H__
#define __LED_H__


#include <common.h>


extern led_nodes const
		power_led,
		mcu_ssd_alarm_led,
		mcu_ssd_status_led,
		mcu_alarm_led,
		mcu_status_led;


void led_control(led_nodes led_node, int status);


#endif  /* __LED_H__ */
