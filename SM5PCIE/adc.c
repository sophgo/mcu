#include <stdint.h>

struct adc_reg {
	uint32_t isr;
	uint32_t ier;
	uint32_t cr;
	uint32_t cfgr1;
	uint32_t cfgr2;
	uint32_t smpr;
	uint32_t r0[2];
	uint32_t tr;
	uint32_t chselr;
	uint32_t r1[5];
	uint32_t dr;
};
