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

void eeprom_init(struct i2c_slave_ctx *i2c);
void eeprom_log_power_off_reason(int reason);

#define EEPROM_POWER_OFF_REASON_POWER_OFF	0x80
#define EEPROM_POWER_OFF_REASON_RESET		0x81
#define EEPROM_POWER_OFF_REASON_REBOOT		0x82
#define EEPROM_POWER_OFF_REASON_OVER_HEAT	0x83
#define EEPROM_POWER_OFF_REASON_WATCHDOG	0x84

int eeprom_get_lock_status(void);
void eeprom_lock_code(uint8_t code);
void eeprom_write_byte_protected(uint16_t offset, uint8_t data);
uint8_t eeprom_read_byte(uint32_t offset);
int is_mixed_mode(void);
int is_test_mode(void);
#endif
