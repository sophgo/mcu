#ifndef __POWER_H__
#define __POWER_H__

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

struct power_node {
	char *name;
	int type;
	unsigned int delay;
	unsigned long param[2];
};

typedef int (*power_on_func)(void);
typedef int (*power_off_func)(void);

void power_init(void);
int power_on(void);
void power_off(void);
int power_status(void);
int board_power_init(void);
#endif
