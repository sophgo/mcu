#include <i2c_master.h>
#include <mp5475.h>
#include <debug.h>
#include <tick.h>
#include <pca9848.h>
#include <stdio.h>
#include <gd32e50x_i2c.h>
#include <timer.h>
#include <common.h>
#include <system.h>

#define MP5475_SLAVE_ADDR	0x60
#define I2C			I2C0

static uint8_t mp5475_channel[3] = {2, 4, 0};

static inline void mp5475_select(int idx)
{
	pca9848_set(PCA9848_1, 1 << mp5475_channel[idx]);
}

static inline int mp5475_read_byte(int idx, unsigned char cmd)
{
	unsigned char tmp;

	mp5475_select(idx);

	if (i2c_master_smbus_read_byte(I2C, MP5475_SLAVE_ADDR,
					  1, cmd, &tmp))
		dbg_printf("mp5475-%d read 0x%x failed\n", idx, cmd);

	return tmp;
}

static inline int mp5475_write_byte(int idx, unsigned char cmd,
				    unsigned char data)
{
	mp5475_select(idx);

	if (i2c_master_smbus_write_byte(I2C, MP5475_SLAVE_ADDR,
					  1, cmd, data))
		dbg_printf("mp5474-%d write 0x%x faile\n", idx, cmd);
	return 0;
}

int mp5475_buck_on(int idx, unsigned int buck)
{
	unsigned char tmp;
	tmp = mp5475_read_byte(idx, 0x40);
	tmp |= (1 << 5) >> buck;
	mp5475_write_byte(idx, 0x40, tmp);
	return 0;
}

void mp5475_buck_off(int idx, unsigned int buck)
{
	unsigned char tmp;
	tmp = mp5475_read_byte(idx, 0x40);
	tmp &= ~((1 << 5) >> buck);
	mp5475_write_byte(idx, 0x40, tmp);
}

int mp5475_voltage_config(int idx, unsigned int buck, unsigned int voltage)
{
	unsigned int reg_h = 0x13 + 8 * buck;
	unsigned int reg_l = 0x14 + 8 * buck;
	unsigned int value = voltage / 2;
	unsigned int val_h = (value >> 8) & 0x03;
	unsigned int val_l = value & 0xff;

	mp5475_write_byte(idx, reg_h, val_h);
	mp5475_write_byte(idx, reg_l, val_l);
	return 0;
}

unsigned long mp5475_output_current(int idx, unsigned int buck)
{
	unsigned int reg = 0x59 + 2 * buck;

	return mp5475_read_byte(idx, reg) * 50;
}

unsigned long mp5475_output_voltage(int idx, unsigned int buck)
{
	unsigned int reg = 0x5a + 2 * buck;

	return mp5475_read_byte(idx, reg) * 25;
}

int _mp5475_init(int idx)
{
	/* enable system enable, disable buck 1, 2, 3, 4 */
	mp5475_write_byte(idx, 0x40, 1 << 7);
	mp5475_voltage_config(idx, 0, 1800);
	mp5475_voltage_config(idx, 1, 600);
	mp5475_voltage_config(idx, 2, 1100);
	mp5475_voltage_config(idx, 3, 850);
	return 0;
}

int mp5475_init(void)
{
	int i;

	for (i = 0; i < MP5475MAX; ++i)
		_mp5475_init(i);

	return 0;
}

void mp5475_destroy(void)
{
}


