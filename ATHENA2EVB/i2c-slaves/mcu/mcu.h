/*
 * mcu.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __MCU_H__
#define __MCU_H__

#include <i2c/i2c01_slave/i2c01_slave.h>
#include <common/common.h>

#define CRITICAL_ACTION_REBOOT		1
#define CRITICAL_ACTION_POWER_DOWN	2

#define PRINT_OUTPUT_INTERVAL		1000

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void mcu_process(void);
uint8_t get_critical_action(void);
uint8_t get_critical_temp(void);
uint8_t get_repower_on_temp(void);

#endif /* __MCU_H__ */
