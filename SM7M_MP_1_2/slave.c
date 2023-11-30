#include <mcu.h>
#include <eeprom.h>
#include <wdt.h>
#include <tmp451.h>
#include <tca6416a.h>
#include <pic.h>
#include <keyboard.h>
#include <common.h>
#include <project.h>

static struct i2c_slave_ctx i2c2_slave_ctx;
static struct i2c01_slave_ctx i2c0_slave_ctx;

#define I2C2_SLAVE_OA1		0x17
#define I2C2_SLAVE_OA2		0x68
#define I2C2_SLAVE_OA2M		3

#define DUMMY_SLAVE_OA1		0x38
#define DUMMY_SLAVE_OA2		-1
#define DUMMY_SLAVE_OA2M	0

static void open_sa5_i2c_slave(void)
{
	i2c01_slave_start(&i2c0_slave_ctx);
	nvic_irq_enable(MCU_CPLD_I2C_IRQ, 0, 0);
}

void slave_init(void)
{
	i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2,
		       I2C2_SLAVE_OA1, I2C2_SLAVE_OA2, I2C2_SLAVE_OA2M);

	i2c01_slave_init(&i2c0_slave_ctx, (void *)I2C0, DUMMY_SLAVE_OA1,
			 DUMMY_SLAVE_OA2, DUMMY_SLAVE_OA2M);

	mcu_init(&i2c2_slave_ctx, &i2c0_slave_ctx);
	eeprom_init(&i2c2_slave_ctx);
	wdt_init(&i2c2_slave_ctx);
	// tmp451_init(&i2c2_slave_ctx);

	if (pic_available()) {
		kbd_init(&i2c2_slave_ctx);
		pic_init(&i2c2_slave_ctx);
	}

	if (tca6416a_available())
		tca6416a_init(&i2c2_slave_ctx);

	i2c_slave_start(&i2c2_slave_ctx);

	nvic_irq_enable(MCU_SOC_I2C_IRQ, 0, 0);

	if (get_board_type() == SM5MA)
		open_sa5_i2c_slave();
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

