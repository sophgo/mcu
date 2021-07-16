#include <stdbool.h>
#include <pin.h>
#include <power.h>
#include <chip.h>
#include <loop.h>

volatile static int listen;

static void wol_process(void)
{
	if (chip_is_enabled() || power_status())
		return;

	if (!listen)
		return;

	if (gpio_get(PMEB_PORT, PMEB_PIN))
		return;

	listen = false;
	power_on();
	chip_enable();
}

void wol_init(void)
{
	listen = false;
	loop_add(wol_process);
}

void wol_start_listen(void)
{
	listen = true;
}

void wol_stop_listen(void)
{
	listen = false;
}

