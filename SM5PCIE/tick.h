#ifndef __TICK_H__
#define __TICK_H__

void tick_init(unsigned long freq);
void mdelay(unsigned long ms);
unsigned long long tick_get(void);
void tick_test(void);

#endif
