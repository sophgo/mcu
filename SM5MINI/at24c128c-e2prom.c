#include <libopencm3/cm3/nvic.h>
#include <libopencmsis/core_cm3.h>
#include <i2c_slave.h>
#include <i2c_master.h>
#include <common.h>
#include <i2c_master.h>
#include <at24c128c-e2prom.h>
#include <debug.h>
#include <tick.h>
#include <mcu.h>
#include <project.h>
#include <stdio.h>
#include <string.h>
#include <eeprom.h>

#define AT24C08D_SLAVE_ADDR	    (0x50)
#define EEPROM_SLAVE_ADDR	    (0X6f)
#define I2C_WR_RETRY		    (10)

static struct i2c_slave_op at24c128c_slave = {
	.addr = EEPROM_SLAVE_ADDR,
};

static struct eeprom_ctx at24_eeprom;

static inline int __at24c128c_read_ack(uint8_t offset, uint8_t *value)
{
	return (i2c_master_smbus_read_byte(I2C2, AT24C08D_SLAVE_ADDR,
					  1, offset, value));
}

int at24c128c_read_byte(void *priv, unsigned int offset)
{
	unsigned int ret = 0;
	uint8_t buf[2];
	uint8_t data;

	buf[0] = offset >> 8;
	buf[1] = offset & 0xff;

	ret = i2c_master_trans(I2C2, AT24C08D_SLAVE_ADDR, 1, buf, 2, &data, 1);
	if (ret != 0)
		return -1;

	return data;
}

int at24c128c_write_byte(void *priv, unsigned int offset, uint8_t data)
{
	unsigned int ret = 0;
	uint8_t buf[3];

	buf[0] = offset >> 8;
	buf[1] = offset & 0xff;
	buf[2] = data;
	for (int i = 0; i < I2C_WR_RETRY; i++) {
		ret = i2c_master_trans(I2C2, AT24C08D_SLAVE_ADDR, 10, buf, 3,
		       NULL, 0);
		if (ret == 0)
			break;
	}

	if (ret != 0)
		return -1;

	return 0;
}

bool is_se6ctrl_board(void)
{
	unsigned int ret = 0;
	unsigned char val;

	ret = __at24c128c_read_ack(0x00, &val);
	if (ret != 0)
		return false;

	return true;
}

void at24c128c_init(struct i2c_slave_ctx *i2c)
{
	eeprom_create(
	i2c,
	&at24_eeprom,
	&at24c128c_slave,
	at24c128c_read_byte,
	at24c128c_write_byte,
	AT24C128C_EEPROM_SIZE
	);
}

