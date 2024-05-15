#ifndef __BOARD_POWER_IMPL_H__
#define __BOARD_POWER_IMPL_H__

#include <stdbool.h>

bool is_evb_power_key_on(void);
void milkv_warm_poweroff(void);
void milkv_warm_reboot(void);
void board_power_control(void);
void milkv_auto_power_on(void);

#endif