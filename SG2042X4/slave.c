#include <gd32e50x_i2c.h>
#include <gd32e50x.h>
#include <common.h>
#include <project.h>
#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <eeprom.h>
#include <mcu.h>
#include <nct218.h>
#include <wdt.h>

static struct i2c_slave_ctx i2c2_slave_ctx;

#define I2C2_OA1		0x17
#define I2C2_OA2		0x68
#define I2C2_OA2M		3

static int slave_init_time = 1;

void slave_init(void)
{
	i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2, I2C2_OA1, I2C2_OA2, I2C2_OA2M);

	if (slave_init_time > 0) {
		mcu_init(&i2c2_slave_ctx);
		eeprom_init(&i2c2_slave_ctx);
		nct218_init(&i2c2_slave_ctx);
		wdt_init(&i2c2_slave_ctx);
	}

	i2c_slave_start(&i2c2_slave_ctx);
	nvic_irq_enable(I2C2_EV_IRQn, 0, 0);

	slave_init_time = 0;
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
