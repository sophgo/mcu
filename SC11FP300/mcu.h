#ifndef __MCU_H__
#define __MCU_H__

#include <i2c01_slave.h>
#include <i2c_slave.h>
#include <common.h>

#define CRITICAL_ACTION_POWERDOWN	2
#define CRITICAL_ACTION_REBOOT		1

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void mcu_milkv_init(struct i2c_slave_ctx *i2c_slave_ctx);
void mcu_process(void);
void current_print_func(void);
uint8_t get_critical_action(void);
uint8_t get_critical_temp(void);
uint8_t get_repoweron_temp(void);
void mcu_process(void);
unsigned int get_12v_power(void);
unsigned char get_12v_power_l(void);
unsigned char get_12v_power_h(void);

#endif

