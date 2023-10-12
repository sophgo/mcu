/*
 * mcu.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <i2c/i2c01_slave/i2c01_slave.h>
#include <common/common.h>

#define CRITICAL_ACTION_REBOOT		1
#define CRITICAL_ACTION_POWER_DOWN	2

#define ADC_CHANNEL_SUM			3

#define COLLECT_INTERVAL		25
#define PRINT_OUTPUT_INTERVAL		1000
#define FILTER_DEPTH_SHIFT		2
#define FILTER_DEPTH			(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK		(FILTER_DEPTH - 1)

struct filter {
	unsigned short data[FILTER_DEPTH];
	unsigned long total;
	unsigned long value;
	int p;
};

extern struct filter adc_average_tab[ADC_CHANNEL_SUM];

void mcu_init(struct i2c01_slave_ctx *i2c_slave_ctx);
void mcu_process(void);
void adc_print(void);
void current_print(void);
void temp_print(void);
uint8_t get_critical_action(void);
uint8_t get_critical_temp(void);
uint8_t get_repower_on_temp(void);
