/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/usart.h>
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

	return 0;
}

static int save_data(void *data, unsigned long len)
{
	return 0;
}
