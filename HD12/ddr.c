#include <stdint.h>
#include <adc.h>
#include <ddr.h>
#include <project.h>
#include "system.h"

static int ddr_size;


void ddr_size_init(void)
{
	switch (get_hardware_version()) {
	case HW_VER_V1_0:
		dbg_printf("[%s] ddr size 128G\n", __func__);
		ddr_size = DDR_SIZE_2R_128G;
		break;
	case HW_VER_V1_1:
		dbg_printf("[%s] ddr size 64G\n", __func__);
		ddr_size = DDR_SIZE_2R_64G;
		break;
	case HW_VER_V2_0:
		dbg_printf("[%s] ddr size 32G\n", __func__);
		ddr_size = DDR_SIZE_1R_32G;
		break;
	default:
		ddr_size = DDR_SIZE_2R_64G;
		break;
	}
}

int get_ddr_size()
{
	return ddr_size;
}
