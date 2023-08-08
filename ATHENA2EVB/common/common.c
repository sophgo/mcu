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

#define DDR_TYPE		UNDEFINED_DDR_TYPE

static uint8_t board_type;

uint8_t get_ddr_type(void)
{
	return DDR_TYPE;
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

char *get_board_type_name(void)
{
	switch (board_type) {
	case ATHENA2EVB:
		return "Athena2 EVB";
	default:
		/* U means unknown type */
		return "Athena2-Unknown";
	}
}

void set_board_type(uint8_t type)
{
	board_type = type;
}

uint8_t get_board_type(void)
{
	return board_type;
}
