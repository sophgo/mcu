#include <project.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <system.h>

static int init(struct project *proj)
{
	system_set_stdout(USART2, RCC_USART2, 921600);

	rcc_periph_clock_enable(RCC_GPIOA);

	/* usart2 */
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2 | GPIO3);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH,
			GPIO2 | GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);

	return 0;
}

PROJECT(PROJ_SC5H, "sc5h", NULL, init);
