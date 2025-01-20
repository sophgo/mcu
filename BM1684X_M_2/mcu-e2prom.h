#ifndef __MCU_E2PROM_H__
#define __MCU_E2PROM_H__

#define EEPROM_POWER_OFF_REASON_POWER_OFF	0x80
#define EEPROM_POWER_OFF_REASON_RESET		0x81
#define EEPROM_POWER_OFF_REASON_REBOOT		0x82
#define EEPROM_POWER_OFF_REASON_OVER_HEAT	0x83
#define EEPROM_POWER_OFF_REASON_WATCHDOG	0x84

int mcu_eeprom_read_byte(void *priv, unsigned int offset);
int mcu_eeprom_write_byte(void *priv, unsigned int offset, uint8_t data);
void mcu_eeprom_init(struct i2c_slave_ctx *i2c);
void mcu_eeprom_power_off_reason(int reason);
void mcu_eeprom_write_byte_protected(uint16_t offset, uint8_t data);
int is_mixed_mode(void);
int is_test_mode(void);
#endif
