#include <gd32e50x_misc.h>
#include <stdio.h>
#include <common.h>

#define HCLK		(180 * 1000 * 1000)
#define SYSTICK_CLK	(HCLK / 8)
#define SYSTICK_HZ	(1000)

static volatile unsigned long tick;
static volatile uint64_t tick64;

#define TICK_TASK_MAX	8

static int task_num;
static struct {
	unsigned long tick;
	unsigned long interval;
	void (*func)(void);
} task[TICK_TASK_MAX];

void tick_init(void)
{
	nvic_irq_disable(SysTick_IRQn);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD = SYSTICK_CLK / SYSTICK_HZ;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
	nvic_irq_enable(SysTick_IRQn, 0, 0);
}

void tick_destroy(void)
{
	nvic_irq_disable(SysTick_IRQn);
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
	SysTick->VAL = 0;
}

void mdelay(unsigned long ms)
{
	unsigned long cur = tick;
	/* make sure, wait time is larger than the given one */
	while (tick - cur <= ms)
		;
}

void tick_set(unsigned long n)
{
	__disable_irq();
	tick = n;
	__enable_irq();
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

/* overwrite the default one */
void SysTick_Handler(void)
{
	int i;

	++tick;
	++tick64;

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
