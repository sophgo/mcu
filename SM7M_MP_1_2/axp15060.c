#include <i2c_master.h>
#include <axp15060.h>
#include <debug.h>
#include <tick.h>
#include <pin.h>
#include <common.h>

#define AXP15050_SLAVE_ADDR	0x37
#define I2C			I2C1

struct axp15060_channel_info {
	uint8_t en_reg, en_shift;
	uint8_t vol_cfg_reg;
	int vol_base, vol_step;
} axp15060_channel_info[] = {
	[AXP15060_DCDC1] = {
		.en_reg = 0x10,
		.en_shift = 0,
		.vol_cfg_reg = 0x13,
		.vol_base = 1500,
		.vol_step = 100,
	},

	[AXP15060_DCDC2] = {
		.en_reg = 0x10,
		.en_shift = 1,
		.vol_cfg_reg = 0x14,
		.vol_base = 500,
		.vol_step = 10,
	},

	[AXP15060_DCDC3] = {
		.en_reg = 0x10,
		.en_shift = 2,
		.vol_cfg_reg = 0x15,
		.vol_base = 500,
		.vol_step = 10,
	},

	[AXP15060_DCDC4] = {
		.en_reg = 0x10,
		.en_shift = 3,
		.vol_cfg_reg = 0x16,
		.vol_base = 500,
		.vol_step = 10,
	},

	[AXP15060_DCDC5] = {
		.en_reg = 0x10,
		.en_shift = 4,
		.vol_cfg_reg = 0x17,
		.vol_base = 800,
		.vol_step = 10,
	},

	[AXP15060_DCDC6] = {
		.en_reg = 0x10,
		.en_shift = 5,
		.vol_cfg_reg = 0x18,
		.vol_base = 500,
		.vol_step = 100,
	},

	[AXP15060_ALDO1] = {
		.en_reg = 0x11,
		.en_shift = 0,
		.vol_cfg_reg = 0x19,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_ALDO2] = {
		.en_reg = 0x11,
		.en_shift = 1,
		.vol_cfg_reg = 0x20,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_ALDO3] = {
		.en_reg = 0x11,
		.en_shift = 2,
		.vol_cfg_reg = 0x21,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_ALDO4] = {
		.en_reg = 0x11,
		.en_shift = 3,
		.vol_cfg_reg = 0x22,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_ALDO5] = {
		.en_reg = 0x11,
		.en_shift = 4,
		.vol_cfg_reg = 0x23,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_BLDO1] = {
		.en_reg = 0x11,
		.en_shift = 5,
		.vol_cfg_reg = 0x24,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_BLDO2] = {
		.en_reg = 0x11,
		.en_shift = 6,
		.vol_cfg_reg = 0x25,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_BLDO3] = {
		.en_reg = 0x11,
		.en_shift = 7,
		.vol_cfg_reg = 0x26,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_BLDO4] = {
		.en_reg = 0x12,
		.en_shift = 0,
		.vol_cfg_reg = 0x27,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_BLDO5] = {
		.en_reg = 0x12,
		.en_shift = 1,
		.vol_cfg_reg = 0x28,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_CLDO1] = {
		.en_reg = 0x12,
		.en_shift = 2,
		.vol_cfg_reg = 0x29,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_CLDO2] = {
		.en_reg = 0x12,
		.en_shift = 3,
		.vol_cfg_reg = 0x2a,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_CLDO3] = {
		.en_reg = 0x12,
		.en_shift = 4,
		.vol_cfg_reg = 0x2b,
		.vol_base = 700,
		.vol_step = 100,
	},

	[AXP15060_CLDO4] = {
		.en_reg = 0x12,
		.en_shift = 5,
		.vol_cfg_reg = 0x2d,
		.vol_base = 700,
		.vol_step = 100,
	},
};

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

int axp15060_channel_on(unsigned int channel)
{
	uint8_t reg, val, shift;

	reg = axp15060_channel_info[channel].en_reg;
	shift = axp15060_channel_info[channel].en_shift;

	val = axp15060_read_byte(reg);
	val |= 1 << shift;
	axp15060_write_byte(reg, val);
	return 0;
}

void axp15060_channel_off(unsigned int channel)
{
	uint8_t reg, val, shift;

	reg = axp15060_channel_info[channel].en_reg;
	shift = axp15060_channel_info[channel].en_shift;

	val = axp15060_read_byte(reg);
	val &= ~(1 << shift);
	axp15060_write_byte(reg, val);

}

int axp15060_voltage_config(unsigned int channel, unsigned int voltage)
{
	int base = axp15060_channel_info[channel].vol_base;
	int step = axp15060_channel_info[channel].vol_step;
	int reg = axp15060_channel_info[channel].vol_cfg_reg;
	int cfg = (voltage - base) / step;

	axp15060_write_byte(reg, cfg);
	return 0;
}

int axp15060_init(void)
{
	/* disable all output */
	axp15060_write_byte(0x10, 0);
	axp15060_write_byte(0x11, 0);
	axp15060_write_byte(0x12, 0);

	/* set dcdc4 and dcdc6 to poly-phase mode */
	axp15060_write_byte(0x1a, 1 << 7);
	/* set all dcdc to pwm mode */
	axp15060_write_byte(0x1b, 0x3f);

	axp15060_voltage_config(AXP15060_DCDC1, 1800);
	axp15060_voltage_config(AXP15060_DCDC2, 830);

	/* LPDDR4 and LPDDR4X are supported */
	if (get_ddr_type() == DDR_TYPE_LPDDR4)
		axp15060_voltage_config(AXP15060_DCDC3, 1120);
	else
		axp15060_voltage_config(AXP15060_DCDC3, 610);

	axp15060_voltage_config(AXP15060_DCDC4, 1120);
	axp15060_voltage_config(AXP15060_DCDC5, 830);
	axp15060_voltage_config(AXP15060_ALDO1, 1800);

	return 0;
}

void axp15060_destroy(void)
{
}

