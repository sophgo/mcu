#include <libopencm3/cm3/nvic.h>
#include <i2c_slave.h>
#include <tca6416a.h>
#include <pin.h>

static struct {
	int set_idx;
	int idx;
	uint8_t port[2];
} gpioex_ctx;

const struct {
	int port, pin;
} tca6416a_vpin2gpio[16] = {
	[4] = {MCU_LED_PORT, MCU_LED_PIN},
	[11] = {HEALTHY_LED_PORT, HEALTHY_LED_PIN},
};

static void tca6416a_i2c_slave_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		gpioex_ctx.set_idx = 1;
}

static void tca6416a_i2c_slave_write(void *priv, uint8_t data)
{
	uint16_t vpin;
	int i;

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
		break;
	case TCA6416A_P0_OUT:
		gpioex_ctx.port[0] = data;
		break;
	case TCA6416A_P1_OUT:
		gpioex_ctx.port[1] = data;
		break;
	default:
		break;
	}

	vpin = gpioex_ctx.port[0] | (gpioex_ctx.port[1] << 8);

	for (i = 0; i < 16; ++i) {
		if (vpin & (1 << i))
			gpio_set(tca6416a_vpin2gpio[i].port,
				 tca6416a_vpin2gpio[i].pin);
		else
			gpio_clear(tca6416a_vpin2gpio[i].port,
				   tca6416a_vpin2gpio[i].pin);
	}

	gpioex_ctx.idx = (gpioex_ctx.idx + 1) & TCA6416A_REG_MASK;
}

static uint8_t tca6416a_i2c_slave_read(void *priv)
{
	uint8_t err = 0;

	switch (gpioex_ctx.idx) {
	case TCA6416A_P0_OUT:
	case TCA6416A_P0_IN:
		err = gpioex_ctx.port[0];
		break;
	case TCA6416A_P1_OUT:
	case TCA6416A_P1_IN:
		err = gpioex_ctx.port[1];
		break;
	case TCA6416A_P0_POL:
	case TCA6416A_P1_POL:
	case TCA6416A_P0_CFG:
	case TCA6416A_P1_CFG:
		err = 0;
		break;
	default:
		break;
	}

	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
	return err;
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

