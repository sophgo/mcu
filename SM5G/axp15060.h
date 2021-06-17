#ifndef __AXP15060_H__
#define __AXP15060_H__

int axp15060_init(void);
void axp15060_destroy(void);
int axp15060_buck_on(unsigned int buck);
void axp15060_buck_off(unsigned int buck);
int axp15060_voltage_config(unsigned int buck, unsigned int voltage);
unsigned long axp15060_output_current(unsigned int buck);
unsigned long axp15060_output_voltage(unsigned int buck);

#endif
