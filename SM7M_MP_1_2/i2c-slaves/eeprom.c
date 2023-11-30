/*
 * eeprom.c
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */
#include <i2c_slave.h>
#include <common.h>
#include <loop.h>
#include <flash.h>
#include <string.h>
#include <stdbool.h>

#define EEPROM_BUFFER_SIZE	(128)

struct write_request_queue {
	/* do not write eeprom during i2c translation */
	int lock;
	int len;
	struct {
		uint16_t offset;
		uint8_t data;
	} cell[EEPROM_BUFFER_SIZE];
};

static struct eeprom_ctx {
	int set_idx;
	int idx;
	unsigned char tmp;

	struct write_request_queue wrq;
} ctx;

static inline void eeprom_spin_lock(void)
{
	__disable_irq();
}

static inline void eeprom_spin_unlock(void)
{
	__enable_irq();
}

/* stm32 compatible */

uint8_t eeprom_read_byte(uint32_t offset)
{
	return *(((uint8_t *)EEPROM_BASE) + offset);
}

void eeprom_flush_data_unlock(void)
{
	struct write_request_queue *wrq = &ctx.wrq;
	int i;
	uint32_t buf[EEPROM_SIZE / 4];

	if (wrq->len == 0)
		return;

	/* load page data from program flash */
	memcpy(buf, (void *)EEPROM_BASE, EEPROM_SIZE);

	/* modify loaded data */
	for (i = 0; i < wrq->len; ++i)
		((uint8_t *)buf)[wrq->cell[i].offset] = wrq->cell[i].data;

	/* restore loaded data */
	flash_program_page(EEPROM_BASE, buf, EEPROM_SIZE);

	wrq->len = 0;
}

void eeprom_write_byte(uint32_t offset, uint8_t data)
{
	struct write_request_queue *wrq = &ctx.wrq;

	/* push data into write request queue */
	eeprom_spin_lock();
	if (wrq->len >= EEPROM_BUFFER_SIZE)
		eeprom_flush_data_unlock();

	wrq->cell[wrq->len].offset = offset;
	wrq->cell[wrq->len].data = data;
	++wrq->len;
	eeprom_spin_unlock();
}

void eeprom_flush_data(void)
{
	if (ctx.wrq.len == 0 || ctx.wrq.lock)
		return;

	eeprom_spin_lock();
	eeprom_flush_data_unlock();
	eeprom_spin_unlock();
}

static void eeprom_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		ctx.set_idx = 2;
	}
	ctx.wrq.lock = true;
}

static void eeprom_stop(void *priv)
{
	ctx.wrq.lock = false;
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
	.stop = eeprom_stop,
	.write = eeprom_write,
	.read = eeprom_read,
};

void eeprom_init(struct i2c_slave_ctx *i2c)
{
	loop_add(eeprom_flush_data);
	i2c_slave_register(i2c, &slave);
}

void eeprom_log_power_off_reason(int reason)
{
	uint8_t tmp = reason;

	eeprom_write_byte(EEPROM_POWER_OFF_REASON_OFFSET, tmp);
}

