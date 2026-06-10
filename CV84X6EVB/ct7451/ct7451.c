/*
 * ct7451.c: CT7451 温度传感器驱动 (I2C0 master)
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <i2c/i2c_master/i2c_master.h>
#include <ct7451/ct7451.h>
#include <tick/tick.h>
#include <loop/loop.h>
#include <debug.h>

int ct7451_remote_temp;
int ct7451_local_temp;

#define CT7451_REG_LOCAL		0x00
#define CT7451_REG_REMOTE		0x01

void ct7451_init(void)
{
	loop_add(ct7451_process);
	debug("ct7451: init done (I2C0, addr=0x%02X)\n", CT7451_SLAVE_ADDR);
}

void ct7451_process(void)
{
	static unsigned long last_read;
	unsigned long now = tick_get();
	int timeout = 5;
	unsigned char data;

	if (now - last_read < 2000)
		return;
	last_read = now;

	/* 读本地温度 (内部传感器) */
	if (i2c_master_smbus_read_byte(I2C0, CT7451_SLAVE_ADDR, timeout, CT7451_REG_LOCAL, &data) == 0)
		ct7451_local_temp = (int)data;

	/* 读远程温度 (外部二极管) */
	if (i2c_master_smbus_read_byte(I2C0, CT7451_SLAVE_ADDR, timeout, CT7451_REG_REMOTE, &data) == 0)
		ct7451_remote_temp = (int)data;
}
