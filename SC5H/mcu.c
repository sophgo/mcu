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
#include <project_id.h>
#include <pin.h>
#include <adc.h>
#include <tick.h>

#define MCU_REG_MAX	0x64
#define MCU_SW_VER	5
unsigned char MCU_HW_VER;

#define REG_PROJECT	0x00
#define REG_SW_VER	0x01
#define REG_HW_VER	0x02
#define REG_CMD_REG	0x03
#define REG_VQPS	0x09
#define REG_STAGE	0x3c

#define REG_VOLTAGE	0x26
#define REG_CURRENT	0x28

extern struct i2c_slave_ctx i2c1_slave_ctx;

struct mcu_ctx {
	int set_idx;
	int idx;
	int cmd_tmp;
	uint8_t cmd;
	unsigned long current;
	unsigned long voltage;
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

#define FILTER_DATA_WIDTH	16
#define FILTER_DEPTH_SHIFT	8
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

#if FILTER_DATA_WIDTH == 16
typedef uint16_t filter_data_t;
#else
#error undefined filter width or unsupported filter width
#endif

struct filter {
	filter_data_t data[FILTER_DEPTH];
	unsigned long total;
	int p;
};

static filter_data_t filter_init(struct filter *f, filter_data_t d)
{
	int i;

	f->p = 0;
	f->total = 0;
	for (i = 0; i < FILTER_DEPTH; ++i) {
		f->data[i] = d;
		f->total += d;
	}
	return d;
}

static filter_data_t filter_in(struct filter *f, filter_data_t d)
{
	f->total -= f->data[f->p];
	f->total += d;
	f->data[f->p] = d;
	f->p = (f->p + 1) & FILTER_DEPTH_MASK;

	return f->total >> FILTER_DEPTH_SHIFT;
}

static struct filter current_filter;
static struct filter voltage_filter;

void mcu_cmd_process(void)
{
	static unsigned long last_tick = 0;
	unsigned long current_tick;
	unsigned long current, voltage;

	current_tick = tick_get();
	if (current_tick != last_tick) {
		adc_read(&current, &voltage);
		mcu_ctx.current = filter_in(&current_filter, current);
		mcu_ctx.voltage = filter_in(&voltage_filter, voltage);
		last_tick = current_tick;
	}

	switch (mcu_ctx.cmd) {
		case 8:
			i2c_upgrade_start();
			break;
		default:
			break;
	}
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
		ctx->cmd_tmp = data;
		break;
	case REG_VQPS:
		if (data)
			gpio_set(EN_VQPS_PORT, EN_VQPS_PIN);
		else
			gpio_clear(EN_VQPS_PORT, EN_VQPS_PIN);
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
	case REG_CMD_REG:
		ret = 0;
		break;
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
		ret = get_stage();
		break;
	case REG_VQPS:
		ret = gpio_get(EN_VQPS_PORT, EN_VQPS_PIN) ? 1 : 0;
		break;
	case REG_CURRENT:
		ret = ctx->current & 0xff;
		break;
	case REG_CURRENT + 1:
		ret = (ctx->current >> 8) & 0xff;
		break;
	case REG_VOLTAGE:
		ret = ctx->voltage & 0xff;
		break;
	case REG_VOLTAGE + 1:
		ret = (ctx->voltage >> 8) & 0xff;
		break;
	default:
		ret = 0xff;
		break;
	}

	idx_inc(ctx);
	return ret;
}

static void mcu_stop(void *priv)
{
	struct mcu_ctx *ctx = priv;
	ctx->cmd = ctx->cmd_tmp;
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
	.stop = mcu_stop,
	.reset = mcu_reset,
	.priv = &mcu_ctx,
};

void mcu_init(void)
{
	unsigned long current, voltage;
	unsigned char pcb_ver, bom_ver;

	i2c_slave_register(&i2c1_slave_ctx, &slave);

	gpio_mode_setup(GPIOC,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			GPIO14 | GPIO15);

	gpio_mode_setup(GPIOH,
			GPIO_MODE_INPUT,
			GPIO_PUPD_NONE,
			GPIO0 | GPIO1);

	pcb_ver = (gpio_port_read(GPIOC) >> 14) & 3;
	bom_ver = gpio_port_read(GPIOH) & 3;

	MCU_HW_VER = (pcb_ver << 4) | bom_ver;

	gpio_clear(EN_VQPS_PORT, EN_VQPS_PIN);
	gpio_mode_setup(EN_VQPS_PORT, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, EN_VQPS_PIN);


	adc_read(&current, &voltage);
	mcu_ctx.current = filter_init(&current_filter, current);
	mcu_ctx.voltage = filter_init(&voltage_filter, voltage);
}

