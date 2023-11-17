#include <stdio.h>
#include <i2c_master.h>
#include <i2c_slave.h>
#include <tick.h>
#include <power.h>
#include <common.h>
#include <stdlib.h>
#include <debug.h>
#include <mcu.h>
#include <chip.h>
#include <loop.h>
#include <wdt.h>

#define NCT218_REG_MAX	(21)

#define PTRNA	(-1)	/* operation not apply */
#define PORNA	(-1)	/* power-on reset value not apply */

struct nct218_reg {
	const int16_t rptr;	/* pointer read */
	const int16_t wptr;	/* pointer write */
	const int16_t por;	/* power-on reset */
	uint8_t value;		/* current value */
};

static struct nct218_ctx {
	int set_ptr;
	int soc, board;	/* soc and board temperature */
	int critical;	/* critical temp */
	int overtemp;
	volatile uint8_t *rptr, *wptr;
	struct nct218_reg map[NCT218_REG_MAX];
} nct218_ctx = {
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
		{0x19, 0x19, 0x6c},
		{0x20, 0x20, 0x55},
		{0x21, 0x21, 0x0a},
		{0x22, 0x22, 0x01},
		{0xfe, PTRNA, 0x1a},
		{0xff, PTRNA, 0x00},
	},
};

static void software_reset(void)
{
	struct nct218_reg *p;
	for (p = nct218_ctx.map; p != nct218_ctx.map + NCT218_REG_MAX; ++p)
		if (p->por == PORNA)
			p->value = 0;
		else
			p->value = p->por;
	nct218_ctx.set_ptr = 0;
	nct218_ctx.rptr = nct218_ctx.wptr = &(nct218_ctx.map[0].value);
}

static void nct218_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		nct218_ctx.set_ptr = 1;
}

static void nct218_write(void *priv, uint8_t data)
{
	if (nct218_ctx.set_ptr) {
		nct218_ctx.set_ptr = 0;
		switch (data){
		case 0:
			nct218_ctx.rptr = &(nct218_ctx.map[0].value);
			nct218_ctx.wptr = NULL;
			return;
		case 1:
			nct218_ctx.rptr = &(nct218_ctx.map[1].value);
			nct218_ctx.wptr = NULL;
			return;
		}

		struct nct218_reg *p;
		for (p = nct218_ctx.map; p != nct218_ctx.map + NCT218_REG_MAX;
		     ++p) {
			if (p->rptr == data) {
				nct218_ctx.rptr = &p->value;
				if (p->rptr == p->wptr)
					nct218_ctx.wptr = nct218_ctx.rptr;
				return;
			}
			if (p->wptr == data) {
				nct218_ctx.rptr = NULL;
				nct218_ctx.wptr = &p->value;
				return;
			}
		}
		nct218_ctx.rptr = NULL;
		nct218_ctx.wptr = NULL;
		return;
	}
	if (nct218_ctx.wptr) {
		*(nct218_ctx.wptr) = data;
		return;
	}
	// error handling
}

static uint8_t real_temp_to_reg(int temp)
{
	int is_ex_mode = nct218_ctx.map[3].value & (1 << 2);
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

static uint8_t nct218_read(void *priv)
{
	volatile uint8_t * volatile rptr = nct218_ctx.rptr;

	if (rptr == &(nct218_ctx.map[0].value)) {
		/* board */
		return real_temp_to_reg(nct218_ctx.board);
	} else if (rptr == &(nct218_ctx.map[1].value)) {
		/* soc */
		return real_temp_to_reg(nct218_ctx.soc);
	} else if (rptr) {
		return *(nct218_ctx.rptr);
	}

	// error handling
	return 0;
}

static struct i2c_slave_op nct218_slave = {
	.addr = 0x6b,	/* nct218 common slave address */
	.match = nct218_match,
	.write = nct218_write,
	.read = nct218_read,
};

#define NCT218_COLLECT_INTERVAL	2000
#define NCT218_OVERTEMP_MAX	5

#define I2C			I2C1
#define SMBTO			1
#define NCT218_SLAVE_ADDR	(0x4c)

#define NCT218_ALERT		(0x22)
#define NCT218_SMBTO_MASK	(1 << 7)

#define NCT218_CONFIG_RD_ADDR	(0x03)
#define NCT218_CONFIG_WR_ADDR	(0x09)
#define NCT218_RANGE_MASK	(1 << 2)

/* local temperature */
#define NCT218_LT	(0)
/* remote temperature */
#define NCT218_RT	(1)

static unsigned long last_time;

static void nct218_update_temp(void)
{
	uint8_t tmp;

	i2c_master_smbus_read_byte(I2C, NCT218_SLAVE_ADDR, SMBTO,
				   NCT218_LT, &tmp);
	nct218_ctx.board = (int)tmp - 64;

	i2c_master_smbus_read_byte(I2C, NCT218_SLAVE_ADDR, SMBTO,
				   NCT218_RT, &tmp);
	nct218_ctx.soc = (int)tmp - 64;

	set_soc_temp(nct218_ctx.soc - 5);
	set_board_temp(nct218_ctx.board);
}

void nct218_get_temp(int *board, int *soc)
{
	nct218_update_temp();
	*board = nct218_ctx.board;
	*soc = nct218_ctx.soc - 5;
}

void nct218_process(void)
{
	unsigned long current_time = tick_get();
	int soc, board;

	if (current_time - last_time < NCT218_COLLECT_INTERVAL)
		return;

	last_time = current_time;

	nct218_get_temp(&board, &soc);

	if (get_needpoweron_satus() == 1) {
		if (soc < get_repoweron_temp() && board < 80) {
			chip_popd_reset_end();
			clr_needpoweron();
		}
	}

	if (!chip_is_enabled()) {
		nct218_ctx.overtemp = 0;
		return;
	}

	if (soc > get_critical_temp()) {
		++nct218_ctx.overtemp;
		if (nct218_ctx.overtemp > NCT218_OVERTEMP_MAX) {
			chip_disable();
			power_off();
			wdt_reset();
			nct218_ctx.overtemp = 0;
			printf("temp is over, board will ");
			if (get_critical_action() == CRITICAL_ACTION_REBOOT) {
				set_needpoweron();
				printf("reboot\n");
			} else {
				printf("poweroff\n");
			}

		}
	}
}

void nct218_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	uint8_t tmp;

	/* enable smbus timeout */
	i2c_master_smbus_read_byte(I2C, NCT218_SLAVE_ADDR, SMBTO,
				   NCT218_ALERT, &tmp);
	tmp |= NCT218_SMBTO_MASK;
	i2c_master_smbus_write_byte(I2C, NCT218_SLAVE_ADDR,
				    SMBTO, NCT218_ALERT, tmp);

	/* enable extended mode */
	i2c_master_smbus_read_byte(I2C, NCT218_SLAVE_ADDR, SMBTO,
				   NCT218_CONFIG_RD_ADDR, &tmp);
	tmp |= NCT218_RANGE_MASK;
	i2c_master_smbus_write_byte(I2C, NCT218_SLAVE_ADDR, SMBTO,
				    NCT218_CONFIG_WR_ADDR, tmp);

	/* wait untill next conversion, nct218 default conversion rate is 16, so
	 * it takes at most 62.5ms till next conversion
	 */
	mdelay(65);
	nct218_update_temp();

	last_time = tick_get();
	software_reset();
	i2c_slave_register(i2c_slave_ctx, &nct218_slave);
	loop_add(nct218_process);
}

