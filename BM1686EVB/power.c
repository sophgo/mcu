#include <libopencm3/stm32/gpio.h>
#include <libopencmsis/core_cm3.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <common.h>
#include <board_power.h>
#include <system.h>
/* in us */
#define NODE_CHECK_TIMEOUT	4000

static int node_check(struct power_node const *node)
{
	int err = 0;
	uint32_t port = node->param[0];
	uint16_t pin = node->param[1];

	timer_start(NODE_CHECK_TIMEOUT);

	while (gpio_get(port, pin) == 0) {
		if (timer_is_timeout()) {
			err = -1;
			break;
		}
	}
	timer_stop();

	return err;
}

static int node_on(struct power_node *node)
{
	int err = 0;

	debug("%s ", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		gpio_set(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_CHECK) {
		err = node_check(node);
	} else {
		power_on_func func = (power_on_func)node->param[0];
		if (func)
			err = func();
	}

	debug("[%c]\n", err ? 'X' : 'O');

	if (err) {
		node->status = POWER_STATUS_ERR;
	} else {
		node->status = POWER_STATUS_ON;
		if (node->delay)
			timer_udelay(node->delay);
	}

	return err;
}

static void node_off(struct power_node *node)
{
	debug("%s\n", node->name);

	if (!strcmp(node->name, "VDD-1.8V")) {
		printf("not cler %s\n", node->name);
		return;
	}

	if (!strcmp(node->name, "EN_12V_SYS")) {
		printf("not cler %s\n", node->name);
		return;
	}


	/* skip check nodes */
	if (node->type == NODE_TYPE_ENABLE) {
		gpio_clear(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_FUNCTION) {
		power_off_func func = (power_off_func)node->param[1];
		if (func)
			func();
	}
	node->status = POWER_STATUS_OFF;
}

int node_seqon(struct power_node *node, unsigned int num)
{
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i);
		if (err)
			break;
	}

	/* donot power off */
#if 0
	if (err) {
		--i;
		while (i >= 0) {
			node_off(node + i);
			--i;
		}
	}
#endif
	return err;
}

void node_seqoff(struct power_node *node, unsigned int num)
{
	int i;

	for (i = num - 1; i >= 0; --i)
		node_off(node + i);
}

int power_on(void)
{
	int err = node_seqon(board_power_nodes, ARRAY_SIZE(board_power_nodes));

	if (err) {
		led_set_frequency(5);
	} else {
		led_set_frequency(1);
	}

	return err;
}

void power_off(void)
{
	node_seqoff(board_power_nodes, ARRAY_SIZE(board_power_nodes));
	led_set_frequency(LED_FREQ_ALWAYS_OFF);
}

void power_init(void)
{
	led_set_frequency(LED_FREQ_ALWAYS_OFF);
}

int power_node_on(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(board_power_nodes); i++) {
		if (!strcmp((board_power_nodes + i)->name, name))
			return node_on(board_power_nodes + i);
	}

	return -1;
}

int power_nodes_on(const char **name, int num)
{
	int i;

	for (i = 0; i < num; i++) {
		if (power_node_on(name[i]))
			return -1;
	}

	return 0;
}

void power_node_off(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(board_power_nodes); i++) {
		if (!strcmp((board_power_nodes + i)->name, name))
			return node_off(board_power_nodes + i);
	}
}

void power_nodes_off(const char **name, int num)
{
	int i;

	for (i = 0; i < num; i++)
		power_node_off(name[i]);
}

int power_node_status(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(board_power_nodes); i++) {
		if (!strcmp((board_power_nodes + i)->name, name))
			return (board_power_nodes + i)->status;
	}

	return -1;
}

int power_nodes_status(const char **name, int num)
{
	int i;

	for (i = 0; i < num; i++) {
		if (power_node_status(name[i]) != POWER_STATUS_ON)
			return POWER_STATUS_OFF;
	}

	return POWER_STATUS_ON;
}

int cmd_get_node_status()
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(board_power_nodes); i++){
		uart_puts((board_power_nodes + i)->name);
		if ((board_power_nodes+i)->status == POWER_STATUS_ON)
			uart_puts(" [O]\n");
		else
			uart_puts(" [X]\n");
	}

	return 0;
}
