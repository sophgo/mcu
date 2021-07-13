#include <libopencm3/cm3/nvic.h>
#include <i2c_slave.h>
#include <tca6416a.h>
#include <pin.h>

#define TCA6416A_PORT2VPIN(port, pin)	(((port) * 8) + (pin))

#define TCA6416A_ERROR_LED_PORT		0
#define TCA6416A_ERROR_LED_PIN		4
#define TCA6416A_ERROR_LED_VPIN		\
	TCA6416A_PORT2VPIN(TCA6416A_ERROR_LED_PORT, TCA6416A_ERROR_LED_PIN)

#define TCA6416A_STATUS_LED_PORT	1
#define TCA6416A_STATUS_LED_PIN		3
#define TCA6416A_STATUS_LED_VPIN	\
	TCA6416A_PORT2VPIN(TCA6416A_STATUS_LED_PORT, TCA6416A_STATUS_LED_PIN)

static struct {
	int set_idx;
	int idx;
	uint8_t port[2];
} gpioex_ctx;

const struct {
	int port, pin;
} tca6416a_vpin2gpio[16] = {
	[TCA6416A_ERROR_LED_VPIN] = {MCU_LED_PORT, MCU_LED_PIN},
	[TCA6416A_STATUS_LED_VPIN] = {HEALTHY_LED_PORT, HEALTHY_LED_PIN},
};

static void tca6416a_i2c_slave_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		gpioex_ctx.set_idx = 1;
}

static void tca6416a_pin_update(void)
{
	uint16_t vpin;
	int i;

	vpin = gpioex_ctx.port[0] | (gpioex_ctx.port[1] << 8);

	for (i = 0; i < 16; ++i) {
		if (tca6416a_vpin2gpio[i].port) {
			if (vpin & (1 << i))
				gpio_clear(tca6416a_vpin2gpio[i].port,
					   tca6416a_vpin2gpio[i].pin);
			else
				gpio_set(tca6416a_vpin2gpio[i].port,
					 tca6416a_vpin2gpio[i].pin);
		}
	}
}

void tca6416a_error_led_on(void)
{
	/* update current port status */
	gpioex_ctx.port[TCA6416A_ERROR_LED_PORT] |= 1 << TCA6416A_ERROR_LED_PIN;
	tca6416a_pin_update();
}

void tca6416a_reset(void)
{
	/* update current port status */
	gpioex_ctx.port[0] = 0;
	gpioex_ctx.port[1] = 0;
	tca6416a_pin_update();
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

	tca6416a_pin_update();

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

