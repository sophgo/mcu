#include <upgrade.h>
#include <string.h>
#include <project.h>
#include <i2c_master.h>
#include <common.h>
#include <tick.h>
#include <flash.h>

#if 1
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
#endif

#include <upgrade.h>
#include <string.h>
#include <project.h>
#include <i2c_master.h>
#include <project.h>
#include <chip.h>

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
	// typedef void (*upgrade_entry)(uint32_t project);
	// struct efie *efie = find_efie(type);

	// if (efie == NULL)
	// 	return;

	/* disable sys tick */
	// systick_counter_disable();
	/* disable i2c1 and i2c2 */
	i2c_master_destroy(I2C0);
	i2c_master_destroy(I2C1);
	i2c_master_destroy(I2C2);

	chip_destroy();

	/* disable all interrupt */
	// cm_disable_interrupts();

#if 0
	/* working in thumb mode, we should add 1 to destination */
	/* thumb instruction pc should be even number */
	upgrade_entry entry = (upgrade_entry)
		(efie->offset + MEMMAP_FLASH_START + 1);
	entry(SC7FP150);
#endif
}

#if 0
void app_start(void)
{
	unsigned long app_start, app_entry, app_stack;
	struct efie *app_efie;

	app_efie = find_efie(RUN_STAGE_APP);

	app_start = app_efie->offset + MEMMAP_FLASH_START;
	app_entry = *(volatile uint32_t *)(app_start + 4);
	app_stack = *(volatile uint32_t*)app_start;

	asm volatile ("mov  sp, %0" ::"r"(app_stack));

	typedef void (*app_entry_t)(void);
	app_entry_t entry = (void *)app_entry;
	entry();
}

static struct efie *find_efie(uint32_t type)
{
	struct efie *p;

	for (p = (void *)MEMMAP_EFIT_START;
	     (unsigned long)p < MEMMAP_EFIT_END;
	     ++p) {
		if (p->type == type)
			return p;
	}
	return NULL;
}

int get_stage(void)
{
	register uint32_t pc;
	asm volatile ("mov %0, pc" : "=r" (pc));
	if (pc < MEMMAP_LOADER_END)
		return RUN_STAGE_LOADER;
	else
		return RUN_STAGE_APP;
}

static void checksum(void *out, void *in, unsigned long len)
{
	uint32_t *src = in;
	char *init = "*BITMAIN-SOPHON*";
	uint32_t result[4];
	memcpy(result, init, sizeof(result));

	unsigned long block = len >> 4;
	unsigned long left = len & 0xf;
	unsigned long i, j;

	for (i = 0; i < block; ++i, src += 4) {
		for (j = 0; j < 4; ++j) {
			result[j] ^= src[j];
		}
	}

	for (i = 0; i < left; ++i) {
		((uint8_t *)result)[i] ^= ((uint8_t *)src)[i];
	}
	memcpy(out, result, sizeof(result));
}

/*
 * 0 means success, app is complete upgraded
 * -1 means failed, app is broken
 */
int check_app(void)
{
	void *checksum_start;
	unsigned long checksum_len;
	uint8_t *expected_checksum, calculated_checksum[16];
	struct efie *app_efie;

	app_efie = find_efie(RUN_STAGE_APP);

	if (app_efie->is_checked)
		return 0;

	/* get application efie */
	checksum_start = (void *)(app_efie->offset + MEMMAP_FLASH_START);
	checksum_len = app_efie->length;
	expected_checksum = app_efie->checksum;
	if ((unsigned long)checksum_start < MEMMAP_FLASH_START ||
	    (unsigned long)checksum_start >= MEMMAP_FLASH_END) {
		return -1;
	}
	/* we know stm32 address is 32bit long,
	 * a 64bit var can handle overflow. ^_^
	 */
	uint64_t checksum_end = (unsigned long)checksum_start + checksum_len;

	if (checksum_end > MEMMAP_FLASH_END) {
		return -1;
	}

	checksum(calculated_checksum, checksum_start, checksum_len);
	if (memcmp(expected_checksum, calculated_checksum, 16))
		/* failed */
		return -1;

	/* looks good */
	flash_clear_error();
	struct efie app_efie_backup;

	memcpy(&app_efie_backup, app_efie, sizeof(app_efie_backup));

	/* mark app as checked */
	app_efie_backup.is_checked = 1;
	// flash_program_page((unsigned long)app_efie, &app_efie_backup);

	return 0;
}
#endif
