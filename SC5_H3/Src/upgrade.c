#include "stm32l0xx_hal.h"
#include "main.h"
#include "upgrade.h"
#include "lptim.h"
#include "i2c.h"
#include <string.h>
#include "project.h"
#include "flash.h"

struct efie {
	uint32_t	offset;
	uint32_t	length;
	uint8_t		checksum[16];
	uint8_t		is_checked;
	uint8_t		reserved[103];
} __packed;

struct efie *app_efie = (void *)MEMMAP_EFIT_START;
struct efie *upgrader_efie = (void *)(MEMMAP_EFIT_START + MEMMAP_EFIE_SIZE);

void upgrade_start(void)
{
	  typedef void (*upgrade_entry)(uint32_t project);
	  // asm volatile ("xxx: b xxx");
	  /* disable sys tick */
	  SysTick->CTRL &= ~1;
	  /* disable i2c1 and i2c3 */

	  /* disable all interrupt */
	  __disable_irq();
	  __HAL_I2C_DISABLE(&hi2c1);

	  /* working in thumb mode, we should add 1 to destination */
	  /* thumb instruction pc should be even number */
	  upgrade_entry entry = (upgrade_entry)(upgrader_efie->offset + MEMMAP_FLASH_START + 1);
	  entry(SC5H3);
}

void app_start(void)
{
	unsigned long app_start, app_entry;
	app_start = app_efie->offset + MEMMAP_FLASH_START;
	app_entry = *(volatile uint32_t *)(app_start + 4);
	__set_MSP(*(volatile uint32_t*)app_start);
	typedef void (*app_entry_t)(void);
	app_entry_t entry = (void *)app_entry;
	entry();
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

	if (app_efie->is_checked)
		return 0;

	/* get application efie */
	checksum_start = (void *)(app_efie->offset + MEMMAP_FLASH_START);
	checksum_len = app_efie->length;
	expected_checksum = app_efie->checksum;
	if ((unsigned long)checksum_start < MEMMAP_FLASH_START || (unsigned long)checksum_start >= MEMMAP_FLASH_END) {
		return -1;
	}
	/* we know stm32 address is 32bit long, a 64bit var can handle overflow. ^_^ */
	uint64_t checksum_end = (unsigned long)checksum_start + checksum_len;

	if (checksum_end > MEMMAP_FLASH_END) {
		return -1;
	}

	checksum(calculated_checksum, checksum_start, checksum_len);
	if (memcmp(expected_checksum, calculated_checksum, 16))
		/* failed */
		return -1;

	/* looks good */
	flash_init();
	struct efie app_efie_backup;

	memcpy(&app_efie_backup, app_efie, sizeof(app_efie_backup));

	/* mark app as checked */
	app_efie_backup.is_checked = 1;
	flash_program((unsigned long)app_efie, &app_efie_backup, sizeof(app_efie_backup));

	return 0;
}

void setup_stage(void)
{
	register uint32_t pc;
	asm volatile ("mov %0, pc" : "=r" (pc));
	pc -= 4;
	if (pc < MEMMAP_LOADER_END)
		i2c_regs.stage = RUN_STAGE_LOADER;
	else
		i2c_regs.stage = RUN_STAGE_APP;

	if (i2c_regs.stage == RUN_STAGE_LOADER) {
		/* success */
		if (check_app() == 0) {
			/* fly to app, no return */
			app_start();
		}
	}
}

