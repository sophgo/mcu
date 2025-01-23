#include <gd32e50x_i2c.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <i2c01_slave.h>
#include <timer.h>

#define BAUDRATE	(100 * 1000)

/* APB1 clock */
#define I2C01_INTERNAL_CLOCK	(90 * 1000 * 1000)

struct i2c01_reg {
	uint32_t cr1;
	uint32_t cr2;
	uint32_t oar1;
	uint32_t oar2;
	uint32_t dr;
	uint32_t sr1;
	uint32_t sr2;
	uint32_t ccr;
	uint32_t trise;
};

struct i2c01_isr_op;
typedef void (*isr_cb)(struct i2c01_slave_ctx *ctx);

struct i2c01_isr_op {
	isr_cb	cb;
	int		bit;
};

#ifdef BIT
#undef BIT
#define BIT(n)			(1UL << (n))
#endif

#define FIELD(s, n)		(((1UL << (n)) - 1) << (s))

#define CR1_SWRST		BIT(15)
#define CR1_ALERT		BIT(13)
#define CR1_PEC			BIT(12)
#define CR1_POS			BIT(11)
#define CR1_ACK			BIT(10)
#define CR1_STOP		BIT(9)
#define CR1_START		BIT(8)
#define CR1_NOSTRETCH		BIT(7)
#define CR1_ENGC		BIT(6)
#define CR1_ENPEC		BIT(5)
#define CR1_ENARP		BIT(4)
#define CR1_SMBTYPE		BIT(3)
#define CR1_SMBUS		BIT(1)
#define CR1_PE			BIT(0)

#define CR2_LAST		BIT(12)
#define CR2_DMAEN		BIT(11)
#define CR2_ITBUFEN		BIT(10)
#define CR2_ITEVTEN		BIT(9)
#define CR2_ITERREN		BIT(8)
#define CR2_FREQ		FIELD(0, 7)

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

/* nack is not supported on i2c0 and i2c1, i2c0 and i2c1 cannot generate stop
 * event when pattern NACK + STOP, that means a normal smbus register read
 * cannot get stop event, but before stop condition, master should generate NACK
 * first, so if NACK is get, just call stop call back
 */
static void isr_nackf_cb(struct i2c01_slave_ctx *ctx)
{
	if (ctx->slave && ctx->slave->stop)
		ctx->slave->stop(ctx->slave->priv);
	ctx->reg->sr1 &= ~BIT(10);
}

static void isr_rxne_cb(struct i2c01_slave_ctx *ctx)
{
	/* this will clear rxne bit */
	uint8_t data = ctx->reg->dr;

	if (ctx->slave && ctx->slave->write)
		ctx->slave->write(ctx->slave->priv, data);
}

static void isr_txe_cb(struct i2c01_slave_ctx *ctx)
{
	if (ctx->slave && ctx->slave->read)
		ctx->reg->dr = ctx->slave->read(ctx->slave->priv);
	else
		ctx->reg->dr = 0;
}

static void isr_stopf_cb(struct i2c01_slave_ctx *ctx)
{
	if (ctx->slave && ctx->slave->stop)
		ctx->slave->stop(ctx->slave->priv);
	ctx->reg->cr1 |= CR1_PE;
}

static inline struct i2c01_slave_op *find_slave(struct i2c01_slave_ctx *ctx,
						unsigned int addr)
{
	int i;
	struct i2c01_slave_op *slave;
	unsigned int mask;

	for (i = 0; i < ARRAY_SIZE(ctx->slave_list); ++i) {
		slave = ctx->slave_list[i];
		if (slave == NULL)
			continue;
		mask = ~slave->mask;
		if ((slave->addr & mask) == (addr & mask))
			return *(ctx->slave_list + i);
	}
	return NULL;
}

static void isr_addr_cb(struct i2c01_slave_ctx *ctx)
{
	/* sr1 has been read in isr, read sr2 should clear addr bit */
	unsigned int sr2 = ctx->reg->sr2;
	unsigned int addr;
	unsigned int dir;

	addr = (sr2 & BIT(7)) ?
		((ctx->reg->oar2 >> 1) & 0x3f) :
		((ctx->reg->oar1 >> 1) & 0x3f);

	dir = (sr2 & BIT(2)) ? I2C_SLAVE_READ : I2C_SLAVE_WRITE;

	ctx->dir = dir;
	ctx->slave = find_slave(ctx, addr);
	if (ctx->slave) {
		if (ctx->slave->match) {
			ctx->slave->match(ctx->slave->priv, dir);
		}
		if (dir == I2C_SLAVE_READ && ctx->slave->read) {
			ctx->reg->dr = ctx->slave->read(ctx->slave->priv);
		}
	}
}

#define ISR_TXE_SHIFT		7
#define ISR_RXNE_SHIFT		6
#define ISR_ADDR_SHIFT		1
#define ISR_NACK_SHIFT		10
#define ISR_STOP_SHIFT		4

static struct i2c01_isr_op i2c01_isr_table[] = {
	{isr_rxne_cb, ISR_RXNE_SHIFT},
	{isr_txe_cb, ISR_TXE_SHIFT},
	{isr_stopf_cb, ISR_STOP_SHIFT},
	{isr_addr_cb, ISR_ADDR_SHIFT},
	{isr_nackf_cb, ISR_NACK_SHIFT},
};

#define ERR_MASK (BIT(8) | BIT(9) | BIT(11) | BIT(12) | BIT(14) | BIT(15))

void i2c01_slave_isr(struct i2c01_slave_ctx *ctx)
{
	unsigned int sts;
	int i, j, isr;
	int p[] = {
		ISR_ADDR_SHIFT, ISR_RXNE_SHIFT, ISR_TXE_SHIFT,
		ISR_NACK_SHIFT, ISR_STOP_SHIFT,
	};

	sts = ctx->reg->sr1;
	if (sts & ERR_MASK) {
		printf("bus error occor\n");
		/* reset i2c controller */
		ctx->reg->cr1 &= ~CR1_PE;
		timer_udelay(10);
		ctx->reg->cr1 |= CR1_PE;
		return;
	}
	/* ADDR bit is cleared by reading sr1 then reading sr2, ADDR call back
	 * should read sr2 to judge which address is matched.
	 * BTF is cleared by reading sr1 followed by either a read or write in
	 * dr, if rx not empty or tx empty is raised, read or write call back
	 * should fill data to dr or get data from dr.
	 * STOPF is cleared by reading sr1 followed by a write to cr1
	 */

	for (i = 0; i < ARRAY_SIZE(p); ++i) {
		isr = p[i];
		if ((sts >> isr) & 1) {
			for (j = 0; j < ARRAY_SIZE(i2c01_isr_table); ++j) {
				if (i2c01_isr_table[j].bit == isr) {
					if (i2c01_isr_table[j].cb)
						i2c01_isr_table[j].cb(ctx);
				}
			}
		}
	}
}

int i2c01_slave_init(struct i2c01_slave_ctx *ctx, void *reg,
		   int oa1, int oa2, int oa2mask)
{
	int i;
	uint32_t i2c = (uint32_t)reg;

	ctx->reg = reg;

	/* prepare hardware first */
	i2c_deinit(i2c);
	ctx->reg->cr2 &= ~FIELD(0, 7);
	ctx->reg->cr2 |= I2C01_INTERNAL_CLOCK;
	ctx->reg->cr1 &= ~CR1_NOSTRETCH;
	i2c_clock_config(i2c, BAUDRATE, I2C_DTCY_2);

	ctx->isr_irq_mask = 0;
	ctx->dir = I2C_SLAVE_WRITE;
	/* oa1 enable, 7 bit mode, 7 bit address */
	ctx->reg->oar1 &= ~FIELD(10, 5);
	if (oa1 > 0)
		ctx->reg->oar1 |= oa1 << 1;

	/* oa2 enable, oa2 address mask, 7 bit address */
	ctx->reg->oar2 = 0;
	if (oa2 > 0)
		ctx->reg->oar2 = (oa2 << 1) | 1;

	for (i = 0; i < ARRAY_SIZE(i2c01_isr_table); ++i)
		ctx->isr_irq_mask |= (1 << i2c01_isr_table[i].bit);

	return 0;
}

int i2c01_slave_register(struct i2c01_slave_ctx *ctx,
			 struct i2c01_slave_op *slave)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ctx->slave_list); ++i) {
		if (ctx->slave_list[i] == NULL) {
			ctx->slave_list[i] = slave;
			return 0;
		}
	}

	return -1;
}

int i2c01_slave_start(struct i2c01_slave_ctx *ctx)
{
	ctx->slave = NULL;
	ctx->reg->cr1 |= CR1_PE | CR1_ACK;
	ctx->reg->cr2 |= CR2_ITBUFEN | CR2_ITEVTEN | CR2_ITERREN;
	return 0;
}

int i2c01_slave_reset(struct i2c01_slave_ctx *ctx)
{
	struct i2c01_slave_op *op;
	int i;

	ctx->reg->cr1 &= ~CR1_PE;

	for (i = 0; i < ARRAY_SIZE(ctx->slave_list); ++i) {
		op = ctx->slave_list[i];
		if (op && op->reset)
			op->reset(op->priv);
	}

	ctx->reg->cr1 |= CR1_PE;
	return 0;
}

