/*
 * eeprom.h
 *
 *  Created on: Jul 22, 2019
 *      Author: weic
 */

#ifndef __EEPROM__
#define __EEPROM__

#include <stdint.h>
#include <i2c_slave.h>

struct eeprom_ctx {
	int set_idx;
	int idx;
	int (*read_byte)(void *, unsigned int);
	int (*write_byte)(void *, unsigned int, uint8_t);
	unsigned int size;
	unsigned char tmp;
};

void eeprom_create(
	struct i2c_slave_ctx *i2c,
	struct eeprom_ctx *eeprom,
	struct i2c_slave_op *slave,
	int (*read)(void *, unsigned int),
	int (*write)(void *, unsigned int, uint8_t),
	unsigned int size
	);

int eeprom_get_lock_status(void);
void eeprom_lock_code(uint8_t code);
void eeprom_write_byte_protected(void *priv, uint16_t offset, uint8_t data);

#endif
