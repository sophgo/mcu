#include <stdbool.h>
#include <pin.h>
#include <power.h>
#include <chip.h>
#include <loop.h>

static int last_pmeb_status;
static int listen;

void wol_start(void)
{
	listen = true;
	last_pmeb_status = gpio_get(PMEB_PORT, PMEB_PIN);
}

void wol_stop(void)
{
	listen = false;
}

static void wol_process(void)
{
	int current_pmeb_status;

	if (!listen)
		return;

	current_pmeb_status = gpio_get(PMEB_PORT, PMEB_PIN);

	if (last_pmeb_status == 0 || current_pmeb_status == last_pmeb_status)
		goto noop;

	chip_power_on_enable();
noop:
	last_pmeb_status = current_pmeb_status;
}

void wol_init(void)
{
	listen = false;
	loop_add(wol_process);
}

