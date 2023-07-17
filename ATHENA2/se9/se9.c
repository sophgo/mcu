/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <pin.h>
#include <se9.h>
#include <common.h>


se9_output_nodes const
		mcu_usb_hub_reset		=	{	MCU_HUB_RESET_HASH_PORT,	MCU_HUB_RESET_HASH_PIN	},
		fan_power				=	{	OVER_TEMP_PORT,				OVER_TEMP_PIN			},
		bluetooth_power			=	{	BT_M2_EN_PORT,				BT_M2_EN_PIN			},
		wifi_power				=	{	W_DISABLE1_CTRL_PORT,		W_DISABLE1_CTRL_PIN		},
		sd_card_power_select	=	{	SD_PW_SEL_PORT,				SD_PW_SEL_PIN			},
		flight_mode				=	{	FLIGHT_MODE_HASH_PORT,		FLIGHT_MODE_HASH_PIN	},
		lte_5g_power			=	{	GPIO_LTE_PWR_EN_ACP_PORT,	GPIO_LTE_PWR_EN_ACP_PIN	},
		lte_5g_reset			=	{	GPIO_LTE_RST_ACP_PORT,		GPIO_LTE_RST_ACP_PIN	};


void se9_output_signal(se9_output_nodes se9_output_node, int status)
{
	switch (status) {
		case 0:
			gpio_clear(se9_output_node.port, se9_output_node.pin);
			break;
		case 1:
			gpio_set(se9_output_node.port, se9_output_node.pin);
			break;
		default:
			break;
	}
}
