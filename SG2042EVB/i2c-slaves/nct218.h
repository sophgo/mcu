#ifndef __NCT218_H__
#define __NCT218_H__
#include <i2c01_slave.h>

void nct218_init(void);
void nct218_get_temp(int *board, int *soc);
void nct218_process(void);

#endif
