/*
 * i2c.h
 *
 *  Created on: Apr 23, 2019
 *      Author: weic
 */

#ifndef STM32L0XX_HAL_DRIVER_SRC_I2C_H_
#define STM32L0XX_HAL_DRIVER_SRC_I2C_H_

#include <stdint.h>

#define I2C_SLAVE_MAX	(4)

#define I2C_SLAVE_WRITE	(0)
#define I2C_SLAVE_READ	(1)

struct i2c_slave_return {
	unsigned int size;
	void *buf;
};

typedef uint8_t (*i2c_slave_read)(void);
typedef void (*i2c_slave_write)(uint8_t data);
typedef void (*i2c_slave_match)(int dir);
typedef void (*i2c_slave_stop)(void);

struct i2c_slave_op {
	uint16_t addr;
	i2c_slave_match match;
	i2c_slave_write	write;
	i2c_slave_read	read;
	i2c_slave_stop	stop;
};


int i2c_init(void *reg);
int i2c_slave_register(struct i2c_slave_op *slave);
int i2c_slave_start(void);

#endif /* STM32L0XX_HAL_DRIVER_SRC_I2C_H_ */
