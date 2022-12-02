#ifndef __PIC_H__
#define __PIC_H__

#include <i2c_slave.h>

/* PIC function definition */
#define PIC_REG_BOARD_TYPE	0
#define PIC_REG_SW_VERSION	1
#define PIC_REG_HW_VERSION	2
#define PIC_REG_CTRL		3
#define PIC_REG_SOC_TEMP	4
#define PIC_REG_HEATER_CTRL	5
#define PIC_REG_REQUEST		6

#define PIC_BOARD_TYPE_SE5	3
#define PIC_CMD_REBOOT		7
#define PIC_CMD_POWER_OFF	2

#define PIC_REQ_NONE		0
#define PIC_REQ_POWER_OFF	1
#define PIC_REQ_REBOOT		2
#define PIC_REQ_FACTORY_RESET	3

int pic_available(void);
int pic_probe(void);
int pic_write(uint8_t reg, uint8_t val);
int pic_read(uint8_t reg);
void pic_init(struct i2c_slave_ctx *i2c);

#endif
