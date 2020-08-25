/*
 * soc_eeprom.c
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */
#include <assert.h>
#include "i2c_bm.h"
#include "eeprom.h"
#include "main.h"

#define EEPROM_SIZE	(EEPROM_BANK_SIZE)

static struct eeprom_ctx {
	int set_idx;
	int idx;
} ctx;

static void eeprom_match(int dir)
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
		EEPROM_WriteBytes(offset, &data, 1);
}
static void eeprom_write(uint8_t data)
{
	switch (ctx.set_idx) {
	case 2:
		idx_set_hi(data);
		ctx.set_idx = 1;
		return;
	case 1:
		idx_set_lo(data);
		ctx.set_idx = 0;
		return;
	default:
		break;
	}
	eeprom_write_byte_protected(ctx.idx, data);
	idx_inc();
}

static uint8_t eeprom_read(void)
{
	uint8_t tmp;
	EEPROM_ReadBytes(ctx.idx, &tmp, 1);
	idx_inc();
	return tmp;
}

static void eeprom_stop(void)
{
}

static struct i2c_slave_op slave = {
	.addr = 0x6a,
	.match = eeprom_match,
	.write = eeprom_write,
	.read = eeprom_read,
	.stop = eeprom_stop,
};

void eeprom_init(void)
{
	i2c_slave_register(&slave);
}

