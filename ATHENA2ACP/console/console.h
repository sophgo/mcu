/*
 * ecdc.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __CONSOLE_H_
#define __CONSOLE_H_

int console_init(void);
void console_add(void);
void console_poll(void);
void console_test(void);

#endif /* __CONSOLE_H_ */
