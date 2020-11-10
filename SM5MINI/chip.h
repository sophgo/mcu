#ifndef __CHIP_H__
#define __CHIP_H__

#include <stdint.h>

void chip_init(void);
void chip_enable(void);
void chip_disable(void);
void chip_reset(void);
uint32_t chip_uptime(void);
uint32_t chip_reset_times(void);

#endif
