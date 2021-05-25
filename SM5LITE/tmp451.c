#include <i2c_master.h>

#define TMP451_I2C		I2C1
#define TMP451_I2C_ADDR		0x4c
#define TMP451_I2C_TIMEOUT	1

#define TMP451_REG_CONFIG_RD	0x03
#define TMP451_REG_CONFIG_WR	0x09

#define TMP451_REG_CONALT_RD	0x22
#define TMP451_REG_CONALT_WR	0x22

void tmp451_init(void)
{
	/* enable external temperature mode */
	i2c_master_smbus_write_byte(TMP451_I2C, TMP451_I2C_ADDR,
				    TMP451_I2C_TIMEOUT,
				    TMP451_REG_CONFIG_WR, 1 << 2);

	/* enable smbus timeout detection */
	i2c_master_smbus_write_byte(TMP451_I2C, TMP451_I2C_ADDR,
				    TMP451_I2C_TIMEOUT,
				    TMP451_REG_CONALT_WR, 1 | (1 << 7));
}
