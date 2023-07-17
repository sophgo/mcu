/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <adc.h>
#include <system/system.h>
#include <math.h>
#include <mcu.h>


#define DDR_TYPE					UNDEFINED_A2_DDR_TYPE

#define ATHENA2EVB_PCB_VERSION		4
#define ATHENA2ACP_PCB_VERSION		2


static uint8_t board_type;
static int board_temp_ntc1, board_temp_ntc2, soc_temp;


int get_board_temp_ntc1(void)
{
	int adc_local = adc_average_tab[0].value;

	if (adc_local < 40) {
		return -50;
	}
	else if (adc_local > 2750) {
		return 45;
	} else {
		board_temp_ntc1 =  (int) (4390 / (4390 / 298.15 + log((float)4096 / adc_local - 1) ) - 273.15);
		return board_temp_ntc1;
	}
}

void set_board_temp_ntc1(int temp)
{
	board_temp_ntc1 = temp;
}

int get_board_temp_ntc2(void)
{
	int adc_local = adc_average_tab[1].value;

	if (adc_local < 40) {
		return -50;
	}
	else if (adc_local > 2750) {
		return 45;
	} else {
		board_temp_ntc2 =  (int) (4390 / (4390 / 298.15 + log((float)4096 / adc_local - 1) ) - 273.15);
		return board_temp_ntc2;
	}
}


void set_board_temp_ntc2(int temp)
{
	board_temp_ntc2 = temp;
}

int get_soc_temp(void)
{
	return soc_temp;
}

void set_soc_temp(int temp)
{
	soc_temp = temp;
}

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
