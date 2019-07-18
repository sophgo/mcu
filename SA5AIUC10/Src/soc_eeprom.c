/*
 * soc_eeprom.c
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */
#include <assert.h>
#include "i2c_bm.h"
#include "eeprom.h"

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
	EEPROM_WriteBytes(ctx.idx, &data, 1);
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

static struct i2c_slave_op slave3 = {
	.addr = 0x6a,
	.match = eeprom_match,
	.write = eeprom_write,
	.read = eeprom_read,
	.stop = eeprom_stop,
};

void eeprom_init(void)
{
	i2c_slave_register(&slave, i2c_ctx0);
	i2c_slave_register(&slave3,i2c_ctx3);
}

