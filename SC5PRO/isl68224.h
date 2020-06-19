#ifndef __ISL68224_H__
#define __ISL68224_H__

enum {
	ISL68224B12 = 0,
	ISL68224B34,
	ISL68224B56,
	ISL68224B78,
	ISL68224MAX,
};

unsigned long isl68224_output_voltage(int idx, int page);
unsigned long isl68224_output_current(int idx, int page);
unsigned long isl68224_output_power(int idx, int page);

#endif
