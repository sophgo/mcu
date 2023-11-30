#include <chip.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <common.h>
#include <loop.h>
#include <power.h>
#include <stdbool.h>

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
	gpio_clear(SYS_RST_X_PORT, SYS_RST_X_PIN);
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	gpio_set(SYS_RST_X_PORT, SYS_RST_X_PIN);
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
	power_off();
	mdelay(50);
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

