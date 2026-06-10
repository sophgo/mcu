/*
 * pmbus.h: PMBus 电源管理接口
 */
#ifndef __PMBUS_H__
#define __PMBUS_H__

#include <stdint.h>

/* PMBus 参数 */
#define PMBUS_VOUT_CMD		0x21	/* VOUT_COMMAND */
#define PMBUS_VOUT_MODE		0x20	/* VOUT_MODE */

void pmbus_set_addr(uint8_t addr);
void pmbus_read_word(uint8_t addr, uint8_t cmd_reg, uint16_t *val);
void pmbus_write_word(uint8_t addr, uint8_t cmd_reg, uint16_t val);
void pmbus_read_string(uint8_t addr, uint8_t cmd_reg, char *buf, int len);

/* 高层接口: 针对默认地址的 VOUT 读写 (mV) */
unsigned int pmbus_get_voltage(void);
void pmbus_set_voltage(unsigned int mv);

#endif /* __PMBUS_H__ */