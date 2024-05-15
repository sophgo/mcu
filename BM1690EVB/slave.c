#include <gd32e50x_i2c.h>
#include <gd32e50x.h>
#include <common.h>
#include <project.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <i2c-slaves/mcu.h>
#include <i2c-slaves/slt.h>

static struct i2c01_slave_ctx i2c0_slave_ctx;
static struct i2c_slave_ctx i2c2_slave_ctx;

#define I2C1_OA1		0x17
#define I2C1_OA2		0x68
#define OA2_MASK		0x03

void slave_init(void)
{
	if (get_board_type() == MILKV_PIONEER) {
		i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2, I2C1_OA1, I2C1_OA2, OA2_MASK);
		mcu_milkv_init(&i2c2_slave_ctx);
		i2c_slave_start(&i2c2_slave_ctx);
		nvic_irq_enable(I2C2_EV_IRQn, 0, 0);
	} else {
		i2c01_slave_init(&i2c0_slave_ctx, (void *)I2C0, I2C1_OA1, I2C1_OA2);
		mcu_x8_init(&i2c0_slave_ctx);
		slt_init(&i2c0_slave_ctx);
		i2c01_slave_start(&i2c0_slave_ctx);
		nvic_irq_enable(I2C0_EV_IRQn, 0, 0);
	}

}

/* replace default isr */
void I2C0_EV_IRQHandler(void)
{
	i2c01_slave_isr(&i2c0_slave_ctx);
}

/* replace default isr */
void I2C2_EV_IRQHandler(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}

void I2C2_ER_IRQHandler(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}

