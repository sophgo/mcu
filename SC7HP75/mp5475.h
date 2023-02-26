#ifndef __MP5475_H__
#define __MP5475_H__

enum {
	MP5475B1 = 0,
	MP5475B2,
	MP5475B3,
	MP5475MAX,
};

int mp5475_init(void);
void mp5475_destroy(void);
int mp5475_buck_on(int idx, unsigned int buck);
void mp5475_buck_off(int idx, unsigned int buck);
int mp5475_voltage_config(int idx, unsigned int buck, unsigned int voltage);
unsigned long mp5475_output_current(int idx, unsigned int buck);
unsigned long mp5475_output_voltage(int idx, unsigned int buck);

#endif
