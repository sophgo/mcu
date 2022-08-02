#ifndef __CHIP_H__
#define __CHIP_H__

#include <stdint.h>

void chip_init(void);
void chip_enable(void);
void chip_disable(void);
void chip_reset(void);
void chip_popd_reset_early(void);
void chip_popd_reset_end(void);
uint32_t chip_uptime(void);
uint32_t chip_reset_times(void);
int chip_is_enabled(void);
void set_needpoweron(void);
int get_needpoweron_satus(void);
void clr_needpoweron(void);
#endif
