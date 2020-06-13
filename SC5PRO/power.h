#ifndef __POWER_H__
#define __POWER_H__

enum {
	NODE_TYPE_ENABLE,
	NODE_TYPE_FUNCTION,
};

#define GROUP_NUM	4

struct power_node {
	char *name;
	int type;
	unsigned int group_delay, node_delay;
	unsigned long param[GROUP_NUM][2];
};

typedef int (*power_on_func)(void);
typedef int (*power_off_func)(void);

void power_init(void);
int power_on(struct power_node const *node, unsigned int num);
void power_off(struct power_node const *node, unsigned int num);

#endif
