#include <i2c_master.h>
#include <tick.h>
#include <debug.h>
#include <timer.h>
#include <stddef.h>

#define I2C_SPEED	(100 * 1000)

int i2c_master_init(int i2c)
{
	/* configure I2C clock */
	i2c_clock_config(i2c, I2C_SPEED, I2C_DTCY_2);
	/* configure I2C address */
	i2c_mode_addr_config(i2c, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
	/* enable i2c controller */
	i2c_enable(i2c);
	/* enable acknowledge */
	i2c_ack_config(i2c, I2C_ACK_ENABLE);

	return 0;
}

int i2c_master_destroy(int i2c)
{
	i2c_disable(i2c);
	return 0;
}

int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			  uint8_t *w, size_t wn,
			  uint8_t *r, size_t rn)
{
	i2c_transfer7(i2c, addr, w, wn, r, rn);
	return 0;
}

int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn,
		     void *r, unsigned int rn)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, w, wn, r, rn);
}

int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &cmd, 1, data, 1);
}

int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data)
{
	unsigned char tmp[2];

	tmp[0] = cmd;
	tmp[1] = data;
	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 2, NULL, 0);
}

int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &data, 1, NULL, 0);
}

int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, NULL, 0, data, 1);
}

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     &cmd, 1, (uint8_t *)data, 2);
}

