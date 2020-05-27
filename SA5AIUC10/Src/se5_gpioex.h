#ifndef __SE5_GPIOEX_H__
#define __SE5_GPIOEX_H__

int se5_gpioex_init(void);
void se5_reset_board(void);
void se5_heater_ctrl(int temp);
void se5_smb_alert(void);

#endif
