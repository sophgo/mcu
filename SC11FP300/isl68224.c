#include <i2c_master.h>
#include <isl68224.h>
#include <pca9848.h>
#include <debug.h>
#include <tick.h>
#include <gd32e50x_i2c.h>
#include <system.h>
#include <timer.h>
#include <common.h>

#define ISL68224_SLAVE_ADDR	0x60
#define I2C			I2C1

#define REG_VOUT_COMMAND	0x21
#define REG_VOUT_DROOP		0X28
#define REG_PEAK_OC_LIMIT	0XCD
#define REG_PHCURLIMIT		0x3C
#define REG_FAST_OC_LIMIT	0x46
#define REG_SLOW_OC_LIMIT	0xEA

#define REG_PAGE	0x0

static uint8_t isl68224_channel[2] = {7, 6};

static inline void isl68224_select(int idx)
{
	pca9848_set(PCA9848_1, 1 << isl68224_channel[idx]);
}

static inline int isl68224_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;
	isl68224_select(idx);
	if (i2c_master_smbus_read_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("68224-%d read %d failed\n", idx, cmd);
	return tmp;
}

static inline int isl68224_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	isl68224_select(idx);
	if (i2c_master_smbus_write_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, data))
		dbg_printf("68224-%d write %d failed\n", idx, cmd);
	return 0;
}

static inline int isl68224_read_word(int idx, unsigned char cmd)
{
	uint16_t tmp;
	isl68224_select(idx);
	if (i2c_master_smbus_read_word(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("68224-%d write %d failed\n", idx, cmd);

	return tmp;
}

static inline int isl68224_write_word(int idx, unsigned char cmd, uint16_t data)
{
	isl68224_select(idx);
	if (i2c_master_smbus_write_word(I2C, ISL68224_SLAVE_ADDR,
					   1, cmd, data))
		dbg_printf("68224-%d write %d failed\n", idx, cmd);
	return 0;
}

unsigned long isl68224_output_voltage(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit mV*/
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x8b);

	return tmp == 0xffff ? 0 : tmp;
}

unsigned long isl68224_output_current(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit 0.1A */
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x8c);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 100;
}

unsigned long isl68224_output_power(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit W */
	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, 0x96);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp * 1000;
}

int isl68224_set_out_voltage(int idx, int page, int voltage)
{
	isl68224_write_byte(idx, REG_PAGE, page);
	isl68224_write_word(idx, REG_VOUT_COMMAND, (uint16_t)voltage);

	return 0;
}


int isl68224_set_out_droop(int idx, int page, int resistance)
{
	isl68224_write_byte(idx, REG_PAGE, page);
	isl68224_write_word(idx, REG_VOUT_DROOP, (uint16_t)resistance);

	return 0;
}

unsigned long isl68224_out_droop(int idx, int page)
{
	unsigned long tmp;

	isl68224_write_byte(idx, 0, page);
	tmp = (unsigned long)isl68224_read_word(idx, REG_VOUT_DROOP);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

// unit:A
// step:0.1A
int isl68224_set_peck_oc_limit(int idx, int page, int value)
{
	isl68224_write_byte(idx, REG_PAGE, page);
	isl68224_write_word(idx, REG_PEAK_OC_LIMIT, (uint16_t)value);
	//isl68224_write_word(idx, REG_PHCURLIMIT, (uint16_t)(value / 10));

	return 0;
}

int isl68224_read_peck_oc_limit(int idx, int page)
{
	unsigned long tmp;

	isl68224_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)isl68224_read_word(idx, REG_PEAK_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

int isl68224_set_fast_oc_limit(int idx, int page, int value)
{
	isl68224_write_byte(idx, REG_PAGE, page);
	isl68224_write_word(idx, REG_FAST_OC_LIMIT, (uint16_t)value);

	return 0;
}

int isl68224_read_fast_oc_limit(int idx, int page)
{
	unsigned long tmp;

	isl68224_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)isl68224_read_word(idx, REG_FAST_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}


int isl68224_set_slow_oc_limit(int idx, int page, int value)
{
	isl68224_write_byte(idx, REG_PAGE, page);
	isl68224_write_word(idx, REG_SLOW_OC_LIMIT, (uint16_t)value);

	return 0;
}

int isl68224_read_slow_oc_limit(int idx, int page)
{
	unsigned long tmp;

	isl68224_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)isl68224_read_word(idx, REG_SLOW_OC_LIMIT);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}

int isl68224_init(void)
{
	int val;

	for (int i = 0; i < SOC_NUM; i++) {
		val = isl68224_read_peck_oc_limit(i, 0);
		timer_udelay(1000);
		debug("before: chip %d peak oc limit is %d\n", i, val);
		timer_udelay(1000);
		if (val != 1200) {
			isl68224_set_peck_oc_limit(i, 0, 1200);
			timer_udelay(1000);
			val = isl68224_read_peck_oc_limit(i, 0);
			timer_udelay(1000);
			debug("after: chip %d peak oc limit is %d\n", i, val);
		}
		timer_udelay(1000);
		val = isl68224_read_fast_oc_limit(i, 0);
		timer_udelay(1000);
		debug("before: chip %d fast oc limit is %d\n", i, val);
		if (val != 4800) {
			timer_udelay(1000);
			isl68224_set_fast_oc_limit(i, 0, 4800);
			timer_udelay(1000);
			val = isl68224_read_fast_oc_limit(i, 0);
			timer_udelay(1000);
			debug("after: chip %d fast oc limit is %d\n", i, val);
		}
		timer_udelay(1000);
		val = isl68224_read_slow_oc_limit(i, 0);
		timer_udelay(1000);
		debug("before: chip %d slow oc limit is %d\n", i, val);
		if (val != 3600) {
			timer_udelay(1000);
			isl68224_set_slow_oc_limit(i, 0, 3600);
			timer_udelay(1000);
			val = isl68224_read_slow_oc_limit(i, 0);
			timer_udelay(1000);
			debug("after: chip %d slow oc limit is %d\n", i, val);
		}
		timer_udelay(1000);

		val = isl68224_out_droop(i, 0);
		timer_udelay(1000);
		debug("before: chip %d droop is %d\n", i, val);
		if (val != 50) {
			timer_udelay(1000);
			isl68224_set_out_droop(i, 0, 50);
			timer_udelay(1000);
			val = isl68224_out_droop(i, 0);
			timer_udelay(1000);
			debug("after: chip %d droop is %d\n", i, val);
		}
		timer_udelay(1000);

	}
}

