#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencmsis/core_cm3.h>
#include <i2c_slave.h>
#include <i2c_master.h>
#include <common.h>
#include <i2c_master.h>
#include <debug.h>
#include <tick.h>
#include <mcu.h>
#include <project.h>
#include <stdio.h>
#include <string.h>
#include <eeprom.h>
#include <mcu-e2prom.h>

#define  EEPROM_SLAVE_ADDR	    (0x6a)

static struct i2c_slave_op mcu_e2prom_slave = {
	.addr = EEPROM_SLAVE_ADDR,
};

static struct eeprom_ctx mcu_e2prom;

int mcu_eeprom_read_byte(void *priv, unsigned int offset)
{
	return *((volatile uint8_t *)(EEPROM_BASE + offset));
}

int mcu_eeprom_write_byte(void *priv, unsigned int offset, uint8_t data)
{
	eeprom_program_byte(EEPROM_BASE + offset, data);
	return 0;
}

void mcu_eeprom_power_off_reason(int reason)
{
	uint8_t tmp = reason;

	mcu_eeprom_write_byte(&mcu_e2prom, EEPROM_POWER_OFF_REASON_OFFSET, tmp);
}

void mcu_eeprom_write_byte_protected(uint16_t offset, uint8_t data)
{
	if (!eeprom_get_lock_status())
		eeprom_program_byte(EEPROM_BASE + offset, data);
}
int is_mixed_mode(void)
{
	return mcu_eeprom_read_byte(&mcu_e2prom, EEPROM_MIXED_MODE_OFFSET) == 1 ? 1 : 0;
}

int is_test_mode(void)
{
	return mcu_eeprom_read_byte(&mcu_e2prom, EEPROM_TEST_MODE_OFFSET) == 1 ? 1 : 0;
}

void mcu_eeprom_init(struct i2c_slave_ctx *i2c)
{
	eeprom_create(
	i2c,
	&mcu_e2prom,
	&mcu_e2prom_slave,
	mcu_eeprom_read_byte,
	mcu_eeprom_write_byte,
	EEPROM_SIZE
	);
}

