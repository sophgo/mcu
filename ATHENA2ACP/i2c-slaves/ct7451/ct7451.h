/*
 * ct7451.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __CT7451_H__
#define __CT7451_H__

#include <i2c/i2c_slave/i2c_slave.h>

void ct7451_init(void);
void ct7451_get_temp(int *board, int *soc);
void ct7451_process(void);

#endif /* CT7451_H */
