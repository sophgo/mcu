/*
 * pmbus.h: PMBus 电源管理接口
 */
#ifndef __PMBUS_H__
#define __PMBUS_H__

#include <stdint.h>

/* PMBus 参数 */
#define PMBUS_VOUT_MODE		0x20	/* VOUT_MODE */
#define PMBUS_VOUT_CMD		0x21	/* VOUT_COMMAND (写目标电压) */
#define PMBUS_READ_VOUT		0x8B	/* READ_VOUT (读实际输出电压) */

void pmbus_set_addr(uint8_t addr);
uint8_t pmbus_get_addr(void);
void pmbus_read_word(uint8_t addr, uint8_t cmd_reg, uint16_t *val);
void pmbus_write_word(uint8_t addr, uint8_t cmd_reg, uint16_t val);
void pmbus_read_string(uint8_t addr, uint8_t cmd_reg, char *buf, int len);

/* 读 READ_VOUT 原始值, 0=成功 */
int pmbus_read_vout_raw(uint16_t *raw);

/* 读电压 mV, 0=成功 -1=I2C失败 */
int pmbus_get_voltage_mv(unsigned int *mv);

/* 兼容旧接口: 失败返回 0 */
unsigned int pmbus_get_voltage(void);
void pmbus_set_voltage(unsigned int mv);

#endif /* __PMBUS_H__ */
