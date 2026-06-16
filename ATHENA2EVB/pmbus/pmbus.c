/*
 * pmbus.c: PMBus (SMBus 1.1 over I2C0) 简易实现
 *
 * PMBus 基于 SMBus，使用 I2C0 master 通信
 * 默认目标地址: 0x41
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gd32e50x_i2c.h>
#include <i2c/i2c_master/i2c_master.h>
#include <pmbus/pmbus.h>
#include <debug.h>

#define TIMEOUT		50

static uint8_t pmbus_def_addr = 0x41;	/* 默认 PMBus PMIC 7-bit 地址 */

void pmbus_set_addr(uint8_t addr)
{
	pmbus_def_addr = addr;
}

uint8_t pmbus_get_addr(void)
{
	return pmbus_def_addr;
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

/* Linear mode: VOUT_MODE[4:0] 为 5-bit 有符号指数 */
static int pmbus_linear_exponent(void)
{
	uint8_t mode;
	int exp;

	if (i2c_master_smbus_read_byte(I2C0, pmbus_def_addr, TIMEOUT,
				       PMBUS_VOUT_MODE, &mode) != 0)
		return -9;

	exp = (int)(mode & 0x1f);
	if (exp & 0x10)
		exp -= 32;

	return exp;
}

static unsigned int pmbus_raw_to_mv(uint16_t raw, int exp)
{
	if (exp >= 0)
		return (unsigned int)raw * (1U << exp) * 1000U;

	return (unsigned int)raw * 1000U / (1U << (unsigned int)(-exp));
}

int pmbus_read_vout_raw(uint16_t *raw)
{
	if (i2c_master_smbus_read_word(I2C0, pmbus_def_addr, TIMEOUT,
				       PMBUS_READ_VOUT, raw) == 0)
		return 0;

	/* 部分芯片只支持读 VOUT_COMMAND */
	return i2c_master_smbus_read_word(I2C0, pmbus_def_addr, TIMEOUT,
					  PMBUS_VOUT_CMD, raw);
}

int pmbus_get_voltage_mv(unsigned int *mv)
{
	uint16_t raw;
	int exp;

	if (!mv)
		return -1;

	if (pmbus_read_vout_raw(&raw) != 0)
		return -1;

	exp = pmbus_linear_exponent();
	*mv = pmbus_raw_to_mv(raw, exp);
	return 0;
}

unsigned int pmbus_get_voltage(void)
{
	unsigned int mv;

	if (pmbus_get_voltage_mv(&mv) != 0)
		return 0;

	return mv;
}

void pmbus_set_voltage(unsigned int mv)
{
	uint16_t raw;
	int exp;

	exp = pmbus_linear_exponent();
	if (exp >= 0)
		raw = (uint16_t)(mv / (1000U * (1U << exp)));
	else
		raw = (uint16_t)((unsigned long)mv * (1U << (unsigned int)(-exp)) / 1000U);

	i2c_master_smbus_write_word(I2C0, pmbus_def_addr, TIMEOUT,
				    PMBUS_VOUT_CMD, raw);
}
