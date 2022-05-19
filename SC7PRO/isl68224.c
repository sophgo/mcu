#include <i2c_master.h>
#include <isl68224.h>
#include <pca9848.h>
#include <debug.h>
#include <tick.h>
#include <gd32e50x_i2c.h>

#define ISL68224_SLAVE_ADDR	0x60
#define I2C			I2C0

static inline void isl68224_select(int idx)
{
	pca9848_set(PCA9848, 1 << (idx + 4));
}

static inline int isl68224_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;
	isl68224_select(idx);
	while (i2c_master_smbus_read_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

static inline int isl68224_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	isl68224_select(idx);
	while (i2c_master_smbus_write_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, data));
		;
	return 0;
}

static inline int isl68224_read_word(int idx, unsigned char cmd)
{
	uint16_t tmp;
	isl68224_select(idx);
	while (i2c_master_smbus_read_word(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp));
		;
	return tmp;
}

unsigned long isl68224_output_voltage(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit mV*/
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x8b);

	return tmp == 0xffff ? 0 : tmp;
}

unsigned long isl68224_output_current(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit 0.1A */
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x8c);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 100;
}

unsigned long isl68224_output_power(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit W */
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x96);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 1000;
}

