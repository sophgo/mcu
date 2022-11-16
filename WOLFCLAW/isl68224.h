#ifndef __ISL68224_H__
#define __ISL68224_H__

// int i2c_bus_init(int bus, int addr);
unsigned int isl68224_get_nvm_slot_num(int i2c, int addr);
unsigned int isl68224_get_device_id(int i2c, int addr);
unsigned int isl68224_get_reversion_id(int i2c, int addr);
int isl68224_program(int i2c, int addr, char *name);

#endif