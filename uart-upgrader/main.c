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
#include <ymodem.h>
#include <md5.h>

#define FLASH_SIZE	(64 * 1024)

static int save_data(void *data, unsigned long len);
static int check_firmware(void);

int main(void)
{
	int err;

	system_init();
	systick_counter_enable();
	cm_enable_interrupts();

	ymodem_receive(save_data, FLASH_SIZE);

	printf("checking firmware\n");
	err = check_firmware();

	while (1) {
		if (err)
			printf("firmware upgrade failed\n");
		else
			printf("firmware upgrade success\n");
		mdelay(1000);
	}

	return 0;
}

/* xmodem packet size is 128
 * ymodem packet size is 1024
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
	md5_update(&ctx, (void *)(FLASH_BASE), FLASH_SIZE - 16);
	md5_final(&ctx, md5);
	return memcmp(md5, (void *)(((unsigned long)FLASH_BASE) + FLASH_SIZE - 16),
		      16);
}
