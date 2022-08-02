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

static uint32_t uptime;
static uint32_t reset_times;
static int chip_enabled;
static int needpoweron;

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
	gpio_clear(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	gpio_set(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = true;
}

void chip_reset(void)
{
	chip_disable();
	mdelay(30);
	uptime = 0;
	++reset_times;
	chip_enable();
}

void chip_popd_reset_early(void)
{
	power_off();
	chip_disable();
	mdelay(50);
}

void chip_popd_reset_end(void)
{
	power_on();
	uptime = 0;
	++reset_times;
	chip_enable();
}

static void chip_process(void)
{
	if (chip_enabled)
		++uptime;
}

void chip_init(void)
{
	tick_register_task(chip_process, 1000);
}

int chip_is_enabled(void)
{
	return chip_enabled;
}

void set_needpoweron(void)
{
	needpoweron = 1;
}

void clr_needpoweron(void)
{
	needpoweron = 0;
}

int get_needpoweron_satus(void)
{
	return needpoweron;
}
