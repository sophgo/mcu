/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <led.h>
#include <signal.h>
#include <pin.h>


led_nodes const
		power_led			=	{	POWER_LED_PORT,				POWER_LED_PIN			},
		mcu_ssd_alarm_led	=	{	MCU_SSD_ALARM_LED_PORT,		MCU_SSD_ALARM_LED_PIN	},
		mcu_ssd_status_led	=	{	MCU_SSD_STATUS_LED_PORT,	MCU_SSD_STATUS_LED_PIN	},
		mcu_alarm_led		=	{	MCU_ALARM_LED_PORT,			MCU_ALARM_LED_PIN		},
		mcu_status_led		=	{	MCU_STATUS_LED_PORT,		MCU_STATUS_LED_PIN		};


void led_control(led_nodes led_node, int status)
{
	switch (status) {
		case false:
			gpio_clear(led_node.port, led_node.pin);
			break;
		case true:
			gpio_set(led_node.port, led_node.pin);
			break;
		default:
			break;
	}
}
