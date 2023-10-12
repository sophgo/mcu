/*
 * ds1307.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __DS1307_H__
#define __DS1307_H__

#include <stdint.h>

#define DS1307_TIME_REG_MAX		(7)

#define DS1307_COLLECT_INTERVAL		1000

#define I2C				I2C0
#define SMBUS_TIMEOUT			1
#define DS1307_SLAVE_ADDR		(0x68)

#define NA				(-1)	/* Operation Not Applied */

struct ds1307_reg {
	uint8_t read_ptr;			/* Read pointer */
	uint8_t write_ptr;			/* Write pointer */
	uint8_t init_value;			/* Initialization */
	uint8_t present_value;			/* Present value */
};

struct ds1307_ctx {
	struct ds1307_reg seconds;
	struct ds1307_reg minutes;
	struct ds1307_reg hours;
	struct ds1307_reg day_of_week;
	struct ds1307_reg date;
	struct ds1307_reg month;
	struct ds1307_reg year;
	struct ds1307_reg control;
	struct ds1307_reg ram[0x38];
};

extern struct ds1307_ctx ds1307_instance_reg;
extern char time[21];

void ds1307_software_reset(void);
uint8_t ds1307_reg_read(struct ds1307_reg ds1307_reg_tmp);
int ds1307_reg_write(struct ds1307_reg ds1307_reg_tmp, uint8_t value);
void ds1307_get_time(void);
void ds1307_init(void);

#endif /* __DS1307_H__ */
