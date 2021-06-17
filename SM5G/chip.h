#ifndef __CHIP_H__
#define __CHIP_H__

#include <stdint.h>

void chip_init(void);
void chip_enable(void);
void chip_disable(void);
void chip_reset(void);
void chip_popd_reset(void);
uint32_t chip_uptime(void);
uint32_t chip_reset_times(void);
int chip_is_enabled(void);

#endif
