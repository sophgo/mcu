/*
 * mcu.c
 *
 *  Created on: Apr 25, 2019
 *      Author: Chao Wei
 */

#include <libopencm3/stm32/gpio.h>
#include <assert.h>
#include <string.h>
#include <i2c_slave.h>
#include <upgrade.h>
#include <info.h>
#include <project_id.h>

#define MCU_REG_MAX 0x64
#define MCU_SW_VER 0;
unsigned char MCU_HW_VER;

#define REG_PROJECT	0x00
#define REG_SW_VER	0x01
#define REG_HW_VER	0x02
#define REG_CMD_REG	0x03
#define REG_STAGE	0x3c

extern struct i2c_slave_ctx i2c1_slave_ctx;

struct mcu_ctx {
	int set_idx;
	int idx;
};

static struct mcu_ctx mcu_ctx;

static inline void idx_set(struct mcu_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % MCU_REG_MAX;
}

static inline void idx_inc(struct mcu_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % MCU_REG_MAX;
}

static void mcu_match(void *priv, int dir)
{
	struct mcu_ctx *ctx = priv;

	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
}

static void mcu_write(void *priv, volatile uint8_t data)
{
	struct mcu_ctx *ctx = priv;

	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}

	switch (ctx->idx) {
	case REG_CMD_REG:
		break;
	default:
		break;
	}

	idx_inc(ctx);
}

static uint8_t mcu_read(void *priv)
{
	struct mcu_ctx *ctx = priv;
	uint8_t ret = 0;

	switch (ctx->idx) {
	case REG_PROJECT:
		ret = PROJ_SC5H;
		break;
	case REG_SW_VER:
		ret = MCU_SW_VER;
		break;
	case REG_HW_VER:
		ret = MCU_HW_VER;
		break;
	case REG_STAGE:
		ret = stage;
		break;
	default:
		ret = 0xff;
		break;
	}

	idx_inc(ctx);
	return ret;
}

static void mcu_reset(void *priv)
{
	struct mcu_ctx *ctx = priv;
	memset(ctx, 0x00, sizeof(*ctx));
}

static struct i2c_slave_op slave = {
	.addr = 0x17,	/* mcu common slave address */
	.mask = 0x00,
	.match = mcu_match,
	.write = mcu_write,
	.read = mcu_read,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_init(void)
{
	i2c_slave_register(&i2c1_slave_ctx, &slave);

	gpio_mode_setup(GPIOC,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			GPIO14 | GPIO15);

	gpio_mode_setup(GPIOH,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			GPIO0 | GPIO1);

	MCU_HW_VER = (gpio_port_read(GPIOC) >> 14) & 3;
	MCU_HW_VER |= (gpio_port_read(GPIOH) & 3) << 4;
}

