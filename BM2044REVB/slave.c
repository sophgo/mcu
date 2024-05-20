#include <gd32e50x_i2c.h>
#include <gd32e50x.h>
#include <common.h>
#include <project.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <mcu.h>
#include <slt.h>
#include <ct7451.h>

static struct i2c01_slave_ctx i2c0_slave_ctx;

#define I2C0_SLAVE_OA1		0x17
#define I2C0_SLAVE_OA2		0x68

void slave_init(void)
{
	i2c01_slave_init(&i2c0_slave_ctx, (void *)I2C0,
		       I2C0_SLAVE_OA1, I2C0_SLAVE_OA2);
	mcu_init(&i2c0_slave_ctx);
	slt_init(&i2c0_slave_ctx);
	ct7451_init(&i2c0_slave_ctx);
	i2c01_slave_start(&i2c0_slave_ctx);
    	nvic_irq_enable(I2C0_EV_IRQn, 0, 0);
}

/* replace default isr */
void I2C0_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}

/* replace default isr */
void I2C1_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}

void I2C1_ER_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}
