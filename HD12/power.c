#include "pin.h"
#include <gd32f4xx_gpio.h>
#include <timer.h>
#include <power.h>
#include <debug.h>
#include <common.h>
#include <upgrade.h>
#include <board_power.h>
#include <system.h>
#include <chip.h>


/* in us */
#define NODE_CHECK_TIMEOUT	(10 * 1000)

static int power_is_on;

void check_gpio_power_good(void)
{
	int i;

	struct node_power_good pg_nodes[] = {
		{.port = (unsigned long)PG_DDR_VDD_BM0_PORT, .pin = (unsigned long)PG_DDR_VDD_BM0_PIN, .name = "PG_DDR_VDD_BM0"},
		{.port = (unsigned long)PG_VDDC_BM0_PORT, .pin = (unsigned long)PG_VDDC_BM0_PIN, .name = "PG_VDDC_BM0"},
		{.port = (unsigned long)PG_VDDR_BM0_PORT, .pin = (unsigned long)PG_VDDR_BM0_PIN, .name = "PG_VDDR_BM0"},
		{.port = (unsigned long)PG_DDR_VDD2H_L_BM0_PORT, .pin = (unsigned long)PG_DDR_VDD2H_L_BM0_PIN, .name = "PG_DDR_VDD2H_L_BM0"},
		{.port = (unsigned long)PG_DDR_VDD2H_R_BM0_PORT, .pin = (unsigned long)PG_DDR_VDD2H_R_BM0_PIN, .name = "PG_DDR_VDD2H_R_BM0"},
		{.port = (unsigned long)PG_DDR_VDDQ_L_BM0_PORT, .pin = (unsigned long)PG_DDR_VDDQ_L_BM0_PIN, .name = "PG_DDR_VDDQ_L_BM0"},
		{.port = (unsigned long)PG_DDR_VDDQ_R_BM0_PORT, .pin = (unsigned long)PG_DDR_VDDQ_R_BM0_PIN, .name = "PG_DDR_VDDQ_R_BM0"},
		{.port = (unsigned long)PG_VP_PCIE_BM0_PORT, .pin = (unsigned long)PG_VP_PCIE_BM0_PIN, .name = "PG_VP_PCIE_BM0"},

		{.port = (unsigned long)PG_DDR_VDD_BM1_PORT, .pin = (unsigned long)PG_DDR_VDD_BM1_PIN, .name = "PG_DDR_VDD_BM1"},
		{.port = (unsigned long)PG_VDDC_BM1_PORT, .pin = (unsigned long)PG_VDDC_BM1_PIN, .name = "PG_VDDC_BM1"},
		{.port = (unsigned long)PG_VDDR_BM1_PORT, .pin = (unsigned long)PG_VDDR_BM1_PIN, .name = "PG_VDDR_BM1"},
		{.port = (unsigned long)PG_DDR_VDD2H_L_BM1_PORT, .pin = (unsigned long)PG_DDR_VDD2H_L_BM1_PIN, .name = "PG_DDR_VDD2H_L_BM1"},
		{.port = (unsigned long)PG_DDR_VDD2H_R_BM1_PORT, .pin = (unsigned long)PG_DDR_VDD2H_R_BM1_PIN, .name = "PG_DDR_VDD2H_R_BM1"},
		{.port = (unsigned long)PG_DDR_VDDQ_L_BM1_PORT, .pin = (unsigned long)PG_DDR_VDDQ_L_BM1_PIN, .name = "PG_DDR_VDDQ_L_BM1"},
		{.port = (unsigned long)PG_DDR_VDDQ_R_BM1_PORT, .pin = (unsigned long)PG_DDR_VDDQ_R_BM1_PIN, .name = "PG_DDR_VDDQ_R_BM1"},
		{.port = (unsigned long)PG_VP_PCIE_BM1_PORT, .pin = (unsigned long)PG_VP_PCIE_BM1_PIN, .name = "PG_VP_PCIE_BM1"},

		{.port = (unsigned long)MODULE_PG_PORT, .pin = (unsigned long)MODULE_PG_PIN, .name = "PG_MODULE"},
	};

	for(i = 0;i < ARRAY_SIZE(pg_nodes); i++){
		int good = gpio_input_bit_get(pg_nodes->port, pg_nodes->pin);
		dbg_printf("%s : %s (raw=%d)\n", pg_nodes[i].name, good?"Good":"Bad",good);
	}

}

void check_clk_buffer(void)
{
	if (gpio_input_bit_get(CLK_LOSb_PORT, CLK_LOSb_PIN))
		dbg_printf("Clock buffer is OK\n");
	else
		dbg_printf("Clock buffer error!\n");
}

void check_host_pwrgd(void)
{
	if (gpio_input_bit_get(HOST_PWRGD_PORT, HOST_PWRGD_PIN))
		dbg_printf("Host power good is OK\n");
	else
		dbg_printf("Host power good error!\n");
}

static int node_check(struct power_node const *node)
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

static int node_on(struct power_node const *node)
{
	int err = 0;

//	debug("%s ", node->name);

	if (node->type == NODE_TYPE_ENABLE) {
		gpio_set(node->param[0], node->param[1]);
	} else if (node->type == NODE_TYPE_CHECK) {
		err = node_check(node);
	} else {
		power_on_func func = (power_on_func)node->param[0];
		if (func)
			err = func();
	}

//	debug("[%c]\n", err ? 'X' : 'O');

	if (err == 0 && node->delay)
		timer_udelay(node->delay);

	return err;
}

static void node_off(struct power_node const *node)
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
		led_set_frequency(500);
	} else {
		if (get_stage() == RUN_STAGE_LOADER)
			//led_set_frequency(3);
			led_set_frequency(1);
		else
			led_set_frequency(1);
		power_is_on = true;
	}

	return err;
}

void module_off(void)
{
	gpio_bit_reset(MODULE_EN_PORT, MODULE_EN_PIN);
}

void power_off(void)
{
	power_is_on = false;
	sys_rst_disable();
	node_seqoff(board_power_nodes, ARRAY_SIZE(board_power_nodes));
	module_off();
	led_set_frequency(LED_FREQ_ALWAYS_OFF);
}

void power_init(void)
{
	led_set_frequency(LED_FREQ_ALWAYS_OFF);
}

int power_status(void)
{
	return power_is_on;
}


int board_power_init(void)
{
	return power_on();
}
