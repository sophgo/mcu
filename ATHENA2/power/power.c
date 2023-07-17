/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <gd32e50x_gpio.h>
#include <timer.h>
#include <power.h>
#include <debug.h>
#include <common.h>
#include <upgrade.h>
#include <power_node.h>
#include <system/system.h>
#include <stdio.h>
#include <stdbool.h>
#include <timer.h>
#include <chip.h>
#include <slt.h>
#include <loop.h>
#include <signal.h>
#include <se9.h>
#include <led.h>
#include <project.h>
#include <include.h>
#include <include.h>


/* in us */
#define NODE_CHECK_TIMEOUT	(10 * 1000)


int power_is_on;


static int node_check(struct board_power_nodes const *node)
{
	return 0;

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

static int node_on(struct board_power_nodes const *node, uint8_t board_type)
{
	__disable_irq();

	int err = 0;

	for (int i = 0; i < node->type_sum; i++) {
		debug("          [%d] ", node->type_sum);
		if (node->board[i] == board_type) {
			debug("%-20s ", node->name);

			if (node->type == NODE_TYPE_ENABLE) {
				gpio_set(node->param[0], node->param[1]);
			} else if (node->type == NODE_TYPE_CHECK) {
				err = node_check(node);
			} else {
				power_on_func func = (power_on_func)node->param[0];

				if (func)
					err = func();
			}

			debug("[%c]", err ? 'X' : 'O');

			if (err == 0 && node->delay) {
				if (node->delay > 65000) {
					timer_delay_ms(node->delay / 1000);
					timer_delay_us(node->delay % 1000);
				} else {
					timer_delay_us(node->delay);
				}
			}
			debug("[Delay: %d]\n", node->delay);
		}
		else {
			debug("[%d]\n", node->board[i]);
		}
	}
	__enable_irq();

	return err;
}

static void node_off(struct board_power_nodes const *node)
{
	debug("%s\n", node->name);

	/* skip check nodes */
	if (node->type == NODE_TYPE_ENABLE) {
		gpio_clear(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_FUNCTION) {
		power_off_func func = (power_off_func)node->param[1];

		if (func)
			func();
	}
}

int node_seq_on(struct board_power_nodes const *node, unsigned int num)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	debug("\n");
	int err, i;

	err = 0;

	for (i = 0; i < num; ++i) {
		err = node_on(node + i, get_board_type());
		if (err)
			break;
	}

	/* do not power off */
#if 0
	if (err) {
		--i;
		while (i >= 0) {
			node_off(node + i);
			--i;
		}
	}
#endif
	debug("\n");
	return err;
}

void node_seq_off(struct board_power_nodes const *node, unsigned int num)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	debug("\n");
	int i;

	for (i = num - 1; i >= 0; --i)
		node_off(node + i);
	debug("\n");
}

void power_on(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	int err = node_seq_on(board_power_node, ARRAY_SIZE(board_power_node));

	if (err) {
		debug("power on failed node: %d\n", err);
	} else {
		if (get_board_type() == ATHENA2EVB) {

		} else if (get_board_type() == ATHENA2ACP) {
			//led_set_frequency(1);
			led_control(power_led, 					true);
			led_control(mcu_ssd_status_led, 		true);
			led_control(mcu_status_led,				true);
			led_control(mcu_ssd_alarm_led, 			false);
			led_control(mcu_ssd_alarm_led,			false);

			timer_delay_ms(20);

			se9_output_signal(lte_5g_reset,			false);
		}
		chip_enable();
		power_is_on = true;
	}
}

void power_off(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	slt_reset();
	chip_disable();
	power_is_on = false;

	node_seq_off(board_power_node, ARRAY_SIZE(board_power_node));

	if (get_board_type() == ATHENA2EVB) {

	} else if (get_board_type() == ATHENA2ACP) {
		led_control(power_led, 						false);
		led_control(mcu_ssd_status_led, 			false);
		led_control(mcu_status_led,					false);
		led_control(mcu_ssd_alarm_led, 				true);
		led_control(mcu_ssd_alarm_led,				true);

		se9_output_signal(lte_5g_reset,				true);
	}
}

void board_power_control(void)
{
	/* Main loop query */
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	switch (get_board_type()) {
		case ATHENA2EVB:
			break;
		case ATHENA2ACP:
			acp_power_control();
			break;
		default:
			break;
	}
}

void power_init(void)
{
//	debug("[%3lu] %-25s :  %-20s :  %4d\n", chip_uptime(), __FUNCTION__, __FILE__, __LINE__);
	power_is_on = false;
	loop_add(board_power_control);
}

int power_status(void)
{
	return power_is_on;
}
