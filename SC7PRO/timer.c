#include <gd32e50x_timer.h>
#include <stdint.h>
#include <debug.h>
#include <timer.h>

void timer_start(unsigned long us)
{
	timer_disable(TIMER6);

	/* set to about 1M Hz */
	timer_prescaler_config(TIMER6, 180 - 1, TIMER_PSC_RELOAD_NOW);
	timer_autoreload_value_config(TIMER6, us);
	timer_single_pulse_mode_config(TIMER6, TIMER_SP_MODE_SINGLE);

	timer_enable(TIMER6);
}

void timer_stop(void)
{
	timer_disable(TIMER6);
}

int timer_is_timeout(void)
{
	int timeout = (TIMER_CTL0(TIMER6) & TIMER_CTL0_CEN) == 0;

	if (timeout)
		timer_stop();

	return timeout;
}

void timer_mdelay(unsigned long ms)
{
	int i;

	for (i = 0; i < ms; ++i)
		timer_udelay(1000);
}

void timer_udelay(unsigned long us)
{
	if (us == 0)
		return;

	timer_start(us);
	while (!timer_is_timeout())
		;
}

void timer_test(void)
{
	unsigned long i;

	debug("start timer\n");
	for (i = 0; i < 60; ++i) {
		timer_start(1 * 1000 * 1000);
		while (!timer_is_timeout())
			;
		// timer_stop();
		debug("%ld seconds\n", i);
	}
}

