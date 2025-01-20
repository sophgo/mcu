/*
 * wdt.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */
#include <libopencmsis/core_cm3.h>
#include <stdio.h>
#include <string.h>
#include <i2c_slave.h>
#include <loop.h>
#include <chip.h>
#include <mcu.h>
#include <eeprom.h>
#include <tick.h>
#include <mcu.h>
#include <project.h>
#include <mcu-e2prom.h>

/* magic number for different type of watchdogs, no need now */
/* this byte is used to handle some different software implementations of wdt */
#define WDT_MAGIC_REG	(0)
/* write 1 to this register will start watchdog, 0 to stop */
/* read return register value */
#define WDT_ENABLE_REG	(1)
/* write to this register will result in reloading timeout to counter */
#define WDT_RELOAD_REG	(2)
/* offset 3 is reserved */
/* write to this register */
#define WDT_TIMEOUT_REG	(4)
#define WDT_COUNTER_REG	(8)
#define WDT_REG_MAX	(12)

static struct wdt_ctx {
	int set_idx;
	int idx;
	int enable;
	uint32_t timeout;	/* get from timer when addr match and read command */
	/* counter count down, ( from timeout to 0) */
	uint32_t counter;	/* get from timer when addr match and read command */
	uint32_t timeout_shadow;
	uint32_t counter_shadow;
	uint32_t clock;	/* real clock frequency */
} wdt_ctx;

static void wdt_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		wdt_ctx.set_idx = 1;
	} else {
		/* when read request received, update shadow register */
		wdt_ctx.timeout_shadow = wdt_ctx.timeout;
		wdt_ctx.counter_shadow = wdt_ctx.counter;
	}
}

static inline void idx_set(uint8_t idx)
{
	wdt_ctx.idx = idx % WDT_REG_MAX;
}
static inline void idx_inc(void)
{
	wdt_ctx.idx = (wdt_ctx.idx + 1) % WDT_REG_MAX;
}

static void wdt_write(void *priv, uint8_t data)
{
	int offset;

	if (wdt_ctx.set_idx) {
		idx_set(data);
		wdt_ctx.set_idx = 0;
		return;
	}
	switch (wdt_ctx.idx) {
	case WDT_ENABLE_REG:
		wdt_ctx.enable = data;
		break;
	case WDT_TIMEOUT_REG ... (WDT_TIMEOUT_REG + 3):
		offset = wdt_ctx.idx - WDT_TIMEOUT_REG;
		((uint8_t *)&(wdt_ctx.timeout_shadow))[offset] = data;
		break;
	case WDT_RELOAD_REG:
		wdt_ctx.counter = wdt_ctx.timeout;
		break;
	}
	idx_inc();
}

static uint8_t wdt_read(void *priv)
{
	uint8_t ret = 0;
	int offset;

	switch (wdt_ctx.idx) {
	case WDT_ENABLE_REG:
		ret = wdt_ctx.enable;
		break;
	case WDT_TIMEOUT_REG ... (WDT_TIMEOUT_REG + 3):
		offset = wdt_ctx.idx - WDT_TIMEOUT_REG;
		ret = ((uint8_t *)&(wdt_ctx.timeout_shadow))[offset];
		break;
	case WDT_COUNTER_REG ... (WDT_COUNTER_REG + 3):
		offset = wdt_ctx.idx - WDT_COUNTER_REG;
		ret = ((uint8_t *)&(wdt_ctx.counter_shadow))[offset];
		break;
	}
	idx_inc();
	return ret;
}

static void wdt_stop(void *priv)
{
	wdt_ctx.timeout = wdt_ctx.timeout_shadow;
}

static struct i2c_slave_op slave = {
	.addr = 0x69,	/* wdt common slave address */
	.match = wdt_match,
	.write = wdt_write,
	.read = wdt_read,
	.stop = wdt_stop,
};

void wdt_reset(void)
{
	__disable_irq();
	memset(&wdt_ctx, 0, sizeof(wdt_ctx));
//	wdt_ctx.clock = (37 * 1000) / 128;
	wdt_ctx.counter = wdt_ctx.counter_shadow =
		wdt_ctx.timeout = wdt_ctx.timeout_shadow = 0xffffffff;
	__enable_irq();
}

void wdt_process(void)
{
	if (wdt_ctx.enable && wdt_ctx.counter == 0) {
		chip_disable();
		mcu_eeprom_power_off_reason(EEPROM_POWER_OFF_REASON_WATCHDOG);
		// mcu_raise_interrupt(1, MCU_INT_WDT_RST);
		wdt_reset();	/* reset to initial state */
		// if (get_board_type() == SM5ME)
		// 	se5_reset_board();
		// else
			chip_reset();
	}
}

static void wdt_isr(void)
{
	if (wdt_ctx.enable && wdt_ctx.counter > 0)
		--wdt_ctx.counter;

	return;
}

void wdt_init(struct i2c_slave_ctx *i2c)
{
	wdt_reset();

	loop_add(wdt_process);
	tick_register_task(wdt_isr, 1000);
	i2c_slave_register(i2c, &slave);
}

