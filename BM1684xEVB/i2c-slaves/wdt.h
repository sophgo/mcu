/*
 * wdt.h
 *
 *  Created on: Apr 25, 2019
 *      Author: weic
 */

#ifndef WDT_H_
#define WDT_H_

#include <i2c_slave.h>

void wdt_init(struct i2c_slave_ctx *i2c);
void wdt_isr(void);
void wdt_reset(void);

#endif /* WDT_H_ */
