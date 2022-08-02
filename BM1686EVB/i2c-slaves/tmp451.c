#include <stdio.h>
#include <i2c_slave.h>
#include <i2c_master.h>
#include <loop.h>
#include <tick.h>
#include <chip.h>
#include <power.h>
#include <common.h>
#include <mon.h>
#include <stdlib.h>
#include <debug.h>
#include <mcu.h>

#define TMP451_REG_MAX	(23)

#define PTRNA	(-1)	/* operation not apply */
#define PORNA	(-1)	/* power-on reset value not apply */

struct tmp451_reg {
	const int16_t rptr;	/* pointer read */
	const int16_t wptr;	/* pointer write */
	const int16_t por;	/* power-on reset */
	uint8_t value;		/* current value */
};

static struct tmp451_ctx {
	int set_ptr;
	int soc, board;	/* soc and board temperature */
	int critical;	/* critical temp */
	int overtemp;
	volatile uint8_t *rptr, *wptr;
	struct tmp451_reg map[TMP451_REG_MAX];
} tmp451_ctx = {
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
		{0x24, 0x24, 0x00},
		{0xfe, PTRNA, 0x55},
	},
};

static void software_reset(void)
{
	struct tmp451_reg *p;
	for (p = tmp451_ctx.map; p != tmp451_ctx.map + TMP451_REG_MAX; ++p)
		if (p->por == PORNA)
			p->value = 0;
		else
			p->value = p->por;
	tmp451_ctx.set_ptr = 0;
	tmp451_ctx.rptr = tmp451_ctx.wptr = &(tmp451_ctx.map[0].value);
}

static void tmp451_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE)
		tmp451_ctx.set_ptr = 1;
}

static void tmp451_write(void *priv, uint8_t data)
{
	if (tmp451_ctx.set_ptr) {
		tmp451_ctx.set_ptr = 0;
		switch (data){
		case 0:
			tmp451_ctx.rptr = &(tmp451_ctx.map[0].value);
			tmp451_ctx.wptr = NULL;
			return;
		case 1:
			tmp451_ctx.rptr = &(tmp451_ctx.map[1].value);
			tmp451_ctx.wptr = NULL;
			return;
		}

		struct tmp451_reg *p;
		for (p = tmp451_ctx.map; p != tmp451_ctx.map + TMP451_REG_MAX;
		     ++p) {
			if (p->rptr == data) {
				tmp451_ctx.rptr = &p->value;
				if (p->rptr == p->wptr)
					tmp451_ctx.wptr = tmp451_ctx.rptr;
				return;
			}
			if (p->wptr == data) {
				tmp451_ctx.rptr = NULL;
				tmp451_ctx.wptr = &p->value;
				return;
			}
		}
		tmp451_ctx.rptr = NULL;
		tmp451_ctx.wptr = NULL;
		return;
	}
	if (tmp451_ctx.wptr) {
		*(tmp451_ctx.wptr) = data;
		return;
	}
	// error handling
}

static uint8_t real_temp_to_reg(int temp)
{
	int is_ex_mode = tmp451_ctx.map[3].value & (1 << 2);
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

static uint8_t tmp451_read(void *priv)
{
	volatile uint8_t * volatile rptr = tmp451_ctx.rptr;

	if (rptr == &(tmp451_ctx.map[0].value)) {
		/* board */
		return real_temp_to_reg(tmp451_ctx.board);
	} else if (rptr == &(tmp451_ctx.map[1].value)) {
		/* soc */
		return real_temp_to_reg(tmp451_ctx.soc);
	} else if (rptr) {
		return *(tmp451_ctx.rptr);
	}

	// error handling
	return 0;
}

static struct i2c_slave_op tmp451_slave = {
	.addr = 0x6b,	/* tmp451 common slave address */
	.match = tmp451_match,
	.write = tmp451_write,
	.read = tmp451_read,
};

#define TMP451_COLLECT_INTERVAL	2000
#define TMP451_OVERTEMP_MAX	5

#define I2C			I2C2
#define SMBTO			1
#define TMP451_SLAVE_ADDR	(0x4c)

#define TMP451_ALERT		(0x22)
#define TMP451_SMBTO_MASK	(1 << 7)

#define TMP451_CONFIG_RD_ADDR	(0x03)
#define TMP451_CONFIG_WR_ADDR	(0x09)
#define TMP451_RANGE_MASK	(1 << 2)

/* local temperature */
#define TMP451_LT	(0)
/* remote temperature */
#define TMP451_RT	(1)

static unsigned long last_time;

static void tmp451_update_temp(void)
{
	uint8_t tmp;

	i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				   TMP451_LT, &tmp);
	tmp451_ctx.board = (int)tmp - 64;

	i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				   TMP451_RT, &tmp);
	tmp451_ctx.soc = (int)tmp - 64;

	set_soc_temp(tmp451_ctx.soc - 5);
	set_board_temp(tmp451_ctx.board);

}

void tmp451_get_temp(int *board, int *soc)
{
	tmp451_update_temp();
	*board = tmp451_ctx.board;
	*soc = tmp451_ctx.soc - 5;
}

static void tmp451_process(void)
{
	unsigned long current_time = tick_get();
	int soc, board;

	if (current_time - last_time < TMP451_COLLECT_INTERVAL)
		return;

	last_time = current_time;

	tmp451_get_temp(&board, &soc);

	if (get_needpoweron_satus() == 1) {
		if (soc < get_repoweron_temp() && board < 80) {
			chip_popd_reset_end();
			clr_needpoweron();
		}
	}

	if (!chip_is_enabled()) {
		tmp451_ctx.overtemp = 0;
		return;
	}

	if (soc > get_critical_temp()) {
		++tmp451_ctx.overtemp;
		if (tmp451_ctx.overtemp > TMP451_OVERTEMP_MAX) {
			chip_disable();
			power_off();
			tmp451_ctx.overtemp = 0;
			printf("temp is over, board will");
			if (get_critical_action() == CRITICAL_ACTION_REBOOT) {
				set_needpoweron();
				printf("reboot\n");
			} else {
				printf("poweroff\n");
			}

		}
	}
}

void tmp451_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	uint8_t tmp;


	/* enable smbus timeout */
	i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				   TMP451_ALERT, &tmp);
	tmp |= TMP451_SMBTO_MASK;
	i2c_master_smbus_write_byte(I2C, TMP451_SLAVE_ADDR,
				    SMBTO, TMP451_ALERT, tmp);

	/* enable extended mode */
	i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				   TMP451_CONFIG_RD_ADDR, &tmp);
	tmp |= TMP451_RANGE_MASK;
	i2c_master_smbus_write_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				    TMP451_CONFIG_WR_ADDR, tmp);

	/* wait untill next conversion, tmp451 default conversion rate is 16, so
	 * it takes at most 62.5ms till next conversion
	 */
	mdelay(65);

	tmp451_update_temp();

	last_time = tick_get();
	software_reset();
	i2c_slave_register(i2c_slave_ctx, &tmp451_slave);
	loop_add(tmp451_process);
}

