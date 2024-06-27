#ifndef __MP5475_H__
#define __MP5475_H__

int mp5475_init(void);
void mp5475_destroy(void);
int mp5475_buck_on(unsigned int buck);
void mp5475_buck_off(unsigned int buck);
int mp5475_voltage_config(unsigned int buck, unsigned int voltage);
unsigned long mp5475_output_current(unsigned int buck);
unsigned long mp5475_output_voltage(unsigned int buck);
int mp5475_get_tmp(void);

#endif
