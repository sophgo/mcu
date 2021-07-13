#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <chip.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <common.h>
#include <loop.h>
#include <power.h>
#include <stdbool.h>
#include <tca6416a.h>

static uint32_t uptime;
static uint32_t reset_times;
static int chip_enabled;

uint32_t chip_reset_times(void)
{
	return reset_times;
}

uint32_t chip_uptime(void)
{
	return uptime;
}

void chip_disable(void)
{
	tca6416a_reset();
	gpio_clear(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	tca6416a_error_led_on();
	gpio_set(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = true;
}

int chip_is_enabled(void)
{
	return chip_enabled;
}

void chip_reset(void)
{
	chip_disable();
	mdelay(30);
	uptime = 0;
	++reset_times;
	chip_enable();
}

void chip_popd_reset(void)
{
	chip_disable();
	power_off();

	mdelay(2000);
	++reset_times;

	power_on();
	chip_enable();
}

static void chip_process(void)
{
	if (chip_enabled)
		++uptime;
	else
		uptime = 0;
}

void chip_init(void)
{
	tick_register_task(chip_process, 1000);
}

