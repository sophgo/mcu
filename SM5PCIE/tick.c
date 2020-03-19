#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <debug.h>

/* software system tick in ms */
static volatile unsigned long tick;

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
}

void tick_test(void)
{
	int i;
	for (i = 0; i < 10; ++i) {
		debug("%d seconds\r\n", i);
		mdelay(1000);
	}
}
