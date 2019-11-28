/*
 * wdt.h
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */

#ifndef WDT_H_
#define WDT_H_

extern void wdt_init(void);
void wdt_isr(void);
void soc_wdt_reset_process(void);

#endif /* WDT_H_ */
