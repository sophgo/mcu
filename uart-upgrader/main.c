/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/cortex.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <system.h>
#include <xmodem.h>
#include <md5.h>

#define FLASH_SIZE	(64 * 1024)

static int __attribute__((unused))
   	save_data(void *data, unsigned long len);
static int __attribute__((unused))
	dummy_save(void *data, unsigned long len)
{
	return 0;
}

static int check_firmware(void);
static uint8_t md5_digest[16];

static void __attribute__((unused))
	hex_dump(void *hex, unsigned long len)
{
	uint8_t *p;
	unsigned long i;

	for (p = hex, i = 0; i < len; ++i)
		printf("%02x", p[i]);
	printf("\r\n");
}

int main(void)
{
	int err;

	system_init();

	printf("\r\nsend your firmware through xmodem\r\n");

	xmodem_receive(save_data, FLASH_SIZE);

	printf("\r\nchecking firmware\r\n");
	err = check_firmware();

	while (1) {
		if (err)
			printf("firmware upgrade failed\r\n");
		else
			printf("firmware upgrade success\r\n");
		/*
		printf("md5sum: ");
		hex_dump(md5_digest, sizeof(md5_digest));
		*/
		mdelay(1000);
	}

	return 0;
}

/* xmodem packet size is 128 or 1024
 * so we donnot need to handle unaligned write
 */

static unsigned long flash_offset;

static int save_data(void *data, unsigned long len)
{
	unsigned long i;

	for (i = 0; i < len; i += FLASH_PAGE_SIZE,
				flash_offset += FLASH_PAGE_SIZE)
		flash_program_page(flash_offset, ((char *)data) + i);

	return 0;
}

int check_firmware(void)
{
	struct md5_ctx ctx;
	uint8_t md5[16];

	md5_init(&ctx);
	md5_update(&ctx, (void *)(FLASH_BASE), FLASH_SIZE - 128);
	md5_final(&ctx, md5);
	memcpy(md5_digest, md5, sizeof(md5_digest));
	return memcmp(md5, (void *)(((unsigned long)FLASH_BASE) + FLASH_SIZE - 128),
		      16);
}
