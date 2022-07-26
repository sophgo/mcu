/*
 * mcu.h
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */

#ifndef MCU_H_
#define MCU_H_

extern void mcu_init(void);
void mcu_tick_isr();
void mcu_set_temp(int soc, int board);
void set_mcu_default_feature(void);

#define CRITICAL_ACTION_POWERDOWN	2
#define CRITICAL_ACTION_REBOOT		1

#endif /* MCU_H_ */

