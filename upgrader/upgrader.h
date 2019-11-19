#ifndef __UPGRADER_H__
#define __UPGRADER_H__

#include <i2c_slave.h>

extern void upgrader_init(void);
void upgrader_set_stage(uint8_t stage);
void upgrader_calc_cksum(void);

#endif
