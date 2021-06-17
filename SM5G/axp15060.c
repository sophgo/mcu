#include <i2c_master.h>
#include <axp15060.h>
#include <debug.h>
#include <tick.h>
#include <pin.h>

#define AXP15050_SLAVE_ADDR	0x36
#define I2C			I2C1

#define AXP15050_ENABLE_REG_DEFAULT_VALUE	0

static uint8_t axp15060_enable_reg_value = AXP15050_ENABLE_REG_DEFAULT_VALUE;

static inline int axp15060_read_byte(unsigned char cmd)
{
	unsigned char tmp;
	while (i2c_master_smbus_read_byte(I2C, AXP15050_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

static inline int axp15060_write_byte(unsigned char cmd,
				    unsigned char data)
{
	while (i2c_master_smbus_write_byte(I2C, AXP15050_SLAVE_ADDR,
					  1, cmd, data));
		;
	return 0;
}

int axp15060_buck_on(unsigned int buck)
{
	axp15060_enable_reg_value |= 1 << buck;
	axp15060_write_byte(0x10, axp15060_enable_reg_value);
	return 0;
}

void axp15060_buck_off(unsigned int buck)
{
	axp15060_enable_reg_value &= ~(1 << buck);
	axp15060_write_byte(0x10, axp15060_enable_reg_value);
}

void axp15060_ldo_on(unsigned int ldo)
{
	int reg = ldo >= 8 ? 0x12 : 0x11;
	int shift = ldo - 8;
	uint8_t tmp;

	tmp = axp15060_read_byte(reg);
	tmp |= 1 << shift;
	axp15060_write_byte(reg, tmp);
}

void axp15060_ldo_off(unsigned int ldo)
{
	int reg = ldo >= 8 ? 0x12 : 0x11;
	int shift = ldo - 8;
	uint8_t tmp;

	tmp = axp15060_read_byte(reg);
	tmp &= ~(1 << shift);
	axp15060_write_byte(reg, tmp);
}

/* unit is mv */
const struct {
	unsigned int base, step;
} buck_voltage_info[] = {
	{1500, 100},
	{500, 10},
	{500, 10},
	{500, 10},
	{800, 10},
	{500, 100},
};

int axp15060_voltage_config(unsigned int buck, unsigned int voltage)
{
	const int base = buck_voltage_info[buck].base;
	const int step = buck_voltage_info[buck].step;
	int reg = 0x13 + buck;
	int cfg = (voltage - base) / step;

	axp15060_write_byte(reg, cfg);
	return 0;
}

int axp15060_init(void)
{
	uint8_t tmp;

	/* disable all output */
	axp15060_write_byte(0x24, axp15060_enable_reg_value);
	tmp = axp15060_read_byte(0x11);
	axp15060_write_byte(0x11, tmp & ~3);
	/* TODO: reset pwron pin to enable mode, not pok mode */

	axp15060_voltage_config(0, 3300);
	axp15060_voltage_config(1, 800);
	axp15060_voltage_config(2, 800);
	axp15060_voltage_config(3, 600);
	axp15060_voltage_config(4, 1100);
	axp15060_voltage_config(5, 1800);
	return 0;
}

void axp15060_destroy(void)
{
}

