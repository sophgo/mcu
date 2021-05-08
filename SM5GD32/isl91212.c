#include <i2c_master.h>
#include <isl91212.h>
#include <debug.h>
#include <tick.h>
#include <pin.h>

#define ISL91212_SLAVE_ADDR	0x1e
#define I2C			I2C1

/* bit 0 of IO_MODECTRL is not reserved as datasheet says, it seems a global
 * enable bit, no voltage out on all bulks if bit 0 is set to 0
 */
#define ISL91212_ENABLE_REG_DEFAULT_VALUE	((1 << 2) | (1 << 0))

static uint8_t isl91212_enable_reg_value = ISL91212_ENABLE_REG_DEFAULT_VALUE;

static inline int isl91212_read_byte(unsigned char cmd)
{
	unsigned char tmp;
	while (i2c_master_smbus_read_byte(I2C, ISL91212_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

static inline int isl91212_write_byte(unsigned char cmd,
				    unsigned char data)
{
	while (i2c_master_smbus_write_byte(I2C, ISL91212_SLAVE_ADDR,
					  1, cmd, data));
		;
	return 0;
}

int isl91212_buck_on(unsigned int buck)
{
	isl91212_enable_reg_value |= (1 << 7) >> buck;
	isl91212_write_byte(0x24, isl91212_enable_reg_value);
	return 0;
}

void isl91212_buck_off(unsigned int buck)
{
	isl91212_enable_reg_value &= ~((1 << 7) >> buck);
	isl91212_write_byte(0x24, isl91212_enable_reg_value);
}

int isl91212_voltage_config(unsigned int buck, unsigned int voltage)
{
	const int step = 1200;
	uint16_t dac = (voltage * 1000) / step;
	uint8_t cfg1 = (dac >> 2) & 0xff;
	uint8_t cfg0 = (dac & 0x3) << 6;

	unsigned int reg_cfg1 = 0x48 + 0x1a * buck;
	unsigned int reg_cfg0 = reg_cfg1 + 1;

	isl91212_write_byte(reg_cfg1, cfg1);
	isl91212_write_byte(reg_cfg0, cfg0);
	return 0;
}

int isl91212_init(void)
{
	/* enable system enable, disable buck 1, 2, 3, 4 */
	isl91212_write_byte(0x24, isl91212_enable_reg_value);

	isl91212_write_byte(0x3b, 0);
	isl91212_write_byte(0x58, 0);
	isl91212_write_byte(0x72, 0);
	isl91212_write_byte(0x8c, 0);

	isl91212_voltage_config(0, 1000);
	isl91212_voltage_config(1, 1100);
	isl91212_voltage_config(2, 600);
	isl91212_voltage_config(3, 0);
	return 0;
}

void isl91212_destroy(void)
{
}

