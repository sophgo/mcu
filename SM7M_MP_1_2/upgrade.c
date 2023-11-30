#include <upgrade.h>
#include <string.h>
#include <project.h>
#include <i2c_master.h>
#include <common.h>
#include <tick.h>
#include <flash.h>

struct fwinfo {
	uint8_t		md5[16];
	uint8_t		magic[4];
	uint8_t		type;
	uint8_t		mcu_family;
	uint8_t		r0[2];
	uint32_t	timestamp;
	uint8_t		r1[100];
} __attribute__((packed));

int stage;

void app_start(void)
{
	unsigned long app_start, app_entry, app_stack, offset;
	typedef void (*app_entry_t)(void);

	offset = MEMMAP_APP_START - MEMMAP_LOADER_START;
	app_start = MEMMAP_APP_START;
	/* append offset of app itself */
	app_entry = *(volatile uint32_t *)(app_start + 4) + offset;
	app_stack = *(volatile uint32_t*)app_start;
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
	struct fwinfo *fwinfo = (void *)MEMMAP_TAG_START;

	if (memcmp(fwinfo->magic, "MCUF", sizeof(fwinfo->magic)))
		return -1;

	return 0;
}
