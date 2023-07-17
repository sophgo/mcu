/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <include.h>
#include <system/system.h>
#include <board.h>
#include <led.h>
#include <se9.h>
#include <signal.h>
#include <adc.h>
#include <common.h>
#include <project.h>
#include <power.h>


bool core_power_status;
bool mcu_boot1;


void evb_board_init() {
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	board_output_signal	(a2_power_reset,		true);
	board_output_signal	(a2_power_management,	true);
	board_output_signal	(a2_power_on,			false);
	board_output_signal	(a2_power_wakeup,		true);
	board_output_signal	(a2_system_reset,		true);
	board_output_signal	(a2_pcie_mode,			false);

	core_power_status	= board_input_signal(a2_core_power_status);
	mcu_boot1			= board_input_signal(a2_mcu_boot1);
}

void acp_board_init() {
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	/* Same as EVB */

	board_output_signal	(a2_power_reset,		true);
	board_output_signal	(a2_power_management,	true);
	board_output_signal	(a2_power_on,			false);
	board_output_signal	(a2_power_wakeup,		true);
	board_output_signal	(a2_system_reset,		true);
	board_output_signal	(a2_pcie_mode,			false);

	core_power_status	= board_input_signal(a2_core_power_status);
	mcu_boot1			= board_input_signal(a2_mcu_boot1);

	/* ACP Exclusive */

	led_control			(power_led, 			false);
	led_control			(mcu_ssd_alarm_led, 	false);
	led_control			(mcu_ssd_status_led, 	false);
	led_control			(mcu_alarm_led,			false);
	led_control			(mcu_status_led,		false);

	se9_output_signal	(mcu_usb_hub_reset,		true);
	se9_output_signal	(fan_power,				true);
	se9_output_signal	(bluetooth_power,		false);
	se9_output_signal	(wifi_power,			false);
	se9_output_signal	(sd_card_power_select,	false);
	se9_output_signal	(flight_mode,			false);
	se9_output_signal	(lte_5g_power,			false);
	se9_output_signal	(lte_5g_reset,			true);
}

/* Board init based on PCB version commented for evaluation */
/*void board_init(void)
{
	if (get_pcb_version() == ATHENA2EVB_PCB_VERSION) {
		set_board_type(ATHENA2EVB);
		evb_system_init();
		evb_board_init();
	} else if (get_pcb_version() == ATHENA2ACP_PCB_VERSION) {
		set_board_type(ATHENA2ACP);
		acp_system_init();
		acp_board_init();
		system_i2c1_init();
	} else {
//		set_board_type(UNKNOWN);
		*//* Unknown board will be treated as EVB for now*//*
		set_board_type(ATHENA2EVB);
		evb_system_init();
		evb_board_init();
	}
}*/

void board_init(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	if (TMP_BOARD == ATHENA2EVB) {
		set_board_type(ATHENA2EVB);
		evb_system_init();
		evb_board_init();
	} else if (TMP_BOARD == ATHENA2ACP) {
		set_board_type(ATHENA2ACP);
		acp_system_init();
		acp_board_init();
		/* I2C1 as Master */
		system_i2c1_init();
		system_adc_init();
		adc_init();
	} else {
//		set_board_type(UNKNOWN);
		/* Unknown board will be treated as EVB for now*/	//TODO:
		set_board_type(ATHENA2EVB);
		evb_system_init();
		evb_board_init();
	}
}
