#include <i2c_master.h>
#include <mp5475.h>
#include <debug.h>
#include <tick.h>

#define MP5475_SLAVE_ADDR	0x60
#define I2C			I2C2

#define MP5475_ENABLE_REG_DEFAULT_VALUE	(1 << 7 | 1 << 5)
#define MP5475_TMP_REG	0x57

static uint8_t mp5475_enable_reg_value = MP5475_ENABLE_REG_DEFAULT_VALUE;

static inline int mp5475_read_byte(unsigned char cmd)
{
	unsigned char tmp;
	while (i2c_master_smbus_read_byte(I2C, MP5475_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

static inline int mp5475_write_byte(unsigned char cmd,
				    unsigned char data)
{
	while (i2c_master_smbus_write_byte(I2C, MP5475_SLAVE_ADDR,
					  1, cmd, data));
		;
	return 0;
}

int mp5475_buck_on(unsigned int buck)
{
	mp5475_enable_reg_value |= (1 << 5) >> buck;
	mp5475_write_byte(0x40, mp5475_enable_reg_value);
	return 0;
}

void mp5475_buck_off(unsigned int buck)
{
#ifdef CLOSEPMIC
	mp5475_enable_reg_value &= ~((1 << 5) >> buck);
	mp5475_write_byte(0x40, mp5475_enable_reg_value);
#endif
}

int mp5475_voltage_config(unsigned int buck, unsigned int voltage)
{
	unsigned int reg_h = 0x13 + 8 * buck;
	unsigned int reg_l = 0x14 + 8 * buck;
	unsigned int value = voltage / 2;
	unsigned int val_h = (value >> 8) & 0x03;
	unsigned int val_l = value & 0xff;

	mp5475_write_byte(reg_h, val_h);
	mp5475_write_byte(reg_l, val_l);
	return 0;
}

unsigned long mp5475_output_current(unsigned int buck)
{
	unsigned int reg = 0x59 + 2 * buck;

	return mp5475_read_byte(reg) * 50;
}

unsigned long mp5475_output_voltage(unsigned int buck)
{
	unsigned int reg = 0x5a + 2 * buck;

	return mp5475_read_byte(reg) * 25;
}

int mp5475_get_tmp(void)
{
	return mp5475_read_byte(MP5475_TMP_REG) - 50;
}

int mp5475_init(void)
{
	/* enable system enable, disable buck 1, 2, 3, 4 */
	mp5475_write_byte(0x40, mp5475_enable_reg_value);
	mp5475_voltage_config(0, 1850);
	mp5475_voltage_config(1, 1100);
	mp5475_voltage_config(2, 600);
	mp5475_voltage_config(3, 810);
	debug("\n********************************mp5475 init*************************\n");
	return 0;
}

void mp5475_destroy(void)
{
}

