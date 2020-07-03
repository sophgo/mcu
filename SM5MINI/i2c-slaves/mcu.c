#include <string.h>
#include <project.h>
#include <i2c_slave.h>

#define MCU_REG_MAX	0x64
#define MCU_SW_VER	4
unsigned char MCU_HW_VER;

#define REG_PROJECT	0x00
#define REG_SW_VER	0x01
#define REG_HW_VER	0x02
#define REG_CMD_REG	0x03
#define REG_STAGE	0x3c

#define REG_VOLTAGE	0x26
#define REG_CURRENT	0x28


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

void mcu_cmd_process(void)
{
	switch (mcu_ctx.cmd) {
		case 8:
			// i2c_upgrade_start();
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
		ret = SM5MINI;
		break;
	case REG_SW_VER:
		ret = MCU_SW_VER;
		break;
	case REG_HW_VER:
		ret = MCU_HW_VER;
		break;
#if 0
	case REG_STAGE:
		ret = get_stage();
		break;
#endif
	case REG_CURRENT:
		ret = ctx->current & 0xff;
		break;
	case REG_CURRENT + 1:
		ret = (ctx->current >> 8) & 0xff;
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

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	i2c_slave_register(i2c_slave_ctx, &slave);
}

