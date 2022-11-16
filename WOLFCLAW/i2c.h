#ifndef __I2C_H__
#define __I2C_H__

#include <i2c_master.h>
#include <debug.h>

static inline long i2c_smbus_write_quick(int i2c, int addr, unsigned char value)
{
	return i2c_master_smbus_write_block(i2c, addr, 1,
			 0, NULL);
}

static inline long i2c_smbus_read_i2c_block_data(int i2c, int addr, unsigned char command,
						  unsigned char length, unsigned char *values)
{
    if (length > 32)
		length = 32;
	unsigned char data[length];
	int i, err;

	for (i = 0; i < length; i++)
		data[i] = 0;
	err = i2c_master_smbus_read_block(i2c, addr, 1, command,
			 length, (uint8_t *)data);

	for (i = 0; i < length; ++i)
		values[i] = data[i];

	return err;
}

static inline long i2c_smbus_write_i2c_block_data(int i2c, int addr, unsigned char command,
						   unsigned char length, unsigned char *values)
{
    if (length > 32)
		length = 32;
	unsigned char data[length + 1];
	int i;
	for (i = 1; i <= length; i++)
		data[i] = values[i-1];
	data[0] = command;
    return i2c_master_smbus_write_block(i2c, addr, 1,
			 (sizeof(data)/sizeof(data[0])), (uint8_t *)data);
}

#endif
