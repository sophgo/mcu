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
#include <ctype.h>
#include <system.h>
#include <project.h>
#include <console.h>

int main(void)
{
	system_init();
	console_init();

	printf("\nwelcome to sophon %s\n", project_name());

	while (true) {
		console_poll();
	}

	return 0;
}


