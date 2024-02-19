#include <i2c_master.h>
#include <debug.h>

#define TCA9554_SLAVE_ADDR	0x20
#define TCA9554_IN		0x00
#define TCA9554_OUT		0x01
#define TCA9554_INV		0x02
#define TCA9554_CFG		0x03

#define I2C			I2C2

static inline int tca9554_read_byte(unsigned char cmd)
{
	unsigned char tmp;

	i2c_master_smbus_read_byte(I2C, TCA9554_SLAVE_ADDR,
				   1, cmd, &tmp);
	return tmp;
}

static inline void tca9554_write_byte(unsigned char cmd,
				    unsigned char data)
{
	int err;

	do {
		err = i2c_master_smbus_write_byte(I2C, TCA9554_SLAVE_ADDR,
						  1, cmd, data);
		// debug("tca9554 i2c err %d\n", err);
	} while (err);
}

void tca9554_set_all(void)
{
	tca9554_write_byte(TCA9554_OUT, 0xff);
}

void tca9554_clr_all(void)
{
	tca9554_write_byte(TCA9554_OUT, 0x00);
}

void tca9554_init(void)
{
	tca9554_set_all();
	/* set all pins to output */
	tca9554_write_byte(TCA9554_CFG, 0);
}

