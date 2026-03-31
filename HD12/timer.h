#ifndef __TIMER_H__
#define __TIMER_H__

int timer_setup(void);
void timer_start(unsigned long us);
void timer_stop(void);

void timer_mdelay(unsigned long ms);
void timer_udelay(unsigned long us);
int timer_is_timeout(void);

#endif
