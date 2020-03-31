#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <debug.h>
#include <stdint.h>
#include <tick.h>

unsigned long tim6_cnt;

int timer_setup(unsigned long freq)
{
	uint32_t div;

	uint32_t clk = rcc_apb1_frequency / freq;

	rcc_periph_clock_enable(RCC_TIM6);
	timer_disable_counter(TIM6);
#if 0
	nvic_enable_irq(NVIC_TIM6_DAC_IRQ);
	timer_enable_irq(TIM6, TIM_DIER_UIE);

	info("apb1 frequency %ld\r\n", rcc_apb1_frequency);
	info("clk %ld\r\n", clk);
#endif

	timer_update_on_overflow(TIM6);
	timer_set_master_mode(TIM6, TIM_CR2_MMS_UPDATE);

	if (clk > 0xffff)
		div = rcc_apb1_frequency / 0xffff;
	else
		div = 0;

	timer_set_prescaler(TIM6, div);
	timer_set_period(TIM6, (clk / (div + 1)) & 0xffff);


	return 0;
}

void timer_start(void)
{
	timer_enable_counter(TIM6);
}

void tim6_dac_isr(void)
{
	// info("tim6 isr %lu\r\n", (unsigned long)tick_get());
	++tim6_cnt;
	timer_clear_flag(TIM6, TIM_SR_UIF);
}

