/*
 * kbd.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */
#include <i2c_slave.h>

#define KEY_PORT_COUNT	256
/*
#define KEY_PORT_WIDTH	8
#define KEY_COUNT	(KEY_PORT_COUNT * KEY_PORT_WIDTH)
*/

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

void kbd_init(struct i2c_slave_ctx *i2c)
{
	i2c_slave_register(i2c, &slave);
}


void kbd_set(int port, int key)
{
	kbd_ctx.map[port & 0xff] |= key;
}

