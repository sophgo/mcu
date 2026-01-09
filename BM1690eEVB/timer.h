#ifndef __TIMER_H__
#define __TIMER_H__

int timer_setup(void);
void timer_start(unsigned long us);
unsigned long timer_counter(void);
void timer_stop(void);
void timer_test(void);
void timer_mdelay(unsigned long ms);
void timer_udelay(unsigned long us);
int timer_is_timeout(void);

#endif
