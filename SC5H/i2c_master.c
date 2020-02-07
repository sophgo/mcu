#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

int i2c_master_init(int i2c)
{
	i2c_reset(i2c);
	i2c_peripheral_disable(i2c);
	//configure ANFOFF DNF[3:0] in CR1
	i2c_enable_analog_filter(i2c);
	i2c_set_digital_filter(i2c, 0);
	i2c_set_speed(i2c, i2c_speed_sm_100k, rcc_apb1_frequency);
	//configure No-Stretch CR1 (only relevant in slave mode)
	i2c_enable_stretching(i2c);
	//addressing mode
	i2c_set_7bit_addr_mode(i2c);
	i2c_peripheral_enable(i2c);
	return 0;
}

int i2c_master_destroy(int i2c)
{
	i2c_reset(i2c);
	i2c_peripheral_disable(i2c);
	return 0;
}

int i2c_master_trans(int i2c, unsigned char addr,
		     void *w, unsigned int wn,
		     void *r, unsigned int rn)
{
	i2c_transfer7(i2c, addr, w, wn, r, rn);
	return 0;
}

int i2c_master_smbus_read_byte(int i2c, unsigned char addr, unsigned char cmd)
{
	uint8_t data;
	i2c_transfer7(i2c, addr, &cmd, 1, &data, 1);
	return data;
}

int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned char cmd, unsigned char data)
{
	unsigned char tmp[2];

	tmp[0] = cmd;
	tmp[1] = data;
	i2c_transfer7(i2c, addr, tmp, 2, NULL, 0);
	return 0;
}

