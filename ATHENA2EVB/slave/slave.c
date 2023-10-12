/*
 * slave.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_i2c.h>
#include <gd32e50x.h>
#include <common/common.h>
#include <project.h>
#include <i2c/i2c01_slave/i2c01_slave.h>
#include <i2c-slaves/mcu/mcu.h>
#include <i2c-slaves/slt/slt.h>

#define I2C1_OA1		MCU_SLAVE_ADDR
#define I2C1_OA2		0x68
#define OA2_MASK		0x03

static struct i2c01_slave_ctx i2c1_slave_ctx;

void slave_init(void)
{
	/* I2C1 as Slave */
	i2c01_slave_init(&i2c1_slave_ctx, (void *)I2C1, I2C1_OA1, I2C1_OA2);
	mcu_init(&i2c1_slave_ctx);
	slt_init(&i2c1_slave_ctx);
	i2c01_slave_start(&i2c1_slave_ctx);
	nvic_irq_enable(I2C1_EV_IRQn, 0, 0);
}

/* replace default isr */
void I2C1_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c1_slave_ctx);
}
