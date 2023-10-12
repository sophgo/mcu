/*
 * common.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <stdint.h>
#include <project.h>
#include <common/common.h>
#include <pin.h>
#include <tick/tick.h>
#include <system/system.h>
#include <i2c-slaves/mcu/mcu.h>

uint8_t get_board_type(void)
{
	return BOARD_TYPE;
}

uint8_t get_pcb_version(void)
{
	return PCB_VERSION;
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

uint8_t get_ddr_type(void)
{
	return DDR_TYPE;
}

uint8_t get_bom_version(void)
{
	return BOM_VERSION;
}

char *get_board_type_name(void)
{
	switch (get_board_type()) {
	case ATHENA2EVB:
		return "Athena2 EVB";
	default:
		return "Athena2-Unknown";
	}
}

char *get_ddr_type_name(void)
{
	switch (get_ddr_type()) {
		case DDR4:
			return "Athena2 DDR4";
		case LP_DDR4:
			return "Athena2 LP_DDR4";
		default:
			return "Athena2-Unknown";
	}
}
