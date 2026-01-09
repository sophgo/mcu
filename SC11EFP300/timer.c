#include <gd32f4xx_timer.h>
#include <stdint.h>
#include <debug.h>
#include <timer.h>
#include <system.h>


static volatile unsigned long tick_count;

#define TIMER6_COUNTER_MAX	65536


void timer_start(unsigned long us)
{
	timer_disable(TIMER6);

	timer_prescaler_config(TIMER6, 180-1, TIMER_PSC_RELOAD_NOW);
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

	if(timeout)
		timer_stop();

	return timeout;
}

void timer_mdelay(unsigned long ms)
{
	int i;

	for(i = 0; i < ms; i++)
		timer_udelay(1000);

}

void timer_udelay(unsigned long us)
{
	if(us == 0)
		return;

	while(us > (TIMER6_COUNTER_MAX-1)) {
		timer_start(TIMER6_COUNTER_MAX - 1);
		while(!timer_is_timeout())
			;
		us = us - (TIMER6_COUNTER_MAX - 1);
	}

	timer_start(us);
	while(!timer_is_timeout())
		;
}
