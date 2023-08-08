/*
 * mcu.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <i2c/i2c01_slave/i2c01_slave.h>
#include <common/common.h>

#define CRITICAL_ACTION_POWER_DOWN	2
#define CRITICAL_ACTION_REBOOT		1

#define POWER_OFF_REASON_POWER_OFF	0x80
#define POWER_OFF_REASON_RESET		0x81
#define POWER_OFF_REASON_REBOOT		0x82
#define POWER_OFF_REASON_OVER_HEAT	0x83

void mcu_a2evb_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void mcu_process(void);
uint8_t get_critical_action(void);
uint8_t get_critical_temp(void);
uint8_t get_repower_on_temp(void);
