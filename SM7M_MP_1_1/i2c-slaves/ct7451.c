/*
 * ct7451.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <stdio.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <loop.h>
#include <tick.h>
#include <chip.h>
#include <power.h>
#include <common.h>
#include <mon.h>
#include <stdlib.h>
#include <debug.h>
#include <mcu.h>

#define CT7451_REG_MAX	(22)

#define PTRNA	(-1)	/* Operation not apply */
#define PORNA	(-1)	/* Power-on reset value not apply */

#define CT7451_COLLECT_INTERVAL		2000
#define CT7451_OVER_TEMP_MAX		5
/* consecutive I2C read failures before temperatures are cleared to 0.
 * A reading of 0 never trips the over-temp guard (local_temp > critical_temp),
 * so a stuck/unreachable sensor does not falsely power off the board.
 * sample interval is 2s, so 5 failures ~= 10s */
#define CT7451_READ_FAIL_MAX		5

#define I2C				I2C2
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

static struct ct7451_ctx {
	int set_ptr;
	int local_temp, remote_temp;	/* Local and Remote temperature */
	int critical;			/* Critical temp */
	int over_temp;
	int read_fail_cnt;		/* consecutive I2C read failures */
	volatile uint8_t *read_ptr, *write_ptr;
	struct ct7451_reg map[CT7451_REG_MAX];
} ct7451_ctx = {
	.map = {
		{0x00, PTRNA, 0x00},
		{0x01, PTRNA, 0x00},
		{0x02, PTRNA, PORNA},
		{0x03, 0x09, 0x04},	/* Enable extended mode by default */
		{0x04, 0x0a, 0x08},
		{0x05, 0x0b, 0x55},
		{0x06, 0x0c, 0x00},
		{0x07, 0x0d, 0x55},
		{0x08, 0x0e, 0x00},
		{PTRNA, 0x0f, PORNA},
		{0x10, PTRNA, 0x00},
		{0x11, 0x11, 0x00},
		{0x12, 0x12, 0x00},
		{0x13, 0x13, 0x00},
		{0x14, 0x14, 0x00},
		{0x15, PTRNA, 0x00},
		{0x19, 0x19, 0x6c},
		{0x20, 0x20, 0x55},
		{0x21, 0x21, 0x0a},
		{0x22, 0x22, 0x01},
		{0x23, 0x23, 0x00},
		{0xfe, PTRNA, 0x59},
	},
};

static void ct7451_software_reset(void)
{
	struct ct7451_reg *p;
	for (p = ct7451_ctx.map; p != ct7451_ctx.map + CT7451_REG_MAX; ++p)
		if (p->init_value == PORNA)
			p->value = 0;
		else
			p->value = p->init_value;
	ct7451_ctx.set_ptr = 0;
	ct7451_ctx.read_ptr = ct7451_ctx.write_ptr = &(ct7451_ctx.map[0].value);
}

static void ct7451_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		ct7451_ctx.set_ptr = 1;
}

static void ct7451_write(void *priv, uint8_t data)
{
	if (ct7451_ctx.set_ptr) {
		ct7451_ctx.set_ptr = 0;
		switch (data){
		case 0:
			ct7451_ctx.read_ptr = &(ct7451_ctx.map[0].value);
			ct7451_ctx.write_ptr = NULL;
			return;
		case 1:
			ct7451_ctx.read_ptr = &(ct7451_ctx.map[1].value);
			ct7451_ctx.write_ptr = NULL;
			return;
		}

		struct ct7451_reg *p;
		for (p = ct7451_ctx.map; p != ct7451_ctx.map + CT7451_REG_MAX;
		     ++p) {
			if (p->read_ptr == data) {
				ct7451_ctx.read_ptr = &p->value;
				if (p->read_ptr == p->write_ptr)
					ct7451_ctx.write_ptr = ct7451_ctx.read_ptr;
				return;
			}
			if (p->write_ptr == data) {
				ct7451_ctx.read_ptr = NULL;
				ct7451_ctx.write_ptr = &p->value;
				return;
			}
		}
		ct7451_ctx.read_ptr = NULL;
		ct7451_ctx.write_ptr = NULL;
		return;
	}
	if (ct7451_ctx.write_ptr) {
		*(ct7451_ctx.write_ptr) = data;
		return;
	}
	// error handling
}

static uint8_t real_temp_to_reg(int temp)
{
	int is_ex_mode = ct7451_ctx.map[3].value & (1 << 2);
	uint8_t reg;

	if (is_ex_mode) {
		if (temp < -64)
			temp = -64;
		else if (temp > 191)
			temp = 191;

		reg = temp + 64;
	} else {
		if (temp < 0)
			reg = 0;
		else if (temp > 127)
			reg = 127;
		else
			reg = temp;
	}
	return reg;
}

static uint8_t ct7451_read(void *priv)
{
	volatile uint8_t * volatile read_ptr = ct7451_ctx.read_ptr;

	if (read_ptr == &(ct7451_ctx.map[0].value)) {
		/* board */
		return real_temp_to_reg(ct7451_ctx.remote_temp);
	} else if (read_ptr == &(ct7451_ctx.map[1].value)) {
		/* soc */
		return real_temp_to_reg(ct7451_ctx.local_temp);
	} else if (read_ptr) {
		return *(ct7451_ctx.read_ptr);
	}

	// error handling
	return 0;
}
static struct i2c_slave_op tmp451_slave = {
	.addr = 0x6b,	/* tmp451 common slave address */
	.match = ct7451_match,
	.write = ct7451_write,
	.read = ct7451_read,
};


static unsigned long last_time;

static int ct7451_update_temp(void)
{
	uint8_t tmp;
	int ret;
	int fail = 0;

	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
			CT7451_LT, &tmp);
	if (ret) {
		debug("ct7451 read remote temp timeout!(%d)\n", ret);
		fail = 1;
	} else {
		debug("ct7451 read remote temp success! %d\n", tmp);
		ct7451_ctx.remote_temp = (int)tmp - 64;
	}

	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
			CT7451_RT, &tmp);
	if (ret) {
		debug("ct7451 read local temp fail!(%d)\n", ret);
		fail = 1;
	} else {
		debug("ct7451 read local temp success! %d\n", tmp);
		ct7451_ctx.local_temp = (int)tmp - 64;
	}

	if (fail) {
		if (++ct7451_ctx.read_fail_cnt >= CT7451_READ_FAIL_MAX) {
			/* sensor unreachable for too long: invalidate
			 * temperatures. 0 never trips over-temp guard so
			 * this avoids false power-off while exposing the
			 * failure as an obvious bogus value. */
			ct7451_ctx.remote_temp = 0;
			ct7451_ctx.local_temp = 0;
		}
	} else {
		ct7451_ctx.read_fail_cnt = 0;
	}

	set_soc_temp(ct7451_ctx.local_temp - 5);
	set_board_temp(ct7451_ctx.remote_temp);

	return fail ? -1 : 0;
}

int ct7451_get_temp(int *remote_temp, int *local_temp)
{
	int ret = ct7451_update_temp();
	*remote_temp = ct7451_ctx.remote_temp;
	*local_temp = ct7451_ctx.local_temp - 5;
	return ret;
}

static void ct7451_process(void)
{
	unsigned long current_time = tick_get();
	int local_temp, remote_temp;
	int ret;

	if (current_time - last_time < CT7451_COLLECT_INTERVAL)
		return;

	last_time = current_time;

	ret = ct7451_get_temp(&remote_temp, &local_temp);

	/* during read failure keep the last valid temperatures for the
	 * over-temp guard (and 0 once failures persist), but skip the
	 * power-on-back decision so a bogus 0 cannot falsely re-power
	 * the board before it has cooled down. */
	if (get_needpoweron_satus() == 1 && !ret) {
		if (local_temp < get_repoweron_temp() && remote_temp < 80) {
			chip_popd_reset_end();
			clr_needpoweron();
		}
	}

	if (!chip_is_enabled()) {
		ct7451_ctx.over_temp = 0;
		return;
	}

	if (local_temp > get_critical_temp()) {
		++ct7451_ctx.over_temp;
		if (ct7451_ctx.over_temp > CT7451_OVER_TEMP_MAX) {
			chip_disable();
			power_off();
			ct7451_ctx.over_temp = 0;
			printf("Over Temperature, Board ");
			if (get_critical_action() == CRITICAL_ACTION_REBOOT) {
				set_needpoweron();
				printf("Rebooted\n");
			} else {
				printf("Powered OFF\n");
			}

		}
	}
}

void ct7451_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	uint8_t tmp;
	int ret;

	/* Enable smbus timeout */
	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_ALERT, &tmp);
	if (ret)
		debug("ct7451_init read alert reg fail!(%d)\n", ret);
	tmp |= CT7451_SMBTO_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR,
				    SMBTO, CT7451_ALERT, tmp);

	/* Enable extended mode */
	ret = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				   CT7451_CONFIG_RD_ADDR, &tmp);
	if (ret)
		debug("ct7451_init read config reg fail!(%d)\n", ret);
	tmp |= CT7451_RANGE_MASK;
	i2c_master_smbus_write_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
				    CT7451_CONFIG_WR_ADDR, tmp);

	/* Wait until next conversion, ct7451 default conversion rate is 16, so
	 * it takes at most 62.5ms till next conversion */

	mdelay(65);

	ct7451_update_temp();

	last_time = tick_get();
	ct7451_software_reset();

	i2c_slave_register(i2c_slave_ctx, &tmp451_slave);
	loop_add(ct7451_process);
}
