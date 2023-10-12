/*
 * ds1307.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <stdio.h>
#include <i2c/i2c_master/i2c_master.h>
#include <loop/loop.h>
#include <tick/tick.h>
#include <chip/chip.h>
#include <power/power.h>
#include <common/common.h>
#include <stdlib.h>
#include <debug.h>
#include <i2c-slaves/mcu/mcu.h>
#include <i2c-slaves/ds1307/ds1307.h>
#include <string.h>

struct ds1307_ctx ds1307_instance_reg = {
	{0x00, 0x00, 0x00, 0x00},		/* Second  */
	{0x01, 0x01, 0x00, 0x00},		/* Minute  */
	{0x02, 0x02, 0x01, 0x00},		/* Hour    */
	{0x03, 0x03, 0x06, 0x00},		/* Day     */
	{0x04, 0x04, 0x26, 0x00},		/* Date    */
	{0x05, 0x05, 0x08, 0x00},		/* Month   */
	{0x06, 0x06, 0x23, 0x00},		/* Year    */
	{0x07, 0x07, 0x03, 0x00},		/* Control */
	{
		{0x08, 0x08, 0x00, 0x00},
		{0x09, 0x09, 0x00, 0x00},
		{0x0a, 0x0a, 0x00, 0x00},
		{0x0b, 0x0b, 0x00, 0x00},
		{0x0c, 0x0c, 0x00, 0x00},
		{0x0d, 0x0d, 0x00, 0x00},
		{0x0e, 0x0e, 0x00, 0x00},
		{0x0f, 0x0f, 0x00, 0x00},
		{0x10, 0x10, 0x00, 0x00},
		{0x11, 0x11, 0x00, 0x00},
		{0x12, 0x12, 0x00, 0x00},
		{0x13, 0x13, 0x00, 0x00},
		{0x14, 0x14, 0x00, 0x00},
		{0x15, 0x15, 0x00, 0x00},
		{0x16, 0x16, 0x00, 0x00},
		{0x17, 0x17, 0x00, 0x00},
		{0x18, 0x18, 0x00, 0x00},
		{0x19, 0x19, 0x00, 0x00},
		{0x1a, 0x1a, 0x00, 0x00},
		{0x1b, 0x1b, 0x00, 0x00},
		{0x1c, 0x1c, 0x00, 0x00},
		{0x1d, 0x1d, 0x00, 0x00},
		{0x1e, 0x1e, 0x00, 0x00},
		{0x1f, 0x1f, 0x00, 0x00},
		{0x20, 0x20, 0x00, 0x00},
		{0x21, 0x21, 0x00, 0x00},
		{0x22, 0x22, 0x00, 0x00},
		{0x23, 0x23, 0x00, 0x00},
		{0x24, 0x24, 0x00, 0x00},
		{0x25, 0x25, 0x00, 0x00},
		{0x26, 0x26, 0x00, 0x00},
		{0x27, 0x27, 0x00, 0x00},
		{0x28, 0x28, 0x00, 0x00},
		{0x29, 0x29, 0x00, 0x00},
		{0x2a, 0x2a, 0x00, 0x00},
		{0x2b, 0x2b, 0x00, 0x00},
		{0x2c, 0x2c, 0x00, 0x00},
		{0x2d, 0x2d, 0x00, 0x00},
		{0x2e, 0x2e, 0x00, 0x00},
		{0x2f, 0x2f, 0x00, 0x00},
		{0x30, 0x30, 0x00, 0x00},
		{0x31, 0x31, 0x00, 0x00},
		{0x32, 0x32, 0x00, 0x00},
		{0x33, 0x33, 0x00, 0x00},
		{0x34, 0x34, 0x00, 0x00},
		{0x35, 0x35, 0x00, 0x00},
		{0x36, 0x36, 0x00, 0x00},
		{0x37, 0x37, 0x00, 0x00},
		{0x38, 0x38, 0x00, 0x00},
		{0x39, 0x39, 0x00, 0x00},
		{0x3a, 0x3a, 0x00, 0x00},
		{0x3b, 0x3b, 0x00, 0x00},
		{0x3c, 0x3c, 0x00, 0x00},
		{0x3d, 0x3d, 0x00, 0x00},
		{0x3e, 0x3e, 0x00, 0x00},
		{0x3f, 0x3f, 0x00, 0x00},
	}
};

static unsigned long last_time;
char time[21];

void ds1307_software_reset(void)
{
	for (struct ds1307_reg *p = (struct ds1307_reg *)&ds1307_instance_reg;
		p != (struct ds1307_reg *)&ds1307_instance_reg +
		sizeof (ds1307_instance_reg) / sizeof (struct ds1307_reg); ++p)
		if (p->init_value != NA) {
			i2c_master_smbus_write_byte(I2C, DS1307_SLAVE_ADDR,
				SMBUS_TIMEOUT, p->write_ptr, p->init_value);

			i2c_master_smbus_read_byte(I2C, DS1307_SLAVE_ADDR,
				SMBUS_TIMEOUT, p->read_ptr, &p->present_value);

			if (p->present_value != p->init_value) {
				printf("Reset read_ptr: %x [%x] failed.\n",
						p->read_ptr, p->init_value);
			}
			debug("Reset read_ptr: %x [%x]\n", p->read_ptr,
					p->init_value);
		}
}

/* Ex: ds1307_reg_read(ds1307_instance_reg.seconds); */
uint8_t ds1307_reg_read(struct ds1307_reg ds1307_reg_tmp)
{
	i2c_master_smbus_read_byte(I2C, DS1307_SLAVE_ADDR, SMBUS_TIMEOUT,
			ds1307_reg_tmp.read_ptr, &ds1307_reg_tmp.present_value);
	return ds1307_reg_tmp.present_value;
}

/* Ex: ds1307_reg_write(ds1307_instance_reg.ram[10], 0x10); */
int ds1307_reg_write(struct ds1307_reg ds1307_reg_tmp, uint8_t value)
{
	i2c_master_smbus_write_byte(I2C, DS1307_SLAVE_ADDR, SMBUS_TIMEOUT,
			ds1307_reg_tmp.write_ptr, value);
	i2c_master_smbus_read_byte(I2C, DS1307_SLAVE_ADDR, SMBUS_TIMEOUT,
			ds1307_reg_tmp.read_ptr, &ds1307_reg_tmp.present_value);
	if (ds1307_reg_tmp.present_value != value) {
		printf("Wrote 0x%x to 0x%x Failed.\n", value, ds1307_reg_tmp.read_ptr);
		return -1;
	}
	return 0;
}

void ds1307_update_time(void)
{
	for (struct ds1307_reg *p = (struct ds1307_reg *)&ds1307_instance_reg;
		p != (struct ds1307_reg *)&ds1307_instance_reg + DS1307_TIME_REG_MAX;
		++p) {
		i2c_master_smbus_read_byte(I2C, DS1307_SLAVE_ADDR, SMBUS_TIMEOUT,
				p->read_ptr, &p->present_value);
//		debug("Time read_ptr: %x [%x]\n", p->read_ptr, p->present_value);
	}
}

void ds1307_update_ram(void)
{
	for (struct ds1307_reg *p = (struct ds1307_reg *)&ds1307_instance_reg +
		DS1307_TIME_REG_MAX; p != (struct ds1307_reg *)&ds1307_instance_reg +
		     sizeof (ds1307_instance_reg) / sizeof (struct ds1307_reg);
		++p) {
		i2c_master_smbus_read_byte(I2C, DS1307_SLAVE_ADDR, SMBUS_TIMEOUT,
				p->read_ptr, &p->present_value);
//		debug("RAM read_ptr: %x [%x]\n", p->read_ptr, p->present_value);
	}
}

void ds1307_get_time(void) {
	char tmp[3];
	memset(time, 0, sizeof(time));
	time[0] = '2';
	time[1] = '0';
	for (struct ds1307_reg *p = (struct ds1307_reg *)&ds1307_instance_reg +
		DS1307_TIME_REG_MAX; p != (struct ds1307_reg *)&ds1307_instance_reg;
	     	--p) {
		sprintf(tmp, "%02X", (p - 1)->present_value);
		strcat(time, tmp);
		strcat(time, " ");
	}
	time[4] = time[7] = '-';
	time[16] = time[19] = ':';
//	debug("Time: %s\n", time);
}

static void ds1307_process(void)
{
	/* Main loop query */
	unsigned long current_time = tick_get();

	if (current_time - last_time < DS1307_COLLECT_INTERVAL)
		return;

	last_time = current_time;

	ds1307_update_time();
	ds1307_update_ram();
	ds1307_get_time();
}

void ds1307_init(void)
{
//	ds1307_software_reset();

	ds1307_update_time();
	ds1307_update_ram();

	last_time = tick_get();

	loop_add(ds1307_process);
}
