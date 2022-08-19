#include <i2c_master.h>
#include <tick.h>
#include <debug.h>
#include <timer.h>
#include <stddef.h>
#include <common.h>

#define I2C_SPEED	(100 * 1000)

int i2c_master_init(int i2c)
{
	/* configure I2C clock */
	i2c_clock_config(i2c, I2C_SPEED, I2C_DTCY_2);
	/* configure I2C address */
	i2c_mode_addr_config(i2c, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0);
	/* enable i2c controller */
	i2c_enable(i2c);
	/* enable acknowledge */
	i2c_ack_config(i2c, I2C_ACK_ENABLE);

	return 0;
}

int i2c_master_destroy(int i2c)
{
	i2c_disable(i2c);
	return 0;
}

int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			  uint8_t *w, size_t wn,
			  uint8_t *r, size_t rn);

int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn,
		     void *r, unsigned int rn)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, w, wn, r, rn);
}

int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &cmd, 1, data, 1);
}

int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data)
{
	unsigned char tmp[3];

	tmp[0] = cmd;
	tmp[1] = data;
	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 2, NULL, 0);
}

int i2c_master_smbus_write_word(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, uint16_t data)
{
	unsigned char tmp[3];

	tmp[0] = cmd;
	tmp[1] = data & 0xff;
	tmp[2] = (data >> 8) & 0xff;

	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 3, NULL, 0);
}

int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &data, 1, NULL, 0);
}

int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, NULL, 0, data, 1);
}

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     &cmd, 1, (uint8_t *)data, 2);
}

/*
 * i2c transfer implementation, from libopencm3 i2c_common_v1.c, stm32f10x
 * compatible
 */

/* register definition */
#define I2C_CR1		I2C_CTL0
#define I2C_CR2		I2C_CTL1
#define I2C_DR		I2C_DATA

#define I2C_SR1		I2C_STAT0
#define I2C_SR2		I2C_STAT1

/* field definition */
#define I2C_CR1_START	I2C_CTL0_START
#define I2C_CR1_STOP	I2C_CTL0_STOP
#define I2C_CR1_ACK	I2C_CTL0_ACKEN

#define I2C_SR1_SB	I2C_STAT0_SBSEND
#define I2C_SR1_ADDR	I2C_STAT0_ADDSEND
#define I2C_SR1_BTF	I2C_STAT0_BTC
#define I2C_SR1_RxNE	I2C_STAT0_RBNE
#define I2C_SR1_AF	I2C_STAT0_AERR

#define I2C_SR2_BUSY	I2C_STAT1_I2CBSY
#define I2C_SR2_MSL	I2C_STAT1_MASTER

#define I2C_WRITE	0
#define I2C_READ	1

static void i2c_disable_ack(uint32_t i2c)
{
	I2C_CR1(i2c) &= ~I2C_CR1_ACK;
}

static void i2c_enable_ack(uint32_t i2c)
{
	I2C_CR1(i2c) |= I2C_CR1_ACK;
}

static void i2c_send_7bit_address(uint32_t i2c,
				  uint8_t slave, uint8_t readwrite)
{
	I2C_DR(i2c) = (uint8_t)((slave << 1) | readwrite);
}

static uint8_t i2c_get_data(uint32_t i2c)
{
	return I2C_DR(i2c) & 0xff;
}

static void i2c_send_data(uint32_t i2c, uint8_t data)
{
	I2C_DR(i2c) = data;
}

static void i2c_send_stop(uint32_t i2c)
{
	I2C_CR1(i2c) |= I2C_CR1_STOP;
}

static void i2c_send_start(uint32_t i2c)
{
	I2C_CR1(i2c) |= I2C_CR1_START;
}

static int i2c_write7_v1(uint32_t i2c, int addr, uint8_t *data, size_t n)
{
	uint32_t tmp;

	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
		if (timer_is_timeout())
			return -1;
	}

	/* clear AF bit first */
	I2C_SR1(i2c) &= ~I2C_SR1_AF;
	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {
		if (timer_is_timeout())
			return -1;
	}

	i2c_send_7bit_address(i2c, addr, I2C_WRITE);

	/* Waiting for address is transferred. */
	tmp = I2C_SR1(i2c);
	while (!(tmp & I2C_SR1_ADDR)) {
		if (tmp & I2C_SR1_AF) {
			/* NACK is received */
			/* clear AF bit */
			i2c_send_stop(i2c);
			I2C_SR1(i2c) = tmp & ~I2C_SR1_AF;
			return -1;
		}

		if (timer_is_timeout()) {
			i2c_send_stop(i2c);
			return -1;
		}
		tmp = I2C_SR1(i2c);
	}

	/* Clearing ADDR condition sequence. */
	(void)I2C_SR2(i2c);

	for (size_t i = 0; i < n; i++) {
		i2c_send_data(i2c, data[i]);
		while (!(I2C_SR1(i2c) & (I2C_SR1_BTF))) {
			if (timer_is_timeout()) {
				i2c_send_stop(i2c);
				return -1;
			}
		}
	}
	return 0;
}

static int i2c_read7_v1(uint32_t i2c, int addr, uint8_t *res, size_t n)
{
	uint32_t tmp;

	/* clear AF bit first */
	I2C_SR1(i2c) &= ~I2C_SR1_AF;
	i2c_send_start(i2c);
	i2c_enable_ack(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY)))) {
		if (timer_is_timeout())
			return -1;
	}

	i2c_send_7bit_address(i2c, addr, I2C_READ);

	/* Waiting for address is transferred. */
	tmp = I2C_SR1(i2c);
	while (!(tmp & I2C_SR1_ADDR)) {
		if (tmp & I2C_SR1_AF) {
			/* NACK is received */
			/* clear AF bit */
			i2c_send_stop(i2c);
			I2C_SR1(i2c) = tmp & ~I2C_SR1_AF;
			return -1;
		}

		if (timer_is_timeout()) {
			i2c_send_stop(i2c);
			return -1;
		}
		tmp = I2C_SR1(i2c);
	}
	/* Clearing ADDR condition sequence. */
	(void)I2C_SR2(i2c);

	for (size_t i = 0; i < n; ++i) {
		if (i == n - 1) {
			i2c_disable_ack(i2c);
		}
		while (!(I2C_SR1(i2c) & I2C_SR1_RxNE)) {
			if (timer_is_timeout()) {
				i2c_send_stop(i2c);
				return -1;
			}
		}
		res[i] = i2c_get_data(i2c);
	}
	i2c_send_stop(i2c);

	return 0;
}

/**
 * Run a write/read transaction to a given 7bit i2c address
 * If both write & read are provided, the read will use repeated start.
 * Both write and read are optional
 * There are likely still issues with repeated start/stop condtions!
 * @param i2c peripheral of choice, eg I2C1
 * @param addr 7 bit i2c device address
 * @param w buffer of data to write
 * @param wn length of w
 * @param r destination buffer to read into
 * @param rn number of bytes to read (r should be at least this long)
 */
int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			  uint8_t *w, size_t wn,
			  uint8_t *r, size_t rn)
{
	int err = 0;

	timer_start(timeout * 1000);

	if (wn) {
		err = i2c_write7_v1(i2c, addr, w, wn);
		if (err)
			goto out;
	}
	if (rn) {
		err = i2c_read7_v1(i2c, addr, r, rn);
		if (err)
			goto out;
	} else {
		i2c_send_stop(i2c);
	}

out:
	timer_stop();

	if (err) {
		/* wait stop signal done */
		timer_udelay(10);
		i2c_deinit(i2c);
		i2c_master_init(i2c);
	}

	return err;
}

int i2c_master_write_byte(int i2c, unsigned char addr,
			  unsigned long timeout,
			  unsigned char data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     (uint8_t *)&data, 1, NULL, 0);
}

int i2c_master_write_block(int i2c, unsigned char addr,
			   unsigned long timeout,
			   unsigned char *data, unsigned int len)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     (uint8_t *)data, len, NULL, 0);
}

int i2c_master_read_block(int i2c, unsigned char addr,
			  unsigned long timeout,
			  unsigned char *data, unsigned int len)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     NULL, 0, (uint8_t *)data, len);
}

