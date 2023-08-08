/*
 * power.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __POWER_H__
#define __POWER_H__

#include <stdint.h>

enum {
	NODE_TYPE_ENABLE,
	NODE_TYPE_CHECK,
	NODE_TYPE_FUNCTION,
	NODE_TYPE_INVALID,
};

enum {
	POWER_STATUS_OFF,
	POWER_STATUS_ON,
	POWER_STATUS_ERR,
};

struct board_power_nodes {
	char *name;
	int type;
	unsigned int delay;
	unsigned long param[2];
};

typedef int (*power_on_func)(void);
typedef int (*power_off_func)(void);

extern int power_is_on;

void power_init(void);
void power_on(void);
void power_off(void);
int power_status(void);

#endif /* __POWER_H__ */
