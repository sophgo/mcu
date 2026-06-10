/*
 * slave.h:
 */
#ifndef __SLAVE_H__
#define __SLAVE_H__

#include <i2c/i2c01_slave/i2c01_slave.h>

#define MCU_SLAVE_ADDR		0x17

void slave_init(void);

#endif /* __SLAVE_H__ */
