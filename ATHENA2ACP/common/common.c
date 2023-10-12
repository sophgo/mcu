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
#include <adc/adc.h>
#include <system/system.h>
#include <math.h>
#include <i2c-slaves/mcu/mcu.h>
#include <debug.h>

#define NTC_COMPENSATION_SLOPE		1.08
#define NTC_COMPENSATION_INTERCEPT	17.5

static int temp_i2c_local, temp_i2c_remote, board_temp_ntc1, board_temp_ntc2;

/* Board Information */
uint8_t get_board_type(void)
{
	return BOARD_TYPE;
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

uint8_t get_ddr_type(void)
{
	return DDR_TYPE;
}

char *get_board_type_name(void)
{
	switch (get_board_type()) {
		case ATHENA2ACP:
			return "Athena2 ACP";
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

/* CT7451 Temperature Sensor */
int get_temp_i2c_local(void)
{
	return temp_i2c_local;
}

void set_temp_i2c_local(int temp)
{
	temp_i2c_local = temp;
}

int get_temp_i2c_remote(void)
{
	return temp_i2c_remote;
}

void set_temp_i2c_remote(int temp)
{
	temp_i2c_remote = temp;
}

/* NTC1 and NTC2 Thermistor */
int get_temp_board_ntc1(void)
{
	return board_temp_ntc1;
}

void set_board_temp_ntc1(int temp)
{
	board_temp_ntc1 = temp;
}

void update_temp_board_ntc1(void)
{
	unsigned short adc_local = adc_average_tab[0].value;
	float board_temp_ntc1_raw;
	/* Raw temperature data with error */
	board_temp_ntc1_raw =  (int) (4390 / (4390 / 298.15 - logf(
		4096 / (float)adc_local - 1) ) - 273.15);
	/* Compensation to get the real temperature */
	board_temp_ntc1 = (int)(NTC_COMPENSATION_SLOPE * board_temp_ntc1_raw
				+ NTC_COMPENSATION_INTERCEPT);
}

int get_temp_board_ntc2(void)
{
	return board_temp_ntc2;
}

void set_board_temp_ntc2(int temp)
{
	board_temp_ntc2 = temp;
}

void update_temp_board_ntc2(void)
{
	unsigned short adc_local = adc_average_tab[1].value;
	float board_temp_ntc2_raw;
	/* Raw temperature data with error */
	board_temp_ntc2_raw =  (int) (4390 / (4390 / 298.15 - logf(
		4096 / (float)adc_local - 1) ) - 273.15);
	/* Compensation to get the real temperature */
	board_temp_ntc2 = (int)(NTC_COMPENSATION_SLOPE * board_temp_ntc2_raw
				+ NTC_COMPENSATION_INTERCEPT);
}
