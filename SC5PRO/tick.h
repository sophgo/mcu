#ifndef __TICK_H__
#define __TICK_H__

#include <stdint.h>

void tick_init(void);
void mdelay(unsigned long ms);
unsigned long tick_get(void);
uint64_t tick64_get(void);
void tick64_set(uint64_t n);

#endif
