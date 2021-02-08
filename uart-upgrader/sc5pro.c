#include <project.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <system.h>

#define UART_PORT	USART4
#define UART_RCC	RCC_USART4

static int init(struct project *proj)
{
	system_set_stdout(UART_PORT, UART_RCC, 115200);

	rcc_periph_clock_enable(RCC_GPIOE);

	gpio_set_af(GPIOE, GPIO_AF6, GPIO8 | GPIO9);
	gpio_set_output_options(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			GPIO8 | GPIO9);
	gpio_mode_setup(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);

	return 0;
}

PROJECT(PROJ_SC5PRO, "SC5PRO", NULL, init);
