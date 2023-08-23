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

#define DDR_TYPE		UNDEFINED_A2_DDR_TYPE

static uint8_t board_type;
static int soc_temp, board_temp, board_temp_ntc1, board_temp_ntc2;

int get_soc_temp(void)
{
	return soc_temp;
}

void set_soc_temp(int temp)
{
	soc_temp = temp;
}

int get_board_temp(void)
{
	return board_temp;
}

void set_board_temp(int temp)
{
	board_temp = temp;
}

int get_board_temp_ntc1(void)
{
	int adc_local = adc_average_tab[0].value;
	int board_temp_ntc1_raw;
	/* Raw temperature date with error */
	board_temp_ntc1_raw =  (int) (4390 / (4390 / 298.15 - logf(
		(float)4096 / adc_local - 1) ) - 273.15);
	/* Compensation to get the real temperature */
	board_temp_ntc1 = (board_temp_ntc1_raw / 8.1 - 1) * 10 + 30;
	return board_temp_ntc1;
}

void set_board_temp_ntc1(int temp)
{
	board_temp_ntc1 = temp;
}

int get_board_temp_ntc2(void)
{
	int adc_local = adc_average_tab[1].value;
	int board_temp_ntc2_raw;
	/* Raw temperature date with error */
	board_temp_ntc2_raw =  (int) (4390 / (4390 / 298.15 - logf(
		(float)4096 / adc_local - 1) ) - 273.15);
	/* Compensation to get the real temperature */
	board_temp_ntc2 = ((board_temp_ntc2_raw - 2) / 8.1 - 1) * 10 + 30;
	return board_temp_ntc2;
}

//void set_board_temp_ntc2(int temp)
//{
//	board_temp_ntc2 = temp;
//}
//
//const static unsigned short tmp_table[] = {
//	28,31,34,38,42,46,50,55,60,66,72,78,85,92,100,108,117,126,136,
//	147,158,170,183,196,210,225,241,258,276,295,315,336,358,381,405,
//	430,456,483,511,540,571,603,636,670,705,741,778,817,857,898,940,
//	983,1027,1072,1118,1165,1212,1260,1309,1358,1408,1458,1509,1560,
//	1612,1664,1716,1768,1820,1872,1924,1976,2027,2078,2129,2179,2229,
//	2278,2327,2375,2422,2469,2515,2560,2604,2647,2690,2732,2773,2813,
//	2852,2890,2927,2963,2998,3033,3067,3100,3132,3163,3193,3222,3250,
//	3277,3304,3330,3355,3379,3403,3426,3448,3469,3490,3510,3529,3548,
//	3566,3583,3600,3616,3632,3647,3662,3676,3690,3703,3716,3728,3740,
//	3752,3763,3774,3784,3794,3804,3813,3822,3831,3840,3848,3856,3864,
//	3871,3878,3885,3892,3899,3905,3911,3917,3923,3928,3933,3938,3943,
//	3948,3953,3958,3962,3966,3970,3974,3978,3982,3986,3989,3992,3995,
//	3998,4001,4004,4007,4010
//};
//
//int get_board_temp_ntc1(void)
//{
//	int i = 0;
//	for (i = 0; i < ARRAY_SIZE(tmp_table); ) {
//		if (adc_average_tab[0].value > tmp_table[i])
//			i++;
//		else
//			break;
//	}
//
//	board_temp_ntc1 = i - 50;
//	return board_temp_ntc1;
//}
//
//int get_board_temp_ntc2(void)
//{
//	int i = 0;
//	for (i = 0; i < ARRAY_SIZE(tmp_table); ) {
//		if (adc_average_tab[1].value > tmp_table[i])
//			i++;
//		else
//			break;
//	}
//
//	board_temp_ntc2 = i - 50;
//	return board_temp_ntc2;
//}

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
        case ATHENA2ACP:
        	return "Athena2 ACP";
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
