#include <string.h>
#include <stdio.h>
#include <xmodem.h>
#include <libopencm3/stm32/flash.h>
#include <system.h>
#include <md5.h>

static int __attribute__((unused))
	save_data(void *priv, void *data, unsigned long len);
static int __attribute__((unused))
	dummy_save(void *data, unsigned long len)
{
	return 0;
}

/* xmodem packet size is 128 or 1024
 * so we donnot need to handle unaligned write
 */

struct flash_ctx {
	unsigned long offset;
};


static int save_data(void *priv, void *data, unsigned long len)
{
	unsigned long i;
	struct flash_ctx *ctx = priv;
	unsigned long flash_size;

	flash_size = system_get_flash_size();

	for (i = 0; i < len; i += FLASH_PAGE_SIZE,
				ctx->offset += FLASH_PAGE_SIZE) {
		if (ctx->offset >= flash_size)
			return -1;
		flash_program_page(ctx->offset, ((char *)data) + i);
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
