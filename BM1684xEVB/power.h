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
	int status;
	unsigned int delay;
	unsigned long param[2];
};

typedef int (*power_on_func)(void);
typedef int (*power_off_func)(void);

void power_init(void);
int power_on(void);
void power_off(void);
int power_node_on(const char *name);
int power_nodes_on(const char **name, int num);
void power_node_off(const char *name);
void power_nodes_off(const char **name, int num);
int power_node_status(const char *name);
int power_nodes_status(const char **name, int num);
int cmd_get_node_status();

#endif
