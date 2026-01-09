#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#include <gd32e50x_i2c.h>
/* use I2C1 I2C2 ... as function first parameter */

int i2c_master_init(int i2c);
int i2c_master_destroy(int i2c);
int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn, void *r, unsigned int rn);
int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data);
int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data);
int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data);
int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data);

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data);
int i2c_master_write_byte(int i2c, unsigned char addr,
			  unsigned long timeout,
			  unsigned char data);
int i2c_master_write_block(int i2c, unsigned char addr,
			   unsigned long timeout,
			   unsigned char *data, unsigned int len);
int i2c_master_read_block(int i2c, unsigned char addr,
			  unsigned long timeout,
			  unsigned char *data, unsigned int len);

int i2c_master_smbus_write_word(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, uint16_t data);
#endif
