#include <libopencm3/cm3/nvic.h>
#include <i2c_master.h>
#include <string.h>
#include <project.h>
#include <i2c_slave.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <loop.h>
#include <chip.h>
#include <eeprom.h>
#include <power.h>
#include <se5.h>
#include <wdt.h>
#include <mcu.h>

#define SLT_REG_LEN 64
#define SLT_REG_MAX 64

struct slt_ctx {
	int set_idx;
	int idx;
	uint8_t reg[SLT_REG_LEN];
};

static struct slt_ctx slt_ctx;

static inline void idx_set(struct slt_ctx *ctx, uint8_t idx)
{
	ctx->idx = idx % SLT_REG_MAX;
}

static inline void idx_inc(struct slt_ctx *ctx)
{
	ctx->idx = (ctx->idx + 1) % SLT_REG_MAX;
}

static void slt_match(void *priv, int dir)
{
	struct slt_ctx *ctx = priv;

	if (dir == I2C_SLAVE_WRITE)
		ctx->set_idx = 1;
}

static void slt_write(void *priv, volatile uint8_t data)
{
	struct slt_ctx *ctx = priv;

	if (ctx->set_idx) {
		idx_set(ctx, data);
		ctx->set_idx = 0;
		return;
	}

    ctx->reg[ctx->idx] = data;

	idx_inc(ctx);
}

static uint8_t slt_read(void *priv)
{
	struct slt_ctx *ctx = priv;
	uint8_t ret = 0;

    ret = ctx->reg[ctx->idx];

	idx_inc(ctx);
	return ret;
}

static struct i2c_slave_op slave = {
	.addr = 0x6f,	/* mcu common slave address */
	.match = slt_match,
	.write = slt_write,
	.read = slt_read,
	.priv = &slt_ctx,
};

void slt_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	i2c_slave_register(i2c_slave_ctx, &slave);
}

void slt_reset(void)
{
	memset(&slt_ctx, 0, sizeof(struct slt_ctx));
}

uint16_t get_slt_result(void)
{
	return (slt_ctx.reg[1] << 8) | slt_ctx.reg[0];
}
