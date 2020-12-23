#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <debug.h>
#include <stdint.h>
#include <tick.h>

unsigned long tim6_cnt;

int timer_setup(void)
{
	rcc_periph_clock_enable(RCC_TIM6);
	timer_disable_counter(TIM6);

	return 0;
}

void timer_start(unsigned long us)
{
	timer_disable_counter(TIM6);

#if 0
	uint32_t div, cnt;
	uint64_t clk = (uint64_t)us * rcc_apb1_frequency / 1000000;

	if (clk > 0xffff) {
		div = (clk / 0xffff);
		cnt = clk - (div * 0xffff);
	}
	else {
		div = 1;
		cnt = clk;
	}
	timer_set_prescaler(TIM6, div - 1);
	timer_set_period(TIM6, cnt);
#else
	/* set to about 1M Hz */
	timer_set_prescaler(TIM6, 32 - 1);
	timer_set_period(TIM6, us);
#endif
	timer_one_shot_mode(TIM6);
	timer_generate_event(TIM6, TIM_EGR_UG);

	timer_enable_counter(TIM6);
}

void timer_stop(void)
{
	timer_disable_counter(TIM6);
}

int timer_is_timeout(void)
{
	int timeout = (TIM_CR1(TIM6) & TIM_CR1_CEN) == 0;

	if (timeout)
		timer_stop();

	return timeout;
}

void tim6_dac_isr(void)
{
	debug("tim6 isr %lu\n", (unsigned long)tick_get());
	++tim6_cnt;
	timer_clear_flag(TIM6, TIM_SR_UIF);
}

void timer_mdelay(unsigned long ms)
{
	if (ms == 0)
		return;

	timer_start(ms * 1000);
	while (!timer_is_timeout())
		;
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
#if 0
	nvic_enable_irq(NVIC_TIM6_DAC_IRQ);
	timer_enable_irq(TIM6, TIM_DIER_UIE);
	timer_start(1 * 1000 * 1000);
	debug("apb1 frequency %ld\n", rcc_apb1_frequency);
#else
	unsigned long i;

	debug("start timer\n");
	for (i = 0; i < 60; ++i) {
		timer_start(1 * 1000 * 1000);
		while (!timer_is_timeout())
			;
		// timer_stop();
		debug("%ld seconds\n", i);
	}
#endif
}

