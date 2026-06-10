/*
 * upgrade.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <upgrade/upgrade.h>
#include <string.h>
#include <project.h>
#include <i2c/i2c_master/i2c_master.h>
#include <common/common.h>
#include <tick/tick.h>

struct fw_info {
	uint8_t		md5[16];
	uint8_t		magic[4];
	uint8_t		type;
	uint8_t		mcu_family;
	uint8_t		r0[2];
	uint32_t	timestamp;
	uint8_t		r1[100];
} __attribute__((__packed__));

int stage;

void app_start(void)
{
	unsigned long app_start, app_entry, app_stack, offset;
	typedef void (*app_entry_t)(void);

	offset = MEMMAP_APP_START - MEMMAP_LOADER_START;
	app_start = MEMMAP_APP_START;
	/* append offset of app itself */
	app_entry = *(volatile uint32_t *)(app_start + 4) + offset;
	app_stack = *(volatile uint32_t *)app_start;
	app_entry_t entry = (void *)app_entry;

	asm volatile ("mov  sp, %0" ::"r"(app_stack));
	entry();
}

int get_stage(void)
{
	return stage;
}

/*
 * 0 means success, app is complete upgraded
 * -1 means failed, app is broken
 */
int check_app(void)
{
	struct fw_info *fw_info = (void *)MEMMAP_TAG_START;

	if (memcmp(fw_info->magic, "MCUF", sizeof(fw_info->magic)))
		return -1;

	return 0;
}

#include <upgrade/upgrade.h>
#include <string.h>
#include <project.h>
#include <i2c/i2c_master/i2c_master.h>

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		is_checked;
	uint8_t		type;
	uint8_t		padding[102];
} __packed;

static void upgrade_start(unsigned int type);
// static struct efie *find_efie(uint32_t type);

int stage;

void uart_upgrade_start(void)
{
	upgrade_start(RUN_STAGE_UART_UPGRADER);
}

void i2c_upgrade_start(void)
{
	upgrade_start(RUN_STAGE_UPGRADER);
}

static void upgrade_start(unsigned int type)
{
	i2c_master_destroy(I2C0);
	i2c_master_destroy(I2C1);
	i2c_master_destroy(I2C2);

}
