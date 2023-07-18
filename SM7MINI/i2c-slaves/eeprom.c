/*
 * eeprom.c
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencmsis/core_cm3.h>
#include <i2c_slave.h>
#include <common.h>
#include <eeprom.h>

uint8_t eeprom_read_byte(uint32_t offset)
{
	return *((volatile uint8_t *)(EEPROM_BASE + offset));
}

void eeprom_write_byte(uint32_t offset, uint8_t data)
{
	eeprom_program_byte(EEPROM_BASE + offset, data);
}

static struct eeprom_ctx ctx;

static void eeprom_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		ctx.set_idx = 2;
	}
}

static inline void idx_set_lo(uint8_t idx)
{
	ctx.idx |= idx;
	ctx.idx = ctx.idx % EEPROM_SIZE;
}
static inline void idx_set_hi(uint8_t idx)
{
	ctx.idx = idx << 8;
}
static inline void idx_inc(void)
{
	ctx.idx = (ctx.idx + 1) % EEPROM_SIZE;
}

static inline void eeprom_spin_lock(void)
{
	__disable_irq();
}

static inline void eeprom_spin_unlock(void)
{
	__enable_irq();
}

static volatile uint8_t lock = 1;
static volatile uint8_t unlock_code;

int eeprom_get_lock_status(void)
{
	int tmp;

	eeprom_spin_lock();
	tmp = lock;
	eeprom_spin_unlock();

	return tmp;
}

void eeprom_lock_code(uint8_t code)
{
	eeprom_spin_lock();

	/* 'LO': lock
	 * 'CK': unlock
	 */

	/* begin of a sequence */
	if (code == 'L' || code == 'C')
		unlock_code = code;
	else {
		if (code == 'O' && unlock_code == 'L')
			lock = 1;
		else if (code == 'K' && unlock_code == 'C')
			lock = 0;

		unlock_code = 0;
	}

	eeprom_spin_unlock();
}

void eeprom_write_byte_protected(uint16_t offset, uint8_t data)
{
	if (!eeprom_get_lock_status())
		eeprom_write_byte(offset, data);
}

static void eeprom_write(void *priv, uint8_t data)
{
	switch (ctx.set_idx) {
	case 2:
		ctx.tmp = data;
		ctx.set_idx = 1;
		return;
	case 1:
		idx_set_hi(ctx.tmp);
		idx_set_lo(data);
		ctx.set_idx = 0;
		return;
	default:
		break;
	}
	eeprom_write_byte_protected(ctx.idx, data);
	idx_inc();
}

static uint8_t eeprom_read(void *priv)
{
	uint8_t tmp;
	if (ctx.set_idx == 1)
	{
		idx_set_hi(0);
		idx_set_lo(ctx.tmp);
	}
	tmp = eeprom_read_byte(ctx.idx);
	idx_inc();
	return tmp;
}

static struct i2c_slave_op slave = {
	.addr = 0x6a,
	.match = eeprom_match,
	.write = eeprom_write,
	.read = eeprom_read,
};

void eeprom_init(struct i2c_slave_ctx *i2c)
{
	i2c_slave_register(i2c, &slave);
}

void eeprom_log_power_off_reason(int reason)
{
	uint8_t tmp = reason;

	eeprom_write_byte(EEPROM_POWER_OFF_REASON_OFFSET, tmp);
}

int is_mixed_mode(void)
{
	return eeprom_read_byte(EEPROM_MIXED_MODE_OFFSET) == 1 ? 1 : 0;
}

int is_test_mode(void)
{
	return eeprom_read_byte(EEPROM_TEST_MODE_OFFSET) == 1 ? 1 : 0;
}

void eeprom_create(
	struct i2c_slave_ctx *i2c,
	struct eeprom_ctx *eeprom,
	struct i2c_slave_op *slave,
	int (*read)(void *, unsigned int),
	int (*write)(void *, unsigned int, unsigned char),
	unsigned int size
	)
{
	eeprom->read_byte = read;
	eeprom->write_byte = write;
	eeprom->size = size;
	slave->priv = eeprom;
	slave->match = eeprom_match;
	slave->read = eeprom_read;
	slave->write = eeprom_write;

	i2c_slave_register(i2c, slave);
}