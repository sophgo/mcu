#ifndef __MCU_H__
#define __MCU_H__

#include <i2c_slave.h>

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx);
void mcu_cmd_process(void);

#endif
