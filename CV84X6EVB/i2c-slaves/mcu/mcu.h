/*
 * mcu.h: CV84X6EVB 简化 mcu I2C 从设备
 */
#ifndef __MCU_H__
#define __MCU_H__

#include <i2c/i2c01_slave/i2c01_slave.h>

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void mcu_process(void);

#endif /* __MCU_H__ */
