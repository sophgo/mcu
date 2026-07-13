/*
 * ct7451.c: CT7451 温度传感器驱动 (I2C1 master)
 *
 * 12-bit 温度格式: 高字节(0x00/0x01) + 低字节(0x15/0x10), 0.0625°C/LSB
 * 本地温度存储在 ct7451_local_temp (单位: 0.01°C)
 * 远程温度存储在 ct7451_remote_temp (单位: 0.01°C)
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <i2c/i2c_master/i2c_master.h>
#include <ct7451/ct7451.h>
#include <tick/tick.h>
#include <loop/loop.h>
#include <debug.h>
#include <stdlib.h>

int ct7451_remote_temp;
int ct7451_local_temp;

#define CT7451_REG_LOCAL_HI		0x00
#define CT7451_REG_REMOTE_HI		0x01
#define CT7451_REG_LOCAL_LO		0x15
#define CT7451_REG_REMOTE_LO		0x10

/*
 * 12-bit → 0.01°C: raw * 0.0625 * 100 = raw * 25 / 4
 * 四舍五入: (raw * 25 + 2) / 4
 */
static int temp_raw_to_centi(int raw12)
{
	return (raw12 * 25 + 2) / 4;
}

void ct7451_init(void)
{
	loop_add(ct7451_process);
	debug("ct7451: init done (I2C1, addr=0x%02X)\n", CT7451_SLAVE_ADDR);
}

void ct7451_process(void)
{
	static unsigned long last_read;
	unsigned long now = tick_get();
	int timeout = 5;
	unsigned char hi, lo;
	int raw;

	if (now - last_read < 2000)
		return;
	last_read = now;

	/* 读本地温度 (12-bit: 0x00 + 0x15[7:4]) */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_LOCAL_HI, &hi) == 0 &&
	    i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_LOCAL_LO, &lo) == 0) {
		raw = ((int)(int8_t)hi << 4) | (lo >> 4);
		ct7451_local_temp = temp_raw_to_centi(raw);
	}

	/* 读远程温度 (12-bit: 0x01 + 0x10[7:4]) */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_REMOTE_HI, &hi) == 0 &&
	    i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_REMOTE_LO, &lo) == 0) {
		raw = ((int)(int8_t)hi << 4) | (lo >> 4);
		ct7451_remote_temp = temp_raw_to_centi(raw);
	}
}
