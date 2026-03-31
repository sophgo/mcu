#include <i2c_master.h>
#include <multiphase.h>
#include <pca9848.h>
#include <debug.h>
#include <tick.h>
#include <gd32f4xx_i2c.h>
#include <system.h>
#include <timer.h>
#include <common.h>
#include <math.h>

#define multiphase_SLAVE_ADDR	0x20
#define I2C			I2C1
#define multiphase_CHANNEL	1

#define REG_RAIL1_PEAK_OC_LIMIT		0X02 // page 2
#define REG_RAIL2_PEAK_OC_LIMIT		0X12 // page 2
#define REG_VOUT_COMMAND			0x21 // rail1: page0 0x21, rail2: page1 0x21
#define REG_VOUT_MAX				0x24 // rail1: page0 0x24, rail2: page1 0x24
#define REG_VOUT_DROOP				0x28 // rail1: page0 0x28, rail2: page1 0x28
#define REG_RAIL1_IDROOP_MODE		0x09 // page 2
#define REG_RAIL2_IDROOP_MODE		0x19 // page 2

#define REG_VOUT_READ				0x8b // rail1: page0 0x8b, rail2: page1 0x8b
#define REG_IOUT_COMMAND			0x8c // page 0x0 for rail1, page 0x1 for rail2
#define REG_VOUT_POWER				0x96 // page0 0x96, page1 0x96


#define REG_PAGE	0x0

static uint8_t multiphase_channel[1] = {7};

static inline void multiphase_select(int idx)
{
	pca9848_set(PCA9848_1, 1 << multiphase_channel[idx]);
}

static inline int multiphase_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;
	multiphase_select(idx);
	if (i2c_master_smbus_read_byte(I2C, multiphase_SLAVE_ADDR,
					  1, cmd, &tmp))
		debug("multiphase-%d read byte %d failed\n", idx, cmd);
	return tmp;
}

static inline int multiphase_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	multiphase_select(idx);
	if (i2c_master_smbus_write_byte(I2C, multiphase_SLAVE_ADDR,
					  1, cmd, data))
		debug("multiphase-%d write byte %d failed\n", idx, cmd);
	return 0;
}

static inline int multiphase_read_word(int idx, unsigned char cmd)
{
	uint16_t tmp;
	multiphase_select(idx);
	if (i2c_master_smbus_read_word(I2C, multiphase_SLAVE_ADDR,
					  1, cmd, &tmp))
		debug("multiphase-%d read word %d failed\n", idx, cmd);

	return tmp;
}

static inline int multiphase_write_word(int idx, unsigned char cmd, uint16_t data)
{
	multiphase_select(idx);
	if (i2c_master_smbus_write_word(I2C, multiphase_SLAVE_ADDR,
					   1, cmd, data))
		debug("multiphase-%d write word %d failed\n", idx, cmd);
	return 0;
}

unsigned long multiphase_output_voltage(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit mV*/
	multiphase_write_byte(idx, 0, page);
	tmp = (unsigned long)multiphase_read_word(idx, REG_VOUT_READ);

	return tmp == 0xffff ? 0 : tmp;
}

unsigned long multiphase_output_current(int idx, int page)
{
	unsigned long tmp;

	/* set page, unit 0.1A */
	if (page == 0xff) {
		multiphase_write_byte(idx, REG_PAGE, 0);
		tmp = (unsigned long)multiphase_read_word(idx, REG_IOUT_COMMAND);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else if (page == 0x0 || page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, page);
		tmp = (unsigned long)multiphase_read_word(idx, REG_IOUT_COMMAND);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else {
		debug("[%s] invalid page %d\n", __func__, page);
		return 0;
	}

	return tmp * 100;
}

unsigned long multiphase_output_power(int idx, int page)
{
	unsigned long tmp;

	if (page == 0x0) {
		multiphase_write_byte(idx, REG_PAGE, 0);
		tmp = (unsigned long)multiphase_read_word(idx, REG_VOUT_POWER);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else if (page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, 1);
		tmp = (unsigned long)multiphase_read_word(idx, REG_VOUT_POWER);
		tmp = tmp == 0xffff ? 0 : tmp;
	}
	else {
		debug("[%s] invalid page %d\n", __func__, page);
		return 0;
	}
	/* set page, unit W */

	return tmp * 1000;
}

int multiphase_set_out_voltage(int idx, int page, int voltage)
{
	int vid_code = 49 + (voltage - 490) / 5; // volt offset 490mV, step 5mV
	if (page == 0xff) {
		multiphase_write_byte(idx, REG_PAGE, 0);
		multiphase_write_word(idx, REG_VOUT_COMMAND, (uint16_t)vid_code);

		multiphase_write_byte(idx, REG_PAGE, 1);
		multiphase_write_word(idx, REG_VOUT_COMMAND, (uint16_t)vid_code);
	} else if (page == 0x0 || page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, page);
		multiphase_write_word(idx, REG_VOUT_COMMAND, (uint16_t)vid_code);
	} else {
		debug("[%s] invalid page %d\n", __func__, page);
	}

	return 0;
}


int multiphase_set_out_droop(int idx, int page, int resistance)
{
	int tmp = (int)((resistance * 256) / (5*1000)) + 1; // unit mOhm, step 5mOhm, max 1.275 Ohm
	if (page == 0xff) {
		multiphase_write_byte(idx, REG_PAGE, 0);
		multiphase_write_word(idx, REG_VOUT_DROOP, (uint16_t)tmp);

		multiphase_write_byte(idx, REG_PAGE, 1);
		multiphase_write_word(idx, REG_VOUT_DROOP, (uint16_t)tmp);
	} else if (page == 0x0 || page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, page);
		multiphase_write_word(idx, REG_VOUT_DROOP, (uint16_t)tmp);
	} else {
		debug("[%s] invalid page %d\n", __func__, page);
	}

	return 0;
}

unsigned long multiphase_output_droop(int idx, int page)
{
	unsigned long tmp;

	multiphase_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)multiphase_read_word(idx, REG_VOUT_DROOP);
	tmp = tmp == 0xff ? 0 : tmp;

	tmp = (tmp*5*1000) / 256;

	return tmp;
}

// unit:A
// step:0.1A
int multiphase_set_peck_oc_limit(int idx, int page, int value)
{
	if (page ==0xff) {
		multiphase_write_byte(idx, REG_PAGE, 2);
		multiphase_write_word(idx, REG_RAIL1_PEAK_OC_LIMIT, (uint16_t)value);
		multiphase_write_word(idx, REG_RAIL2_PEAK_OC_LIMIT, (uint16_t)value);
	} else if (page == 0x0) {
		multiphase_write_byte(idx, REG_PAGE, page);
		multiphase_write_word(idx, REG_RAIL1_PEAK_OC_LIMIT, (uint16_t)value);
	} else if (page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, page);
		multiphase_write_word(idx, REG_RAIL2_PEAK_OC_LIMIT, (uint16_t)value);
	} else {
		debug("[%s] invalid page %d\n", __func__, page);
	}

	return 0;
}

int multiphase_read_peck_oc_limit(int idx, int page)
{
	unsigned long tmp;
	
	if (page ==0xff) {
		multiphase_write_byte(idx, REG_PAGE, 2);
		tmp = (unsigned long)multiphase_read_word(idx, REG_RAIL1_PEAK_OC_LIMIT);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else if (page == 0x0) {
		multiphase_write_byte(idx, REG_PAGE, page);
		tmp = (unsigned long)multiphase_read_word(idx, REG_RAIL1_PEAK_OC_LIMIT);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else if (page == 0x1) {
		multiphase_write_byte(idx, REG_PAGE, page);
		tmp = (unsigned long)multiphase_read_word(idx, REG_RAIL2_PEAK_OC_LIMIT);
		tmp = tmp == 0xffff ? 0 : tmp;
	} else {
		debug("[%s] invalid page %d\n", __func__, page);
		return 0;
	}

	return tmp;
}

int multiphase_read_reg(int idx, int page, int reg)
{
	unsigned long tmp;

	multiphase_write_byte(idx, REG_PAGE, page);
	tmp = (unsigned long)multiphase_read_word(idx, reg);
	tmp = tmp == 0xffff ? 0 : tmp;

	return tmp;
}


#if 0
void multiphase_init(void)
{
	int val;

	for (int i = 0; i < SOC_NUM; i++) {
		val = multiphase_read_peck_oc_limit(i, 0xff);
		timer_udelay(1000);
		debug("before: multiphase-%d peak oc limit is %d\n", i, val);
		timer_udelay(1000);
		if (val != 115) {
			multiphase_set_peck_oc_limit(i, 0xff, 115);
			timer_udelay(1000);
			val = multiphase_read_peck_oc_limit(i, 0xff);
			timer_udelay(1000);
			debug("after: multiphase-%d peak oc limit is %d\n", i, val);
		}
		timer_udelay(1000);

		val = multiphase_out_droop(i, 0xff);
		timer_udelay(1000);
		debug("before: chip %d & chip %d droop is %d\n", i*2 + 0, i*2 + 1, val);
		if (val != 50) {
			timer_udelay(1000);
			multiphase_set_out_droop(i, 0xff, 50);
			timer_udelay(1000);
			val = multiphase_out_droop(i, 0xff);
			timer_udelay(1000);
			debug("after: chip %d & chip %d droop is %d\n", i*2 + 0, i*2 + 1, val);
		}
		timer_udelay(1000);

	}
}
#endif

