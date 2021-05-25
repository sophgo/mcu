/*
 * kbd.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */
#include <libopencm3/stm32/gpio.h>
#include <i2c_slave.h>
#include <pin.h>
#include <tick.h>

#define KEY_PORT_COUNT	256
/*
#define KEY_PORT_WIDTH	8
#define KEY_COUNT	(KEY_PORT_COUNT * KEY_PORT_WIDTH)
*/

/* virtual key define */
#define POWER_KEY_PORT		0
#define POWER_KEY		(1 << 0)

#define REBOOT_KEY_PORT		0
#define REBOOT_KEY		(1 << 1)

#define FACTORY_RESET_KEY_PORT	0
#define FACTORY_RESET_KEY	(1 << 2)

/* count in ms */
#define CHECK_INTERVAL		(100)
#define CHECK_TIME2COUNTER(t)	((t) * 1000 / CHECK_INTERVAL)
#define REBOOT_DELAY		(CHECK_TIME2COUNTER(1))
#define FACTORY_RESET_DELAY	(CHECK_TIME2COUNTER(12))

static struct kbd_ctx {
	int set_idx;
	int idx;
	uint8_t map[KEY_PORT_COUNT];
} kbd_ctx;

static void kbd_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		kbd_ctx.set_idx = 1;
}

static void kbd_write(void *priv, uint8_t data)
{
	if (kbd_ctx.set_idx) {
		kbd_ctx.idx = data;
		kbd_ctx.set_idx = 0;
		return;
	}
	kbd_ctx.map[kbd_ctx.idx++] &= ~data;
}

static uint8_t kbd_read(void *priv)
{
	return kbd_ctx.map[kbd_ctx.idx++];
}

static struct i2c_slave_op slave = {
	.addr = 0x6d,	/* kbd common slave address */
	.match = kbd_match,
	.write = kbd_write,
	.read = kbd_read,
};

static int reset_key_pressed;

void kbd_set(int port, int key)
{
	kbd_ctx.map[port & 0xff] |= key;
}

void kbd_isr(void)
{
	/* get reset key status, effect low */
	if (gpio_get(RESET_KEY_PORT, RESET_KEY_PIN)) {
		/* reset key is not pressed */
		if (reset_key_pressed > REBOOT_DELAY)
			kbd_set(REBOOT_KEY_PORT, REBOOT_KEY);
		reset_key_pressed = 0;
		return;
	}

	++reset_key_pressed;

	if (reset_key_pressed > FACTORY_RESET_DELAY) {
		kbd_set(FACTORY_RESET_KEY_PORT, FACTORY_RESET_KEY);
		reset_key_pressed = 0;
	}
}

void kbd_init(struct i2c_slave_ctx *i2c)
{
	i2c_slave_register(i2c, &slave);
	tick_register_task(kbd_isr, CHECK_INTERVAL);
}

