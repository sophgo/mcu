#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <common.h>

#define TICK_TASK_MAX	4

/* software system tick in ms */
static volatile unsigned long tick;
static volatile int task_num;
static volatile struct {
	unsigned long tick;
	unsigned long interval;
	void (*func)(void);
} task[TICK_TASK_MAX];

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

int tick_register_task(void (*func)(void), unsigned long interval)
{
	if (task_num >= ARRAY_SIZE(task))
		return -1;

	task[task_num].tick = 0;
	task[task_num].interval = interval;
	task[task_num].func = func;
	++task_num;
	return 0;
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
	int i;

	++tick;

	for (i = 0; i < task_num; ++i) {
		++task[i].tick;
		if (task[i].tick >= task[i].interval)
			task[i].func();
	}
}
