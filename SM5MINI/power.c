#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <common.h>
#include <board_power.h>

/* in us */
#define NODE_CHECK_TIMEOUT	1000

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

static int node_on(struct power_node const *node)
{
	int err = 0;

	debug("power on %s\r\n", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		gpio_set(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_CHECK) {
		err = node_check(node);
	} else {
		power_on_func func = (power_on_func)node->param[0];
		if (func)
			err = func();
	}

	if (err == 0 && node->delay)
		timer_udelay(node->delay);

	return err;
}

static void node_off(struct power_node const *node)
{
	debug("power off %s\r\n", node->name);

	/* skip check nodes */
	if (node->type == NODE_TYPE_ENABLE) {
		gpio_clear(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_FUNCTION) {
		power_off_func func = (power_off_func)node->param[1];
		if (func)
			func();
	}
}

int node_seqon(struct power_node const *node, unsigned int num)
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

void node_seqoff(struct power_node const *node, unsigned int num)
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

