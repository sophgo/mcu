#include <gd32e50x_timer.h>
#include <stdint.h>
#include <debug.h>
#include <timer.h>
#include <system.h>

static volatile unsigned long tick_count;

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

void timer1_init()
{
	timer_parameter_struct tim_struct = {0};

	rcu_periph_clock_enable(RCU_TIMER1);

	tim_struct.counterdirection = TIMER_COUNTER_UP;  // 向上计数
	tim_struct.prescaler = (2 - 1);  // 预分频：180MHz / 2 = 90MHz
	tim_struct.period = (90 - 1);  // 90 / 90MHz = 1us

	timer_init(TIMER1, &tim_struct);

	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	nvic_irq_enable(TIMER1_IRQn, 1, 1);

	timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);

	timer_enable(TIMER1);

}

void set_timer1_tick(unsigned long temp)
{
	timer_interrupt_disable(TIMER1,TIMER_INT_UP);
	tick_count = temp ;
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
}

unsigned long get_timer1_tick(void)
{
	timer_interrupt_disable(TIMER1,TIMER_INT_UP);
	return tick_count;
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
}

void TIMER1_IRQHandler()
{
	tick_count++;
	timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
}

