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

#endif /* MCU_H_ */

