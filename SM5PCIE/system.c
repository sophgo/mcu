/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <tick.h>
#include <stdstub.h>

static void gpio_init(void)
{
	/* enable all gpio power and clock */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
#if 0
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOH);
#endif
	/* GPIO port A */
	/* spi nss */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO15);
	/* uart4 tx, rx */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO0 | GPIO1);
	gpio_set_af(GPIOA, GPIO_AF6, GPIO0 | GPIO1);
	/* adc 5 way */
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
			GPIO3 | GPIO4 | GPIO5 | GPIO6 | GPIO7);

	/* GPIO port B */
	/* spi clk, miso, mosi */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO3 | GPIO4 | GPIO5);
	gpio_set_af(GPIOB, GPIO_AF0, GPIO3 | GPIO4 | GPIO5);
	/* i2c1 scl, sda */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,
			GPIO8 | GPIO9);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8 | GPIO9);
	/* led, sdoe */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			GPIO0 | GPIO1 | GPIO2 | GPIO7);
}

void system_init(void)
{
	/* we may load this to a place other than default reset address */
	/* so relocated vtor -- vector table offset register */
	SCB_VTOR = (uint32_t)&vector_table;

	gpio_init();
	tick_init();
	std_stub_init();
}
