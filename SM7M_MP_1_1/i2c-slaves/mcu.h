#ifndef __MCU_H__
#define __MCU_H__

#include <i2c_slave.h>
#include <common.h>

#define MCU_INT_OVER_TEMP_ALERT		BIT(0)
#define MCU_INT_OVER_TEMP_POWEROFF	BIT(1)
#define MCU_INT_POWERON_ERR		BIT(2)
#define MCU_INT_WDT_RST			BIT(3)
#define MCU_INT_RESET_OP		BIT(4)
#define MCU_INT_TEST_INTR		BIT(7)

#define CRITICAL_ACTION_POWERDOWN	2
#define CRITICAL_ACTION_REBOOT		1

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx);
void mcu_raise_interrupt(uint8_t reg_idx, uint8_t interrupts);
void mcu_test_init(struct i2c_slave_ctx *i2c_slave_ctx);
void mcu_process(void);
int mcu_get_test_mode(void);
void mcu_set_test_mode(int mode);
uint8_t get_critical_action(void);
uint8_t get_critical_temp(void);
uint8_t get_repoweron_temp(void);
bool mcu_get_se6_aiucore(void);

#endif
