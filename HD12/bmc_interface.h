#ifndef __BMC_INTERFACE_H__
#define __BMC_INTERFACE_H__

#include <i2c_slave.h>


void bmc_init(struct i2c_slave_ctx *i2c);
int get_chip_status(void);
void check_chip_status(void);
void reset_chip_status(void);

#endif