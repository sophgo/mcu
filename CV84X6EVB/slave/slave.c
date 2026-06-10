/*
 * slave.c: I2C1 slave 初始化，只注册 mcu 从设备
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <gd32e50x_misc.h>
#include <slave/slave.h>
#include <i2c/i2c01_slave/i2c01_slave.h>
#include <i2c-slaves/mcu/mcu.h>
#include <common/common.h>

static struct i2c01_slave_ctx i2c1_slave_ctx;

void slave_init(void)
{
	i2c01_slave_init(&i2c1_slave_ctx, (void *)I2C1, MCU_SLAVE_ADDR, 0);
	mcu_init(&i2c1_slave_ctx);
	i2c01_slave_start(&i2c1_slave_ctx);
	nvic_irq_enable(I2C1_EV_IRQn, 0, 0);
}

void I2C1_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c1_slave_ctx);
}
