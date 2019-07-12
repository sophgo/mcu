/*
 * i2c.h
 *
 *  Created on: Apr 23, 2019
 *      Author: weic
 */

#ifndef STM32L0XX_HAL_DRIVER_SRC_I2C_H_
#define STM32L0XX_HAL_DRIVER_SRC_I2C_H_

#include <stdint.h>
#include <main.h>

//#define I2C_SLAVE_MAX	(4)

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

typedef uint8_t (*i2c_slave_mcu_read)(I2C_CTX i2c_ctx);
typedef void (*i2c_slave_mcu_write)(uint8_t data,I2C_CTX i2c_ctx);
typedef void (*i2c_slave_mcu_match)(int dir,I2C_CTX i2c_ctx);
typedef void (*i2c_slave_mcu_stop)(I2C_CTX i2c_ctx);

struct i2c_slave_mcu_op {
	uint16_t addr;
	i2c_slave_mcu_match match;
	i2c_slave_mcu_write	write;
	i2c_slave_mcu_read	read;
	i2c_slave_mcu_stop	stop;
};


int i2c_init(void *reg,I2C_CTX i2c_ctx);
int i2c_slave_register(struct i2c_slave_op *slave,I2C_CTX i2c_ctx);
int i2c_slave_start(I2C_CTX i2c_ctx);

#endif /* STM32L0XX_HAL_DRIVER_SRC_I2C_H_ */
