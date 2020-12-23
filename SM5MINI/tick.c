#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>
#include <libopencmsis/core_cm3.h>
#include <common.h>

#define TICK_TASK_MAX	8

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
	return task_num - 1;
}

void tick_set_task_interval(int handle, unsigned long interval)
{
	__disable_irq();
	task[handle].interval = interval;
	task[handle].tick = 0;
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
	int i;

	++tick;

	for (i = 0; i < task_num; ++i) {
		if (task[i].interval == 0)
			continue;
		++task[i].tick;
		if (task[i].tick >= task[i].interval) {
			task[i].func();
			task[i].tick = 0;
		}
	}
}
