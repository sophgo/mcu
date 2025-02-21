/*
 * ct7451.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <stdio.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <tick.h>
#include <chip.h>
#include <power.h>
#include <common.h>
#include <mon.h>
#include <stdlib.h>
#include <debug.h>
#include <pca9848.h>
#include <timer.h>
#include <system.h>
#include <math.h>


#define CT7451_REG_MAX	(22)

#define PTRNA	(-1)	/* Operation not apply */
#define PORNA	(-1)	/* Power-on reset value not apply */

#define CT7451_COLLECT_INTERVAL		2000
#define CT7451_OVER_TEMP_MAX		5

#define I2C				I2C1
#define SMBTO				1
#define CT7451_SLAVE_ADDR		(0x4c)

#define CT7451_ALERT			(0x22)
#define CT7451_SMBTO_MASK		(1 << 7)

#define CT7451_CONFIG_RD_ADDR		(0x03)
#define CT7451_CONFIG_WR_ADDR		(0x09)
#define CT7451_RANGE_MASK		(1 << 2)

#define CT7451_LT			(0)	/* Local temperature */
#define CT7451_RT			(1)	/* Remote temperature */

#define TEMP_COLLECT_INTERVAL		500

static unsigned long last_time_temp;

struct ct7451_reg {
	const int16_t read_ptr;			/* Read pointer */
	const int16_t write_ptr;		/* Write pointer */
	const int16_t init_value;		/* Initialization */
	uint8_t value;				/* Current value */
};

static uint8_t tmp_channel[2] = {2, 3};

static struct ct7451_ctx {
	int set_ptr;
	int local_temp, remote_temp;	/* Local and Remote temperature */
	int critical;			/* Critical temp */
	int over_temp;
	volatile uint8_t *read_ptr, *write_ptr;
	struct ct7451_reg map[CT7451_REG_MAX];
} ct7451_ctx[2];

void tmp_i2c_read(int idx)
{
	uint8_t tmp;
	int ret;

	pca9848_set(PCA9848_1, 1 << tmp_channel[idx] );

	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
 			CT7451_LT, &tmp);
	if(ret) 
		debug("ct7451%d_init read lt reg fail!(read)\n",idx);
 	ct7451_ctx[idx].local_temp = (int)tmp - 64;
	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
 			CT7451_RT, &tmp);
	if(ret) 
		debug("ct7451%d_init read lt reg fail!(read)\n",idx);
 	ct7451_ctx[idx].remote_temp = (int)tmp - 64;

	set_soc_temp(idx, round(ct7451_ctx[idx].remote_temp * 1.0732f - 12.739f));
	set_board_temp(idx, ct7451_ctx[idx].local_temp);
}

void ct7451_process(void)
{
	unsigned long current_time;

	current_time = tick_get();
	if (current_time - last_time_temp > TEMP_COLLECT_INTERVAL) {
		tmp_i2c_read(0);
		timer_udelay(10);
		tmp_i2c_read(1);
		timer_udelay(10);
		last_time_temp = current_time;
	}

}


void ct7451_init(int idx)
{
	uint8_t tmp;
	int ret;

	pca9848_set(PCA9848_1, 1 << tmp_channel[idx]);
	/* Enable smbus timeout */
	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_ALERT, &tmp);
	if(ret) 
		dbg_printf("ct7451%d_init enable smbus fail!(read)\n",idx);
	tmp |= CT7451_SMBTO_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR,
				    SMBTO, CT7451_ALERT, tmp);
	if(ret) 
		dbg_printf("ct7451%d_init enable smbus fail!(write)\n",idx);

	tmp |= CT7451_SMBTO_MASK;
	/* Enable extended mode */
	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_CONFIG_RD_ADDR, &tmp);
	if(ret) 
		dbg_printf("ct7451%d_init enable extended mode fail!(read)\n",idx);
	tmp |= CT7451_RANGE_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				    CT7451_CONFIG_WR_ADDR, tmp);
	if(ret) 
		dbg_printf("ct7451%d_init enable extended mode fail!(write)\n",idx);

	/* Wait until next conversion, ct7451 default conversion rate is 16, so
	 * it takes at most 62.5ms till next conversion */

	 mdelay(65);

	 last_time_temp = tick_get();

}
