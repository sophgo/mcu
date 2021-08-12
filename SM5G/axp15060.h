#ifndef __AXP15060_H__
#define __AXP15060_H__

enum axp15060_channel {
	AXP15060_DCDC1 = 0,
	AXP15060_DCDC2,
	AXP15060_DCDC3,
	AXP15060_DCDC4,
	AXP15060_DCDC5,
	AXP15060_DCDC6,

	AXP15060_ALDO1,
	AXP15060_ALDO2,
	AXP15060_ALDO3,
	AXP15060_ALDO4,
	AXP15060_ALDO5,

	AXP15060_BLDO1,
	AXP15060_BLDO2,
	AXP15060_BLDO3,
	AXP15060_BLDO4,
	AXP15060_BLDO5,

	AXP15060_CLDO1,
	AXP15060_CLDO2,
	AXP15060_CLDO3,
	AXP15060_CLDO4,

	/* AXP15060_CPUSLDO, */
	/* AXP15060_SWITCH, */
};

int axp15060_init(void);
void axp15060_destroy(void);
int axp15060_channel_on(unsigned int channel);
void axp15060_channel_off(unsigned int channel);
int axp15060_voltage_config(unsigned int channel, unsigned int voltage);
unsigned long axp15060_output_current(unsigned int channel);
unsigned long axp15060_output_voltage(unsigned int channel);

#endif
