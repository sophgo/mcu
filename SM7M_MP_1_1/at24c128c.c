#include <i2c_master.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define AT24_SLAVE_ADDR	    (0x50)
#define AT24_I2C		I2C2


static inline int __at24c_read_ack(uint8_t offset, uint8_t *value)
{
	return (i2c_master_smbus_read_byte(AT24_I2C, AT24_SLAVE_ADDR,
					  1, offset, value));
}

bool is_se6ctrl_board(void)
{
	int ret = 0;
	unsigned char val;

	ret = __at24c_read_ack(0x00, &val);
	if (ret != 0)
		return false;
	return true;
}
