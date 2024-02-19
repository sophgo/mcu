#include <i2c_master.h>
#include <isl68224.h>
#include <pca9848.h>
#include <debug.h>
#include <tick.h>
#include <gd32e50x_i2c.h>
#include <system.h>

#define ISL68224_SLAVE_ADDR	0x60
#define I2C			I2C1

#define REG_VOUT_COMMAND	0x21
#define REG_VOUT_DROOP		0X28
#define REG_PAGE	0x0

static uint8_t isl68224_channel[6] = {1, 3, 6, 7, 4, 0};

static inline void isl68224_select(int idx)
{
	if (idx == 5){
		pca9848_set(TCA9548A1, 1 << isl68224_channel[idx]);
	}else{
		pca9848_set(PCA9848, 1 << isl68224_channel[idx]);
	}
}

static inline int isl68224_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;
	isl68224_select(idx);
	while (i2c_master_smbus_read_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("68224-%d read %d failed\n", idx, cmd);
	return tmp;
}

static inline int isl68224_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	isl68224_select(idx);
	while (i2c_master_smbus_write_byte(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, data))
		dbg_printf("68224-%d write %d failed\n", idx, cmd);
	return 0;
}

static inline int isl68224_read_word(int idx, unsigned char cmd)
{
	uint16_t tmp;
	isl68224_select(idx);
	while (i2c_master_smbus_read_word(I2C, ISL68224_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("68224-%d write %d failed\n", idx, cmd);

	return tmp;
}

static inline int isl68224_write_word(int idx, unsigned char cmd, uint16_t data)
{
	isl68224_select(idx);
	while (i2c_master_smbus_write_word(I2C, ISL68224_SLAVE_ADDR,
					   1, cmd, data));
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

	return tmp  ;
}
