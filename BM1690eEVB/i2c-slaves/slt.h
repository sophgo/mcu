#ifndef __SLT_H__
#define __SLT_H__
#include <i2c01_slave.h>

void slt_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void slt_reset(void);
uint16_t get_slt_result(int num);
#endif