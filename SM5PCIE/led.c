#include <libopencm3/stm32/gpio.h>

static int led_status = 0;

void led_working(void)
{
	uint32_t io;
	io = 1 << led_status;
	++led_status;
	if (led_status == 3)
		led_status = 0;

	uint32_t tmp = (GPIO_ODR(GPIOB) & ~7) | (io & 7);
	GPIO_ODR(GPIOB) = tmp;
}

