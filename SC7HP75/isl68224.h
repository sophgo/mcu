#ifndef __ISL68224_H__
#define __ISL68224_H__

enum {
	ISL68224B12 = 0,
	ISL68224B34,
	ISL68224MAX,
};

unsigned long isl68224_output_voltage(int idx, int page);
unsigned long isl68224_output_current(int idx, int page);
unsigned long isl68224_output_power(int idx, int page);
int isl68224_set_out_voltage(int idx, int page, int voltage);

#endif
