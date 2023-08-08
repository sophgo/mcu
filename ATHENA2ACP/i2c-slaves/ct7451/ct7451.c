/*
 * ct7451.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <stdio.h>
#include <i2c/i2c_slave/i2c_slave.h>
#include <i2c/i2c_master/i2c_master.h>
#include <loop/loop.h>
#include <tick/tick.h>
#include <chip/chip.h>
#include <power/power.h>
#include <common/common.h>
#include <adc/adc.h>
#include <stdlib.h>
#include <debug.h>
#include <i2c-slaves/mcu/mcu.h>

#define CT7451_REG_MAX	(22)

#define PTRNA	(-1)	/* operation not apply */
#define PORNA	(-1)	/* power-on reset value not apply */

struct ct7451_reg {
	const int16_t rptr;	/* pointer read */
	const int16_t wptr;	/* pointer write */
	const int16_t por;	/* power-on reset */
	uint8_t value;		/* current value */
};

static struct ct7451_ctx {
	int set_ptr;
	int soc, board;	/* soc and board temperature */
	int critical;	/* critical temp */
	int over_temp;
	volatile uint8_t *rptr, *wptr;
	struct ct7451_reg map[CT7451_REG_MAX];
} ct7451_ctx = {
	.map = {
		{0x00, PTRNA, 0x00},
		{0x01, PTRNA, 0x00},
		{0x02, PTRNA, PORNA},
		{0x03, 0x09, 0x04},	/* enable extended mode by default */
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

static void software_reset(void)
{
	struct ct7451_reg *p;
	for (p = ct7451_ctx.map; p != ct7451_ctx.map + CT7451_REG_MAX; ++p)
		if (p->por == PORNA)
			p->value = 0;
		else
			p->value = p->por;
	ct7451_ctx.set_ptr = 0;
	ct7451_ctx.rptr = ct7451_ctx.wptr = &(ct7451_ctx.map[0].value);
}

#define CT7451_COLLECT_INTERVAL	2000
#define CT7451_OVER_TEMP_MAX	5

#define I2C			I2C1
#define SMBTO			1
#define CT7451_SLAVE_ADDR	(0x4c)

#define CT7451_ALERT		(0x22)
#define CT7451_SMBTO_MASK	(1 << 7)

#define CT7451_CONFIG_RD_ADDR	(0x03)
#define CT7451_CONFIG_WR_ADDR	(0x09)
#define CT7451_RANGE_MASK	(1 << 2)

/* local temperature */
#define CT7451_LT	(0)
/* remote temperature */
#define CT7451_RT	(1)

static unsigned long last_time;
unsigned int counter;

static void ct7451_update_temp(void)
{
	uint8_t tmp;
//	int status;

	counter++;

//	status = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
//				   CT7451_LT, &tmp);
        i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
                                            CT7451_LT, &tmp);
//	debug("\n[%u]\n", counter);
//	debug("SoC Origin: [%d] %d\n", status, tmp);

	ct7451_ctx.board = (int)tmp - 64;

//	status = i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
//				   CT7451_RT, &tmp);
        i2c_master_smbus_read_byte(I2C, CT7451_SLAVE_ADDR, SMBTO,
                                            CT7451_RT, &tmp);

//	debug("Board Origin: [%d] %d\n", status, tmp);

	ct7451_ctx.soc = (int)tmp - 64;

	set_soc_temp(ct7451_ctx.soc - 5);
	set_board_temp(ct7451_ctx.board);
//	debug("SoC Set: [%u]\n", ct7451_ctx.soc - 5);
//	debug("Board Set: [%u]\n", ct7451_ctx.board);
}

void ct7451_get_temp(int *board, int *soc)
{
	ct7451_update_temp();
	*board = ct7451_ctx.board;
	*soc = ct7451_ctx.soc - 5;
}

static void ct7451_process(void)
{
	unsigned long current_time = tick_get();
	int soc, board;

	if (current_time - last_time < CT7451_COLLECT_INTERVAL)
		return;

	last_time = current_time;

	ct7451_get_temp(&board, &soc);

	if (get_need_power_on_status() == 1) {
		if (soc < get_repower_on_temp() && board < 80) {
			chip_popd_reset_end();
			clr_need_power_on();
		}
	}

	if (!chip_is_enabled()) {
		ct7451_ctx.over_temp = 0;
		return;
	}

	if (soc > get_critical_temp()) {
		++ct7451_ctx.over_temp;
		if (ct7451_ctx.over_temp > CT7451_OVER_TEMP_MAX) {
			chip_disable();
			power_off();
			ct7451_ctx.over_temp = 0;
			printf("Over Temperature, Board ");
			if (get_critical_action() == CRITICAL_ACTION_REBOOT) {
				set_need_power_on();
				printf("Rebooted\n");
			} else {
				printf("Powered OFF\n");
			}

		}
	}
}

void ct7451_init(void)
{
	uint8_t tmp;

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
	 * it takes at most 62.5ms till next conversion
	 */

	tick_delay_ms(65);

	ct7451_update_temp();

	last_time = tick_get();
	software_reset();
	loop_add(ct7451_process);
}

