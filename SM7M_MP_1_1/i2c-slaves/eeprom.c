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

static volatile uint8_t lock = 1;
static volatile uint8_t unlock_code;

static void eeprom_match(void *priv, int dir)
{
	struct eeprom_ctx *ctx = (struct eeprom_ctx *)priv;

	if (dir == I2C_SLAVE_WRITE) {
		ctx->set_idx = 2;
	}
}

static inline void idx_set_lo(struct eeprom_ctx *ctx, uint8_t idx)
{
	ctx->idx |= idx;
	ctx->idx = ctx->idx % ctx->size;
}
static inline void idx_set_hi(struct eeprom_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx << 8;
}
static inline void idx_inc(struct eeprom_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % ctx->size;
}

static inline void eeprom_spin_lock(void)
{
	__disable_irq();
}

static inline void eeprom_spin_unlock(void)
{
	__enable_irq();
}

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

void eeprom_write_byte_protected(void *priv, uint16_t offset, uint8_t data)
{
	struct eeprom_ctx *ctx = (struct eeprom_ctx *)priv;
	if (!eeprom_get_lock_status())
		ctx->write_byte(priv, offset, data);
}

static void eeprom_write(void *priv, uint8_t data)
{
	struct eeprom_ctx *ctx = (struct eeprom_ctx *)priv;

	switch (ctx->set_idx) {
	case 2:
		ctx->tmp = data;
		ctx->set_idx = 1;
		return;
	case 1:
		idx_set_hi(ctx, ctx->tmp);
		idx_set_lo(ctx, data);
		ctx->set_idx = 0;
		return;
	default:
		break;
	}
	eeprom_write_byte_protected(priv, ctx->idx, data);
	idx_inc(ctx);
}

static uint8_t eeprom_read(void *priv)
{
	struct eeprom_ctx *ctx = (struct eeprom_ctx *)priv;
	uint8_t tmp;
	if (ctx->set_idx == 1)
	{
		idx_set_hi(ctx, 0);
		idx_set_lo(ctx, ctx->tmp);
	}
	tmp = ctx->read_byte(priv, ctx->idx);
	idx_inc(ctx);
	return tmp;
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

