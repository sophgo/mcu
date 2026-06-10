/*
 * pmbus.c: PMBus (SMBus 1.1 over I2C0) 简易实现
 *
 * PMBus 基于 SMBus，使用 I2C0 master 通信
 * 默认目标地址: 0x40, VOUT_MODE exponent = -9 (1/512 mV)
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <i2c/i2c_master/i2c_master.h>
#include <pmbus/pmbus.h>
#include <debug.h>

#define TIMEOUT		10

static uint8_t pmbus_def_addr = 0x40;	/* 默认 PMBus PMIC 地址 */

void pmbus_set_addr(uint8_t addr)
{
	pmbus_def_addr = addr;
}

void pmbus_read_word(uint8_t addr, uint8_t cmd_reg, uint16_t *val)
{
	i2c_master_smbus_read_word(I2C0, addr, TIMEOUT, cmd_reg, val);
}

void pmbus_write_word(uint8_t addr, uint8_t cmd_reg, uint16_t val)
{
	i2c_master_smbus_write_word(I2C0, addr, TIMEOUT, cmd_reg, val);
}

void pmbus_read_string(uint8_t addr, uint8_t cmd_reg, char *buf, int len)
{
	uint8_t cmd = cmd_reg;
	i2c_master_trans(I2C0, addr, TIMEOUT, &cmd, 1, (uint8_t *)buf, len - 1);
	buf[len - 1] = '\0';
}

/* ================================================================
 * VOUT 读写 (默认地址)
 *
 * VOUT = VOUT_COMMAND * 1000 / 512  (exponent = -9, 即 /512)
 * VOUT_COMMAND = VOUT * 512 / 1000
 * ================================================================ */
unsigned int pmbus_get_voltage(void)
{
	uint16_t raw;

	if (i2c_master_smbus_read_word(I2C0, pmbus_def_addr, TIMEOUT,
				       PMBUS_VOUT_CMD, &raw) != 0)
		return 0;

	return (unsigned int)raw * 1000 / 512;
}

void pmbus_set_voltage(unsigned int mv)
{
	uint16_t raw;

	/* Linear format: VOUT_COMMAND = mV * 512 / 1000 */
	raw = (uint16_t)((unsigned long)mv * 512 / 1000);
	i2c_master_smbus_write_word(I2C0, pmbus_def_addr, TIMEOUT,
				    PMBUS_VOUT_CMD, raw);
}