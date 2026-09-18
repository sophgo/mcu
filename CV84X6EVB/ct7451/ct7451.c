/*
 * ct7451.c: CT7451 温度传感器驱动 (I2C1 master)
 *
 * 12-bit 温度格式: 高字节(0x00/0x01) + 低字节(0x15/0x10), 0.0625°C/LSB
 * RANGE=1 扩展格式: 编码值 = 实际温度 + 64°C, 量程 -64 ~ +191.9375°C
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
int ct7451_remote_temp_raw;
int ct7451_local_temp;

#define CT7451_REG_LOCAL_HI		0x00
#define CT7451_REG_REMOTE_HI		0x01
#define CT7451_REG_CONFIG_RD		0x03
#define CT7451_REG_CONFIG_WR		0x09
#define CT7451_REG_LOCAL_LO		0x15
#define CT7451_REG_REMOTE_LO		0x10

/* 配置寄存器 RANGE 位: 0 = 0~+127.9°C, 1 = -64~+191.9°C (编码值偏移+64°C) */
#define CT7451_CONFIG_RANGE		(1 << 2)
/* 扩展格式偏移 64°C, 0.01°C 单位 */
#define CT7451_EXTENDED_OFFSET		6400

/*
 * 12-bit → 0.01°C: raw * 0.0625 * 100 = raw * 25 / 4
 * 四舍五入: (raw * 25 + 2) / 4
 */
static int temp_raw_to_centi(int raw12)
{
	return (raw12 * 25 + 2) / 4;
}

/* remote 温度校准: Tsense = 0.9973 * T环温 - 7.686, 输入输出均为 0.01°C */
static int temp_calibrate(int centi)
{
	int product = centi * 9973;
	int div;

	if (centi >= 0)
		div = (product + 5000) / 10000;
	else
		div = (product - 5000) / 10000;

	return div - 769;
}

void ct7451_init(void)
{
	unsigned char cfg;

	/*
	 * 置 RANGE=1, 扩展量程 -64 ~ +191.9375°C, 支持 -45°C 低温测量
	 * 读-改-写, 保留其他配置位默认值
	 */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, 5,
				       CT7451_REG_CONFIG_RD, &cfg) == 0) {
		cfg |= CT7451_CONFIG_RANGE;
		i2c_master_smbus_write_byte(I2C1, CT7451_SLAVE_ADDR, 5,
					    CT7451_REG_CONFIG_WR, cfg);
		debug("ct7451: RANGE=1, extended range -64~+191.9C\n");
	} else {
		debug("ct7451: WARN: config read failed, RANGE stays default\n");
	}

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

	/* 确保 RANGE=1 始终生效 (CT7451 掉电后寄存器复位) */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_CONFIG_RD, &hi) == 0) {
		if (!(hi & CT7451_CONFIG_RANGE)) {
			hi |= CT7451_CONFIG_RANGE;
			i2c_master_smbus_write_byte(I2C1, CT7451_SLAVE_ADDR,
						    timeout,
						    CT7451_REG_CONFIG_WR, hi);
		}
	}

	/*
	 * 读本地温度 (12-bit: 0x00 + 0x15[7:4])
	 * 扩展格式高字节为无符号(0~191), 减 64°C 偏移得实际温度
	 */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_LOCAL_HI, &hi) == 0 &&
	    i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_LOCAL_LO, &lo) == 0) {
		raw = ((int)hi << 4) | (lo >> 4);
		ct7451_local_temp = temp_raw_to_centi(raw) - CT7451_EXTENDED_OFFSET;
	}

	/* 读远程温度 (12-bit: 0x01 + 0x10[7:4]) */
	if (i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_REMOTE_HI, &hi) == 0 &&
	    i2c_master_smbus_read_byte(I2C1, CT7451_SLAVE_ADDR, timeout,
				       CT7451_REG_REMOTE_LO, &lo) == 0) {
		raw = ((int)hi << 4) | (lo >> 4);
		ct7451_remote_temp_raw = temp_raw_to_centi(raw) - CT7451_EXTENDED_OFFSET;
		ct7451_remote_temp = temp_calibrate(ct7451_remote_temp_raw);
	}
}
