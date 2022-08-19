#include <common.h>
#include <project.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>

static struct i2c_slave_ctx i2c2_slave_ctx;
static struct i2c01_slave_ctx i2c0_slave_ctx;

#define I2C0_SLAVE_OA1		0x17
#define I2C0_SLAVE_OA2		0x4c
#define I2C0_SLAVE_OA2M		3

void slave_init(void)
{
	i2c_slave_init(&i2c2_slave_ctx, (void *)I2C0,
		       I2C0_SLAVE_OA1, I2C0_SLAVE_OA2, I2C0_SLAVE_OA2M);

	i2c_slave_start(&i2c2_slave_ctx);
}

/* replace default isr */
void I2C2_EV_IRQHandler(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}

/* replace default isr */
void I2C0_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}

void I2C0_ER_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}

