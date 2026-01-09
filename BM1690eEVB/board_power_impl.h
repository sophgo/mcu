#ifndef __BOARD_POWER_IMPL_H__
#define __BOARD_POWER_IMPL_H__

#include <stdbool.h>

void board_power_control(void);
void atx_power_on(void);
void atx_power_off(void);

#endif
