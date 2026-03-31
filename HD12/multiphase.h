#ifndef __multiphase_H__
#define __multiphase_H__

enum {
	MultiPhaseB1 = 0,
	MultiPhaseB2,
	MultiPhaseB3,
	MultiPhaseMAX,
};

unsigned long multiphase_output_voltage(int idx, int page);
unsigned long multiphase_output_current(int idx, int page);
unsigned long multiphase_output_power(int idx, int page);
int multiphase_set_out_voltage(int idx, int page, int voltage);
int multiphase_set_out_droop(int idx, int page, int resistance);
unsigned long multiphase_output_droop(int idx, int page);
int multiphase_read_reg(int idx, int page, int reg);
void multiphase_init(void);
#endif
