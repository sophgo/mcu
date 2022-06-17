#include <gd32e50x_fmc.h>
#include <flash.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <xmodem.h>
#include <system.h>
#include <md5.h>
#include <common.h>

static int __attribute__((unused))
	save_data(void *priv, void *data, unsigned long len);
static int __attribute__((unused))
	dummy_save(void *data, unsigned long len)
{
	return 0;
}

/* data and len must 4 bytes align */
void flash_program_page(unsigned long addr, void *data, unsigned int len)
{
	int i;
	uint32_t tmp;

	fmc_unlock();

	fmc_page_erase(addr);

	for (i = 0; i < len / 4; ++i) {
		tmp = ((uint32_t *)data)[i];

		if (tmp != 0xffffffff)
			fmc_word_program(addr + i * 4, tmp);
	}

	fmc_lock();

	fmc_flag_clear(FMC_FLAG_END);
}

/* xmodem packet size is 128 or 1024
 * so we donnot need to handle unaligned write
 */

struct flash_ctx {
	unsigned long offset;
};
/* we donnot consider that data need to save in two page*/

int flash_program_small_page(unsigned long offset, void *data, unsigned int len)
{
	static unsigned int ereased_start = 0;
	static unsigned int ereased_end = 0;
	unsigned long addr = FLASH_BASE + offset;
	unsigned int start = addr;
	unsigned int end = addr + len;
	int i = 0;
	uint32_t tmp;

	if (start < ereased_start || end > ereased_end){
		fmc_unlock();
		ereased_start = (addr >> 13)<<13;
		ereased_end = ereased_start + FLASH_PAGE_SIZE;
		fmc_page_erase(ereased_start);
		fmc_lock();
		fmc_flag_clear(FMC_FLAG_END);
		if (end > ereased_end){
			return 0;
		}
	}

	fmc_unlock();
	for (i = 0; i < len/4; i++){
		tmp = ((uint32_t *)data)[i];

		if (tmp != 0xffffffff)
			fmc_word_program(addr + i * 4, tmp);
	}
	fmc_lock();
	fmc_flag_clear(FMC_FLAG_END);

	return len;
}

static int save_data(void *priv, void *data, unsigned long len)
{
	unsigned long i;
	struct flash_ctx *ctx = priv;
	unsigned long flash_size;
	int ret_len = 0;

	flash_size = GD32E50X_FLASH_SIZE;

	for (i = 0; i < len; i += ret_len,
				ctx->offset += ret_len) {
		if (ctx->offset >= flash_size)
			return -1;
		ret_len = flash_program_small_page(ctx->offset, ((char *)data) + i, len);
		if (ret_len == 0){
			return -1;
		}
	}

	return 0;
}

long download_to_flash(unsigned long offset)
{
	struct flash_ctx ctx;

	ctx.offset = offset;

	return xmodem_receive(save_data, &ctx);
}

static void __attribute__((unused))
	hex_dump(void *hex, unsigned long len)
{
	uint8_t *p;
	unsigned long i;

	for (p = hex, i = 0; i < len; ++i)
		printf("%02x", p[i]);
	printf("\n");
}

int check_firmware(unsigned long size)
{
	struct md5_ctx ctx;
	uint8_t md5[16];

	md5_init(&ctx);
	md5_update(&ctx, (void *)(FLASH_BASE), size - 128);
	md5_final(&ctx, md5);
	return memcmp(md5,
		      (void *)(((unsigned long)FLASH_BASE) + size - 128),
		      16);
}

void flash_clear_error(void)
{
	fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_PGAERR |
		       FMC_WPERR | FMC_FLAG_END);
}

uint8_t eeprom_read_byte(uint32_t offset)
{
	return *((volatile uint8_t *)(EEPROM_BASE + offset));
}

void eeprom_write_byte(uint32_t offset, uint8_t data)
{
	uint8_t flash_data[FLASH_PAGE_SIZE] = {0};
	volatile uint8_t *p_eeprom = (uint8_t *) EEPROM_BASE;

	int i = 0;

	for (i = 0; i < FLASH_PAGE_SIZE; i++){
		flash_data[i] = p_eeprom[i];
	}

	flash_data[offset] = data;
	flash_program_page(EEPROM_BASE, flash_data, FLASH_PAGE_SIZE);
}

