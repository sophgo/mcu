#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <i2c_master.h>
#include <debug.h>
#include <pin.h>
#include <timer.h>
#include <power.h>
#include <common.h>

static int node_on(struct power_node const *node)
{
	int i, err = 0;

	debug("power on %s\r\n", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		for (i = 0; i < ARRAY_SIZE(node->param); ++i) {
			gpio_set(node->param[i][0], node->param[i][1]);
			timer_udelay(node->group_delay);
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(node->param); ++i) {
			power_on_func func = (power_on_func)node->param[i][0];
			if (func) {
				err = func();
				if (err)
					break;
			}
			timer_udelay(node->group_delay);
		}
	}

	if (err == 0 && node->node_delay)
		timer_udelay(node->node_delay);

	return err;
}

static void node_off(struct power_node const *node)
{
	int i;

	debug("power off %s\r\n", node->name);

	if (node->type == NODE_TYPE_ENABLE)
		for (i = 0; i < ARRAY_SIZE(node->param); ++i)
			gpio_clear(node->param[i][0], node->param[i][1]);
	else
		for (i = 0; i < ARRAY_SIZE(node->param); ++i)
			((power_off_func)(node->param[i][1]))();
}

int power_on(struct power_node const *node, unsigned int num)
{
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i);
		if (err)
			break;
	}
	if (i != num) {
		--i;
		while (i >= 0) {
			node_off(node + i);
			--i;
		}
	}
	return err;
}

void power_off(struct power_node const *node, unsigned int num)
{
	int i;

	for (i = num; i >= 0; --i)
		node_off(node + i);
}
