#include <i2c_slave.h>
#include <i2c_master.h>
#include <loop.h>
#include <tick.h>
#include <chip.h>
#include <power.h>
#include <common.h>
#include <mon.h>
#include <stdlib.h>

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

#define TMP451_COLLECT_INTERVAL	1000

#define I2C			I2C1
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
static void tmp451_process(void)
{
	unsigned long current_time = tick_get();
	unsigned char tmp;
	int soc, board, critical;

	if (last_time == 0) {
		/* enable tmp451 smbus timeout, set tmp451 working at extended
		 * mode
		 */
		i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
					   TMP451_ALERT, &tmp);
		if ((tmp & TMP451_SMBTO_MASK) == 0) {
			/* tmp451 has not enabled smbus timeout feature,
			 * enable it */
			tmp |= TMP451_SMBTO_MASK;
			i2c_master_smbus_write_byte(I2C, TMP451_SLAVE_ADDR,
				SMBTO, TMP451_ALERT, tmp);
		}
		/* enable extended mode */
		i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
			TMP451_CONFIG_RD_ADDR, &tmp);
		tmp |= TMP451_RANGE_MASK;
		i2c_master_smbus_write_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
			TMP451_CONFIG_WR_ADDR, tmp);

		last_time = current_time;

		/* default temperature */
		tmp451_ctx.soc = 20 + 5;
		tmp451_ctx.board = 20;

		set_soc_temp(tmp451_ctx.soc - 5);
		set_board_temp(tmp451_ctx.board);
		return;
	}

	if (current_time - last_time < TMP451_COLLECT_INTERVAL)
		return;

	i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
				   TMP451_LT, &tmp);

	board = (int)tmp - 64;
	soc = board + 5;

	if (chip_is_enabled()) {
		i2c_master_smbus_read_byte(I2C, TMP451_SLAVE_ADDR, SMBTO,
					   TMP451_RT, &tmp);
		soc = (int)tmp - 64;
	}

	tmp451_ctx.soc = soc;
	tmp451_ctx.board = board;

	soc -= 5;

	set_soc_temp(soc);
	set_board_temp(board);

	if (get_hardware_version() == 0x12)
		critical = 120;
	else
		critical = 95;

	if (power_status() && soc > critical)
		power_off();

	last_time = current_time;
}

void tmp451_init(struct i2c_slave_ctx *i2c_slave_ctx)
{
	software_reset();
	i2c_slave_register(i2c_slave_ctx, &tmp451_slave);
	loop_add(tmp451_process);
}


