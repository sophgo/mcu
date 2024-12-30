#ifndef __ISL68224_H__
#define __ISL68224_H__

enum {
	ISL68224B1 = 0,
	ISL68224B2,
	ISL68224B3,
	ISL68224MAX,
};

unsigned long isl68224_output_voltage(int idx, int page);
unsigned long isl68224_output_current(int idx, int page);
unsigned long isl68224_output_power(int idx, int page);
int isl68224_set_out_voltage(int idx, int page, int voltage);
int isl68224_set_out_droop(int idx, int page, int resistance);
unsigned long isl68224_out_droop(int idx, int page);

#endif
