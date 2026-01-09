#ifndef __raa228234_H__
#define __raa228234_H__

enum {
	raa228234B1 = 0,
	raa228234B2,
	raa228234B3,
	raa228234MAX,
};

unsigned long raa228234_output_voltage(int idx, int page);
unsigned long raa228234_output_current(int idx, int page);
unsigned long raa228234_output_power(int idx, int page);
int raa228234_set_out_voltage(int idx, int page, int voltage);
int raa228234_set_out_droop(int idx, int page, int resistance);
unsigned long raa228234_out_droop(int idx, int page);
void raa228234_init(void);
#endif
