#ifndef __TICK_H__
#define __TICK_H__

void tick_init(void);
uint32_t tick_get(void);

extern volatile uint32_t tick;

#endif
