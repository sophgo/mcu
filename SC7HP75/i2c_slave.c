#include <gd32e50x_i2c.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <i2c_slave.h>

#define BAUDRATE	(100 * 1000)

struct i2c_reg {
	uint32_t cr1;
	uint32_t cr2;
	uint32_t oar1;
	uint32_t oar2;
	uint32_t timingr;
	uint32_t timeoutr;
	uint32_t isr;
	uint32_t icr;
	uint32_t pecr;
	uint32_t rxdr;
	uint32_t txdr;
};

struct i2c_isr_op;
typedef void (*isr_cb)(struct i2c_slave_ctx *ctx);
typedef void (*isr_clr)(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op);

struct i2c_isr_op {
	isr_cb	cb;
	isr_clr	clr;
	int		bit;
};

#define CR1_SBC			(1 << 16)
#define CR1_RXDMAEN		(1 << 15)
#define CR1_TXDMAEN		(1 << 14)

#define CR1_NOSTRETCH		(1 << 17)
#define CR1_ERRIE		(1 << 7)
#define CR1_TCIE		(1 << 6)
#define CR1_STOPIE		(1 << 5)
#define CR1_NACKIE		(1 << 4)
#define CR1_ADDRIE		(1 << 3)
#define CR1_RXIE		(1 << 2)
#define CR1_TXIE		(1 << 1)

#define CR1_IES_MASK	(0xfe)

#define CR1_PE			(1 << 0)

#define ISR_ADDCODE_SHIFT	(17)
#define ISR_ADDCODE_MASK	(0x7f << ISR_ADDCODE_SHIFT)
#define ISR_DIR_SHIFT		(16)
#define ISR_DIR_MASK		(1 << ISR_DIR_SHIFT)
#define ISR_DIR_WRITE		I2C_SLAVE_WRITE
#define ISR_DIR_READ		I2C_SLAVE_READ
#define ISR_BUSY_SHIFT		(15)
#define ISR_STOPF			(0x1UL << 5)
#define ISR_NACKF			(0x1UL << 4)
#define ISR_BUSY			(0x1 << ISR_BUSY_SHIFT)
#define ISR_TXE				(1)

static unsigned int i2c_intr_disable(struct i2c_slave_ctx *ctx)
{
	unsigned int old = ctx->reg->cr1;
	ctx->reg->cr1 &= ~CR1_IES_MASK;
	return old;
}

static void i2c_intr_restore(struct i2c_slave_ctx *ctx, unsigned int old)
{
	ctx->reg->cr1 |= old & CR1_IES_MASK;
}

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define ISR_OP_DEFAULT(name)				\
void isr_##name##_cb(struct i2c_slave_ctx *ctx) {}	\
void isr_##name##_clr(struct i2c_slave_ctx *ctx,	\
		      struct i2c_isr_op *isr_op)	\
{							\
	ctx->reg->icr |= 1 << isr_op->bit;		\
}

//ISR_OP_DEFAULT(rxne)
//ISR_OP_DEFAULT(txis)
//ISR_OP_DEFAULT(stopf)
ISR_OP_DEFAULT(tcr)
ISR_OP_DEFAULT(tc)
//ISR_OP_DEFAULT(addr)
//ISR_OP_DEFAULT(nackf)
ISR_OP_DEFAULT(berr)
ISR_OP_DEFAULT(arlo)
ISR_OP_DEFAULT(ovr)
ISR_OP_DEFAULT(pecerr)
ISR_OP_DEFAULT(timeout)
ISR_OP_DEFAULT(alert)

void isr_nackf_cb(struct i2c_slave_ctx *ctx)
{
	if (ctx->slave && ctx->slave->nack)
		ctx->slave->nack(ctx->slave->priv);
}
void isr_nackf_clr(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op)
{
	ctx->reg->icr |= 1 << isr_op->bit;
	ctx->reg->txdr = 0x00U;
	/* This bit can be written to 1 by software in order to flush
	 * the transmit data register I2C_TXDR.
	*/
	ctx->reg->isr |= ISR_TXE;
}

void isr_rxne_clr(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op)
{
	/* do nothing, read rxdr will clear this bit */
}

void isr_rxne_cb(struct i2c_slave_ctx *ctx)
{
	/* this will clear rxne bit */
	uint8_t data = ctx->reg->rxdr;

	if (ctx->slave && ctx->slave->write)
		ctx->slave->write(ctx->slave->priv, data);
}

void isr_txis_cb(struct i2c_slave_ctx *ctx)
{
	ctx->reg->cr1 &=~CR1_TXIE; /*Disable transmit IT*/
	if (ctx->slave && ctx->slave->read)
		ctx->reg->txdr = ctx->slave->read(ctx->slave->priv);
	else
		ctx->reg->txdr = 0;
}

void isr_txis_clr(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op)
{
	/* this bit will auto clear when txdr write to some data */
}

void isr_stopf_cb(struct i2c_slave_ctx *ctx)
{
	/* This bit can be written to 1 by software in order to flush the
	 * transmit data register I2C_TXDR.
	 */
	ctx->reg->isr |= ISR_TXE;
	if (ctx->slave && ctx->slave->stop)
		ctx->slave->stop(ctx->slave->priv);
}

void isr_stopf_clr(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op)
{
	ctx->reg->icr |= 1 << isr_op->bit;
}

static inline struct i2c_slave_op *find_slave(struct i2c_slave_ctx *ctx,
					      unsigned int addr)
{
	int i;
	struct i2c_slave_op *slave;
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

void isr_addr_cb(struct i2c_slave_ctx *ctx)
{
	unsigned int isr = ctx->reg->isr;
	unsigned int addr = (isr & ISR_ADDCODE_MASK) >> ISR_ADDCODE_SHIFT;
	unsigned int dir = (isr & ISR_DIR_MASK) >> ISR_DIR_SHIFT;

	ctx->dir = dir;
	ctx->slave = find_slave(ctx, addr);
	if (ctx->slave) {
		if (ctx->slave->match) {
			ctx->slave->match(ctx->slave->priv, addr, dir);
		}
		if (dir == I2C_SLAVE_READ && ctx->slave->read) {
			ctx->reg->cr1 |= CR1_TXIE; /*Set transmit IT*/
			ctx->reg->txdr = ctx->slave->read(ctx->slave->priv);
		}
	}
}

void isr_addr_clr(struct i2c_slave_ctx *ctx, struct i2c_isr_op *isr_op)
{
	ctx->reg->icr |= 1 << isr_op->bit;
}

struct i2c_isr_op i2c_isr_table[] = {
	{isr_rxne_cb, isr_rxne_clr, 2},
	{isr_txis_cb, isr_txis_clr, 1},
	{isr_stopf_cb, isr_stopf_clr, 5},
	{isr_tcr_cb, isr_tcr_clr, 7},
	{isr_tc_cb, isr_tc_clr, 6},
	{isr_addr_cb, isr_addr_clr, 3},
	{isr_nackf_cb, isr_nackf_clr, 4},
	{isr_berr_cb, isr_berr_clr, 8},
	{isr_arlo_cb, isr_arlo_clr, 9},
	{isr_ovr_cb, isr_ovr_clr, 10},
	{isr_pecerr_cb, isr_pecerr_clr, 11},
	{isr_timeout_cb, isr_timeout_clr, 12},
	{isr_alert_cb, isr_alert_clr, 13},
};

#define ISR_TXE_SHIFT		0
#define ISR_TXIS_SHIFT		1
#define ISR_RXNE_SHIFT		2
#define ISR_ADDR_SHIFT		3
#define ISR_NACK_SHIFT		4
#define ISR_STOP_SHIFT		5

void i2c_slave_isr(struct i2c_slave_ctx *ctx)
{
	unsigned int old;
	unsigned int sts;
	int i, j, isr;
	int p[6] = {
		ISR_TXE_SHIFT, ISR_TXIS_SHIFT, ISR_RXNE_SHIFT,
		ISR_NACK_SHIFT, ISR_STOP_SHIFT, ISR_ADDR_SHIFT,
	};

	old = i2c_intr_disable(ctx);
	sts = ctx->reg->isr & ctx->isr_irq_mask;

	for (i = 0; i < 6; ++i) {
		isr = p[i];
		if ((sts >> isr) & 1) {
			for (j = 0; j < ARRAY_SIZE(i2c_isr_table); ++j) {
				if (i2c_isr_table[j].bit == isr) {
					if (i2c_isr_table[j].cb)
						i2c_isr_table[j].cb(ctx);
					if (i2c_isr_table[j].clr)
						i2c_isr_table[j].clr(ctx,
							i2c_isr_table + j);
				}
			}
		}
	}

	i2c_intr_restore(ctx, old);
}

static void timing_init(void *reg)
{
	struct i2c_reg *i2c = reg;
	uint32_t sclh, scll, scldelay, sdadelay;

	sclh = 31;
	scll = 37;
	scldelay = 2;
	sdadelay = 0;

	i2c->timingr = (scldelay << 20) | (sdadelay << 16) |
		(sclh << 8) | (scll << 0);
}

static void __attribute__((unused)) peripheral_init(void *reg)
{
	uint32_t i2c = (uint32_t)reg;

	/* let i2c2 works at fixed clock rate 8MHz */
	rcu_i2c2_clock_config(RCU_I2C2SRCSRC_CKIRC8M);

	/* BAUDRATE */
	i2c_deinit(i2c);
	i2c_analog_noise_filter_enable(i2c);
	i2c_digital_noise_filter_config(i2c, FILTER_DISABLE);
	i2c_stretch_scl_low_enable(i2c);

	timing_init(reg);
}

int i2c_slave_init(struct i2c_slave_ctx *ctx, void *reg,
		   int oa1, int oa2, int oa2mask)
{
	int i;

	/* prepare hardware first */
	peripheral_init(reg);

	ctx->reg = reg;
	ctx->isr_irq_mask = 0;
	ctx->dir = I2C_SLAVE_WRITE;
	/* oa1 enable, 7 bit mode, 7 bit address */
	ctx->reg->oar1 = 0;
	if (oa1 > 0)
		ctx->reg->oar1 = (1 << 15) | ((oa1 & 0x7f) << 1);

	/* oa2 enable, oa2 address mask, 7 bit address */
	ctx->reg->oar2 = 0;
	if (oa2 > 0)
		ctx->reg->oar2 = (1 << 15) | ((oa2 & 0x7f) << 1)
			| ((oa2mask & 7) << 8);

	for (i = 0; i < ARRAY_SIZE(i2c_isr_table); ++i)
		ctx->isr_irq_mask |= (1 << i2c_isr_table[i].bit);
	return 0;
}

int i2c_slave_register(struct i2c_slave_ctx *ctx, struct i2c_slave_op *slave)
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

int i2c_slave_start(struct i2c_slave_ctx *ctx)
{
	ctx->slave = NULL;
	ctx->reg->cr1 |= CR1_PE;
	/* enable error, address match, rx and tx isr, stop */
	ctx->reg->cr1 |= CR1_ADDRIE | CR1_RXIE | CR1_TXIE |
						CR1_STOPIE | CR1_NACKIE;
	return 0;
}

int i2c_slave_reset(struct i2c_slave_ctx *ctx)
{
	struct i2c_slave_op *op;
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

