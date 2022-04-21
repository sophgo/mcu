#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencmsis/core_cm3.h>
#include <stdint.h>

/* software system tick in ms */
static volatile unsigned long tick;
static volatile uint64_t tick64;

void tick_init(void)
{
	systick_counter_disable();
	systick_clear();
	systick_set_frequency(1000, rcc_ahb_frequency);
	systick_interrupt_enable();
	systick_counter_enable();
}

unsigned long tick_get(void)
{
	return tick;
}

uint64_t tick64_get(void)
{
	uint64_t tmp;

	__disable_irq();
	tmp = tick64;
	__enable_irq();

	return tmp;
}

void tick64_set(uint64_t n)
{
	__disable_irq();
	tick64 = n;
	__enable_irq();
}

void mdelay(unsigned long ms)
{
	unsigned long cur = tick;
	/* make sure, wait time is larger than the given one */
	while (tick - cur <= ms)
		;
}

/* overwrite the default one */
void sys_tick_handler(void)
{
	++tick;
	++tick64;
}
