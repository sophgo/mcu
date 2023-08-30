#ifndef __TICK__
#define __TICK__

void tick_init(void);
void tick_destroy(void);
void mdelay(unsigned long ms);
unsigned long tick_get(void);
int tick_register_task(void (*func)(void), unsigned long interval);
void tick_set_task_interval(int handle, unsigned long interval);

#endif
