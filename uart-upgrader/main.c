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

#define FLASH_SIZE	(64 * 1024)

static int save_data(void *data, unsigned long len);

int main(void)
{
	system_init();
	systick_counter_enable();
	cm_enable_interrupts();

	ymodem_receive(save_data, FLASH_SIZE);

	printf("firmware upgrade done\n")

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
