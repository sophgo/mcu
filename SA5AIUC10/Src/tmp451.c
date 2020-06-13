/*
 * tmp451.c
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */
#include "i2c_slave.h"
#include "main.h"

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

void tmp451_init(void)
{
	software_reset();
	i2c_slave_register(&i2c_ctx3, &tmp451_slave);
}

void tmp451_set_temp(int soc, int board)
{
	tmp451_ctx.soc = soc;
	tmp451_ctx.board = board;
}
