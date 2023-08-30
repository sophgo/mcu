#ifndef __MCU_H__
#define __MCU_H__

#include <i2c_slave.h>
#include <i2c01_slave.h>
#include <common.h>

#define MCU_INT_OVER_TEMP_ALERT		BIT(0)
#define MCU_INT_OVER_TEMP_POWEROFF	BIT(1)
#define MCU_INT_POWERON_ERR		BIT(2)
#define MCU_INT_WDT_RST			BIT(3)
#define MCU_INT_RESET_OP		BIT(4)
#define MCU_INT_TEST_INTR		BIT(7)

void mcu_init(struct i2c_slave_ctx *i2c_slave_ctx,
	      struct i2c01_slave_ctx *i2c01_slave_ctx);
void mcu_raise_interrupt(uint8_t interrupts);

#endif
