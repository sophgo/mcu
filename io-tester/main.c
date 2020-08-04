/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/scb.h>
#include <tick.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

struct gpio {
	unsigned long port, pin;
};

static const unsigned long gpio_ports[] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH,
};

static const struct gpio gpio_exclude[] = {
	{GPIOA, GPIO13},	/* SWDIO */
	{GPIOA, GPIO14},	/* SWCLK */
};

static bool is_exclude(const unsigned long port, const unsigned long pin)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(gpio_exclude); ++i) {
		const struct gpio *p = gpio_exclude + i;

		if (port == p->port && pin == p->pin)
			return true;
	}

	return false;
}

static void gpio_setup(const unsigned long port)
{
	int i;
	unsigned long pins = 0;

	for (i = 0; i < 16; ++i) {
		unsigned long pin = 1 << i;

		if (is_exclude(port, pin))
			continue;

		pins |= pin;
	}
	gpio_set(port, pins);
	gpio_set_output_options(port, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, pins);
	gpio_mode_setup(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pins);
}

static void gpio_update(const unsigned long port)
{
	int i;
	unsigned long pins = 0;

	for (i = 0; i < 16; ++i) {
		unsigned long pin = 1 << i;

		if (is_exclude(port, pin))
			continue;

		pins |= pin;
	}

	gpio_toggle(port, pins);
}

int main(void)
{
	int i;

	SCB_VTOR = (uint32_t)&vector_table;

	/* enable power and clock for all gpio ports */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOH);

	for (i = 0; i < ARRAY_SIZE(gpio_ports); ++i)
		gpio_setup(gpio_ports[i]);


	tick_init();

	while (true) {
		mdelay(500);
		for (i = 0; i < ARRAY_SIZE(gpio_ports); ++i)
			gpio_update(gpio_ports[i]);
	}

	return 0;
}
