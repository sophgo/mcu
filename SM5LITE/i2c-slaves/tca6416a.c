#include <libopencm3/cm3/nvic.h>
#include <i2c_slave.h>
#include <tca6416a.h>
#include <i2c_master.h>

#define TCA6416A_ADDR	0x20

#define TCA6416A_I2C	I2C2

/* in ms */
#define TCA6416A_SMBTO	(1)

static struct {
	int set_idx;
	int idx;
} gpioex_ctx;

static void tca6416a_i2c_slave_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		gpioex_ctx.set_idx = 1;
}

static void tca6416a_i2c_slave_write(void *priv, uint8_t data)
{
	if (gpioex_ctx.set_idx) {
		gpioex_ctx.idx = data & TCA6416A_REG_MASK;
		gpioex_ctx.set_idx = 0;
		return;
	}

	switch (gpioex_ctx.idx) {
	case TCA6416A_P0_IN:
	case TCA6416A_P1_IN:
	case TCA6416A_P0_POL:
	case TCA6416A_P1_POL:
	case TCA6416A_P0_CFG:
	case TCA6416A_P1_CFG:
		/* no effect and donot change pin config */
		return;
	case TCA6416A_P0_OUT:
	case TCA6416A_P1_OUT:
		/* output value is free for user */
		break;
	default:
		return;
	}

	gpioex_ctx.idx = (gpioex_ctx.idx + 1) & TCA6416A_REG_MASK;
}

static uint8_t tca6416a_i2c_slave_read(void *priv)
{
	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
	return 0;
}

static struct i2c_slave_op slave = {
	.addr = 0x6c,
	.match	= tca6416a_i2c_slave_match,
	.write	= tca6416a_i2c_slave_write,
	.read	= tca6416a_i2c_slave_read,
};

int tca6416a_init(struct i2c_slave_ctx *i2c)
{
	return i2c_slave_register(i2c, &slave);
}

