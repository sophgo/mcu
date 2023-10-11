#include <libopencm3/cm3/nvic.h>
#include <i2c_slave.h>
#include <tca6416a.h>
#include <i2c_master.h>

#define TCA6416A_ADDR	0x20

#define TCA6416A_I2C	I2C2

/* in ms */
#define TCA6416A_SMBTO	(1)

static int is_tca6416a_available;
static uint32_t dummy_tca6416a_regmap[TCA6416A_REG_MAX];

int tca6416a_available(void)
{
	return is_tca6416a_available;
}

/* true if 6416 is active */
int tca6416a_probe(void)
{
	int err;
	uint8_t tmp;

	err = i2c_master_smbus_read_byte(
		TCA6416A_I2C, TCA6416A_ADDR, TCA6416A_SMBTO,
		TCA6416A_P0_CFG, &tmp);

	return is_tca6416a_available = (err == 0);
}

int tca6416a_write(uint8_t reg, uint8_t val)
{
	int err;

	if (is_tca6416a_available) {
		nvic_disable_irq(NVIC_I2C1_IRQ);
		err = i2c_master_smbus_write_byte(
			TCA6416A_I2C, TCA6416A_ADDR, TCA6416A_SMBTO, reg, val);
		nvic_enable_irq(NVIC_I2C1_IRQ);
	} else {
		dummy_tca6416a_regmap[reg & TCA6416A_REG_MASK] = val;
		err = 0;
	}

	return err;
}

int tca6416a_read(uint8_t reg)
{
	uint8_t tmp;
	int err;

	if (is_tca6416a_available) {
		nvic_disable_irq(NVIC_I2C1_IRQ);
		err = i2c_master_smbus_read_byte(
			TCA6416A_I2C, TCA6416A_ADDR, TCA6416A_SMBTO,
			reg, &tmp);
		nvic_enable_irq(NVIC_I2C1_IRQ);
	} else {
		tmp = dummy_tca6416a_regmap[reg & TCA6416A_REG_MASK];
		err = 0;
	}

	return err ? -1 : tmp;
}

int tca6416a_get(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	int val;

	reg = TCA6416A_P0_IN + (port ? 1 : 0);
	val = tca6416a_read(reg);
	return val < 0 ? val : (val & (1 << pin));
}

void tca6416a_set(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val |= (1 << pin);
	tca6416a_write(reg, val);
}

void tca6416a_clr(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val &= ~(1 << pin);
	tca6416a_write(reg, val);
}

static struct {
	int set_idx;
	int idx;
	int temp;
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

	tca6416a_write(gpioex_ctx.idx, data);

	gpioex_ctx.idx = (gpioex_ctx.idx + 1) & TCA6416A_REG_MASK;
}

static uint8_t tca6416a_i2c_slave_read(void *priv)
{
	int err;

	err = tca6416a_read(gpioex_ctx.idx);
	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
	return err < 0 ? 0 : err;
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

