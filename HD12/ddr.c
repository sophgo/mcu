#include <stdint.h>
#include <adc.h>
#include <ddr.h>
#include <project.h>

static int ddr_size;

//void ddr_size_init(int size)
//{
//	ddr_size = size;
//}

void ddr_size_init(void)
{
	uint8_t pcb_ver = get_pcb_ver();

	switch (pcb_ver) {
	case PCB_VER_V1_0:
		ddr_size = DDR_SIZE_2R_128G;
		break;
	case PCB_VER_V2_0:
		ddr_size = DDR_SIZE_1R_32G;
		break;
	default:
		ddr_size = DDR_SIZE_2R_128G;
		break;
	}
}

int get_ddr_size()
{
	return ddr_size;
}
