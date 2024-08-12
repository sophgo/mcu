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
	pca9848_set(PCA9848_1, 1 << tmp_channel[idx]);

	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
 			CT7451_LT, &tmp);
 	ct7451_ctx[idx].remote_temp = (int)tmp - 64;
	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
 			CT7451_RT, &tmp);
 	ct7451_ctx[idx].local_temp = (int)tmp - 64;

	set_soc_temp(idx, ct7451_ctx[idx].local_temp - 5);
	set_board_temp(idx, ct7451_ctx[idx].remote_temp);
}

void ct7451_process(void)
{
	tmp_i2c_read(0);
	tmp_i2c_read(1);
}


void ct7451_init(int idx)
{
	uint8_t tmp;

	pca9848_set(PCA9848_1, 1 << tmp_channel[idx]);
	/* Enable smbus timeout */
	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_ALERT, &tmp);
	tmp |= CT7451_SMBTO_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR,
				    SMBTO, CT7451_ALERT, tmp);

	/* Enable extended mode */
	i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_CONFIG_RD_ADDR, &tmp);
	tmp |= CT7451_RANGE_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				    CT7451_CONFIG_WR_ADDR, tmp);

	/* Wait until next conversion, ct7451 default conversion rate is 16, so
	 * it takes at most 62.5ms till next conversion */

	 mdelay(65);

}
