/*
 * i2c.c
 *
 *  Created on: Apr 23, 2019
 *      Author: weic
 */

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include "i2c_bm.h"

#ifndef __IO
#define __IO volatile
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

struct i2c_reg {
	 __IO int32_t cr1;
	 __IO int32_t cr2;
	 __IO int32_t oar1;
	 __IO int32_t oar2;
	 __IO int32_t timingr;
	 __IO int32_t timeoutr;
	 __IO int32_t isr;
	 __IO int32_t icr;
	 __IO int32_t pecr;
	 __IO int32_t rxdr;
	 __IO int32_t txdr;
};

struct i2c_isr_op;
typedef void (*isr_cb)(void);
typedef void (*isr_clr)(struct i2c_isr_op *isr_op);

struct i2c_ctx {
	int dir;
	int idx;
	unsigned int isr_irq_mask;
	struct i2c_reg *reg;
	struct i2c_slave_op *slave_list[I2C_SLAVE_MAX];
	struct i2c_slave_op *slave; /* current slave */
} i2c_ctx;

struct i2c_isr_op {
	isr_cb	cb;
	isr_clr	clr;
	int		bit;
};

#define CR1_SBC			(1 << 16)
#define CR1_RXDMAEN		(1 << 15)
#define CR1_TXDMAEN		(1 << 14)

#define CR1_NOSTRETCH   (1 << 17)
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
#define ISR_BUSY			(0X1 << ISR_BUSY_SHIFT)
#define ISR_TXE				(1)

static unsigned int i2c_intr_disable(void)
{
	unsigned int old = i2c_ctx.reg->cr1;
	i2c_ctx.reg->cr1 &= ~CR1_IES_MASK;
	return old;
}

static void i2c_intr_restore(unsigned int old)
{
	i2c_ctx.reg->cr1 |= old & CR1_IES_MASK;
}


#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define ISR_OP_DEFAULT(name)									\
void isr_##name##_cb(void) {}							\
void isr_##name##_clr(struct i2c_isr_op *isr_op) 		\
{																\
	i2c_ctx.reg->icr |= 1 << isr_op->bit;						\
}

//ISR_OP_DEFAULT(rxne)
//ISR_OP_DEFAULT(txis)
//ISR_OP_DEFAULT(stopf)
ISR_OP_DEFAULT(tcr)
ISR_OP_DEFAULT(tc)
//ISR_OP_DEFAULT(addr)
ISR_OP_DEFAULT(nackf)
ISR_OP_DEFAULT(berr)
ISR_OP_DEFAULT(arlo)
ISR_OP_DEFAULT(ovr)
ISR_OP_DEFAULT(pecerr)
ISR_OP_DEFAULT(timeout)
ISR_OP_DEFAULT(alert)

void isr_rxne_clr(struct i2c_isr_op *isr_op)
{
	/* do nothing, read rxdr will clear this bit */
}

void isr_rxne_cb(void)
{
	/* this will clear rxne bit */
	uint8_t data = i2c_ctx.reg->rxdr;

	if (i2c_ctx.slave)
		i2c_ctx.slave->write(data);
}

void isr_txis_cb(void)
{
	/* TODO: send another bit */
	if (i2c_ctx.slave)
		i2c_ctx.reg->txdr = i2c_ctx.slave->read();
	else
		i2c_ctx.reg->txdr = 0;
}

void isr_txis_clr(struct i2c_isr_op *isr_op)
{
	/* this bit ill clear when txdr write to some data */
}

void isr_stopf_cb(void)
{
	i2c_ctx.reg->isr |= 1;
	/* TODO: stop flag received */
	if (i2c_ctx.slave)
		i2c_ctx.slave->stop();
}

void isr_stopf_clr(struct i2c_isr_op *isr_op)
{
	i2c_ctx.reg->icr |= 1 << isr_op->bit;
}

static inline struct i2c_slave_op *find_slave(unsigned int addr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(i2c_ctx.slave_list); ++i) {
		if (i2c_ctx.slave_list[i]->addr == addr)
			return *(i2c_ctx.slave_list + i);
	}
	return NULL;
}

void isr_addr_cb(void)
{
	unsigned int isr = i2c_ctx.reg->isr;
	unsigned int addr = (isr & ISR_ADDCODE_MASK) >> ISR_ADDCODE_SHIFT;
	unsigned int dir = (isr & ISR_DIR_MASK) >> ISR_DIR_SHIFT;

	i2c_ctx.dir = dir;
	i2c_ctx.slave = find_slave(addr);
	if (i2c_ctx.slave) {
		i2c_ctx.slave->match(dir);
		if (dir == I2C_SLAVE_READ)
			i2c_ctx.reg->txdr = i2c_ctx.slave->read();
	}
}

void isr_addr_clr(struct i2c_isr_op *isr_op)
{
	i2c_ctx.reg->icr |= 1 << isr_op->bit;
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

void i2c_isr(void)
{
	unsigned int old;
	unsigned int sts;
	int i, j, isr;
	int p[6] = {
		ISR_TXE_SHIFT, ISR_TXIS_SHIFT, ISR_RXNE_SHIFT,
		ISR_NACK_SHIFT, ISR_STOP_SHIFT, ISR_ADDR_SHIFT,
	};

	old = i2c_intr_disable();
	sts = i2c_ctx.reg->isr & i2c_ctx.isr_irq_mask;

	for (i = 0; i < 6; ++i) {
		isr = p[i];
		if ((sts >> isr) & 1) {
			for (j = 0; j < ARRAY_SIZE(i2c_isr_table); ++j) {
				if (i2c_isr_table[j].bit == isr) {
					if (i2c_isr_table[j].cb)
						i2c_isr_table[j].cb(i2c_ctx);
					if (i2c_isr_table[j].clr)
						i2c_isr_table[j].clr(i2c_isr_table + j,i2c_ctx);
				}
			}
		}
	}

	i2c_intr_restore(old);
}

int i2c_init(void *reg)
{
	int i;

	i2c_ctx.reg = reg;
	i2c_ctx.isr_irq_mask = 0;
	i2c_ctx.dir = I2C_SLAVE_WRITE;

	for (i = 0; i < ARRAY_SIZE(i2c_isr_table); ++i)
		i2c_ctx.isr_irq_mask |= (1 << i2c_isr_table[i].bit);
	return 0;
}

int i2c_slave_register(struct i2c_slave_op *slave)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(i2c_ctx.slave_list); ++i) {
		if (i2c_ctx.slave_list[i] == NULL) {
			i2c_ctx.slave_list[i] = slave;
			return 0;
		}
	}
	return -1;
}

int i2c_slave_start(void)
{
	/* enable error, address match, rx and tx isr, stop */
	/* nack just to assert */
	/* over run and ander run just to assert */
	i2c_ctx.reg->cr1 |= CR1_ERRIE | CR1_ADDRIE | CR1_RXIE | CR1_TXIE |
						CR1_STOPIE | CR1_NACKIE;
	i2c_ctx.slave = NULL;
	return 0;
}

