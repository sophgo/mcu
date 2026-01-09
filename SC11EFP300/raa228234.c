#include <i2c_master.h>
#include <raa228234.h>
#include <pca9848.h>
#include <debug.h>
#include <tick.h>
#include <gd32f4xx_i2c.h>
#include <system.h>
#include <timer.h>
#include <common.h>

#define RAA228234_SLAVE_ADDR	0x60
#define I2C			I2C1

#define REG_VOUT_COMMAND	0x21
#define REG_VOUT_DROOP		0X28
#define REG_PEAK_OC_LIMIT	0XCD
#define REG_PHCURLIMIT		0x3C
#define REG_FAST_OC_LIMIT	0x46
#define REG_SLOW_OC_LIMIT	0xEA

#define REG_PAGE	0x0

static uint8_t raa228234_channel[2] = {7, 6};

static inline void raa228234_select(int idx)
{
	pca9848_set(PCA9848_1, 1 << raa228234_channel[idx]);
}

static inline int raa228234_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;
	raa228234_select(idx);
	if (i2c_master_smbus_read_byte(I2C, RAA228234_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("ra228234-%d read byte %d failed\n", idx, cmd);
	return tmp;
}

static inline int raa228234_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	raa228234_select(idx);
	if (i2c_master_smbus_write_byte(I2C, RAA228234_SLAVE_ADDR,
					  1, cmd, data))
		dbg_printf("ra228234-%d write byte %d failed\n", idx, cmd);
	return 0;
}

static inline int raa228234_read_word(int idx, unsigned char cmd)
{
	uint16_t tmp;
	raa228234_select(idx);
	if (i2c_master_smbus_read_word(I2C, RAA228234_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("ra228234-%d read word %d failed\n", idx, cmd);

	return tmp;
}

static inline int raa228234_write_word(int idx, unsigned char cmd, uint16_t data)
{
	raa228234_select(idx);
	if (i2c_master_smbus_write_word(I2C, RAA228234_SLAVE_ADDR,
					   1, cmd, data))
		dbg_printf("ra228234-%d write word %d failed\n", idx, cmd);
	return 0;
}

unsigned long raa228234_output_voltage(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit mV*/
	raa228234_write_byte(idx, 0, page);
	tmp = (unsigned long)raa228234_read_word(idx, 0x8b);

	return tmp == 0xffff ? 0 : tmp;
}

unsigned long raa228234_output_current(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit 0.1A */
	raa228234_write_byte(idx, 0, page);
	tmp = (unsigned long)raa228234_read_word(idx, 0x8c);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 100;
}

unsigned long raa228234_output_power(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit W */
	raa228234_write_byte(idx, 0, page);
	tmp = (unsigned long)raa228234_read_word(idx, 0x96);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 1000;
}

int raa228234_set_out_voltage(int idx, int page, int voltage)
{
	raa228234_write_byte(idx, REG_PAGE, page);
	raa228234_write_word(idx, REG_VOUT_COMMAND, (uint16_t)voltage);

	return 0;
}


int raa228234_set_out_droop(int idx, int page, int resistance)
{
	raa228234_write_byte(idx, REG_PAGE, page);
	raa228234_write_word(idx, REG_VOUT_DROOP, (uint16_t)resistance);

	return 0;
}

unsigned long raa228234_out_droop(int idx, int page)
{
	unsigned long tmp;

	raa228234_write_byte(idx, 0, page);
	tmp = (unsigned long)raa228234_read_word(idx, REG_VOUT_DROOP);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

// unit:A
// step:0.1A
int raa228234_set_peck_oc_limit(int idx, int page, int value)
{
	raa228234_write_byte(idx, REG_PAGE, page);
	raa228234_write_word(idx, REG_PEAK_OC_LIMIT, (uint16_t)value);
	//raa228234_write_word(idx, REG_PHCURLIMIT, (uint16_t)(value / 10));

	return 0;
}

int raa228234_read_peck_oc_limit(int idx, int page)
{
	unsigned long tmp;

	raa228234_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)raa228234_read_word(idx, REG_PEAK_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

int raa228234_set_fast_oc_limit(int idx, int page, int value)
{
	raa228234_write_byte(idx, REG_PAGE, page);
	raa228234_write_word(idx, REG_FAST_OC_LIMIT, (uint16_t)value);

	return 0;
}

int raa228234_read_fast_oc_limit(int idx, int page)
{
	unsigned long tmp;

	raa228234_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)raa228234_read_word(idx, REG_FAST_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}


int raa228234_set_slow_oc_limit(int idx, int page, int value)
{
	raa228234_write_byte(idx, REG_PAGE, page);
	raa228234_write_word(idx, REG_SLOW_OC_LIMIT, (uint16_t)value);

	return 0;
}

int raa228234_read_slow_oc_limit(int idx, int page)
{
	unsigned long tmp;

	raa228234_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)raa228234_read_word(idx, REG_SLOW_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

void raa228234_init(void)
{
	int val;

	for (int i = 0; i < RAA228234_CHANNEL; i++) {
		val = raa228234_read_peck_oc_limit(i, 0xff);
		timer_udelay(1000);
		dbg_printf("before: raa228234-%d peak oc limit is %d\n", i, val);
		timer_udelay(1000);
		if (val != 1200) {
			raa228234_set_peck_oc_limit(i, 0xff, 1200);
			timer_udelay(1000);
			val = raa228234_read_peck_oc_limit(i, 0xff);
			timer_udelay(1000);
			dbg_printf("after: raa228234-%d peak oc limit is %d\n", i, val);
		}
		timer_udelay(1000);
		val = raa228234_read_fast_oc_limit(i, 0xff);
		timer_udelay(1000);
		dbg_printf("before: raa228234-%d fast oc limit is %d\n", i, val);
		if (val != 4800) {
			timer_udelay(1000);
			raa228234_set_fast_oc_limit(i, 0xff, 4800);
			timer_udelay(1000);
			val = raa228234_read_fast_oc_limit(i, 0xff);
			timer_udelay(1000);
			dbg_printf("after: raa228234-%d fast oc limit is %d\n", i, val);
		}
		timer_udelay(1000);
		val = raa228234_read_slow_oc_limit(i, 0xff);
		timer_udelay(1000);
		dbg_printf("before: raa228234-%d slow oc limit is %d\n", i, val);
		if (val != 3600) {
			timer_udelay(1000);
			raa228234_set_slow_oc_limit(i, 0xff, 3600);
			timer_udelay(1000);
			val = raa228234_read_slow_oc_limit(i, 0xff);
			timer_udelay(1000);
			dbg_printf("after: raa228234-%d slow oc limit is %d\n", i, val);
		}
		timer_udelay(1000);

		val = raa228234_out_droop(i, 0xff);
		timer_udelay(1000);
		dbg_printf("before: chip %d & chip %d droop is %d\n", i*2 + 0, i*2 + 1, val);
		if (val != 50) {
			timer_udelay(1000);
			raa228234_set_out_droop(i, 0xff, 50);
			timer_udelay(1000);
			val = raa228234_out_droop(i, 0xff);
			timer_udelay(1000);
			dbg_printf("after: chip %d & chip %d droop is %d\n", i*2 + 0, i*2 + 1, val);
		}
		timer_udelay(1000);

	}
}

