#ifndef __TICK__
#define __TICK__

void tick_init(void);
void tick_destroy(void);
void mdelay(unsigned long ms);
unsigned long tick_get(void);
void tick_set(unsigned long n);
int tick_register_task(void (*func)(void), unsigned long interval);
void tick_set_task_interval(int handle, unsigned long interval);
uint64_t tick64_get(void);
void tick64_set(uint64_t n);
#endif
