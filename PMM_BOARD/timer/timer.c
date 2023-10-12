/*
 * timer.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <gd32e50x_timer.h>
#include <stdint.h>
#include <debug.h>
#include <timer/timer.h>

#define TIMER6_COUNTER_MAX	65536

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

void timer_delay_ms(unsigned long ms)
{
	int i;

	for (i = 0; i < ms; ++i)
		timer_delay_us(1000);
}

void timer_delay_us(unsigned long us)
{
	if (us == 0)
		return;

	while (us > (TIMER6_COUNTER_MAX - 1)) {
		timer_start(TIMER6_COUNTER_MAX - 1);
		while (!timer_is_timeout())
			;
		us = us - (TIMER6_COUNTER_MAX - 1);
	}

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

