/*
 * ct7451.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __CT7451_H__
#define __CT7451_H__

void ct7451_init(struct i2c_slave_ctx *i2c_slave_ctx);
void ct7451_get_temp(int *remote, int *local);
void ct7451_process(void);

#endif /* __CT7451_H__ */
