/*
 * gpio.c: CV84X6EVB GPIO 信号控制 + PG 检测
 *
 * PG 检测: 当 PE2, PE3, PE4, PE5 全部为高电平时，点亮 PC9 LED
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#include <gpio/gpio.h>
#include <pin.h>
#include <loop/loop.h>
#include <debug.h>

/* ================================================================
 * GPIO 节点定义
 * ================================================================ */

/* 输出控制引脚 */
struct gpio_node pcie_sel = {
	PCIE_SEL_PORT, PCIE_SEL_PIN, "PCIE_SEL"
};
struct gpio_node en_12v0 = {
	EN_12V0_PORT, EN_12V0_PIN, "EN_12V0"
};
struct gpio_node poweren1 = {
	POWEREN1_PORT, POWEREN1_PIN, "POWEREN1"
};
struct gpio_node poweren2 = {
	POWEREN2_PORT, POWEREN2_PIN, "POWEREN2"
};
struct gpio_node poweren3 = {
	POWEREN3_PORT, POWEREN3_PIN, "POWEREN3"
};
struct gpio_node sys_rstn_h = {
	SYS_RSTN_H_PORT, SYS_RSTN_H_PIN, "SYS_RSTN_H"
};
struct gpio_node pwr_rstn_h = {
	PWR_RSTN_H_PORT, PWR_RSTN_H_PIN, "PWR_RSTN_H"
};
struct gpio_node pwr_button1_h = {
	PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN, "PWR_BUTTON1_H"
};
struct gpio_node pwr_on_h = {
	PWR_ON_H_PORT, PWR_ON_H_PIN, "PWR_ON_H"
};
struct gpio_node pwr_wakeup_h = {
	PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN, "PWR_WAKEUP_H"
};
struct gpio_node pg_ind_led = {
	PG_IND_PORT, PG_IND_PIN, "PG_IND"
};

/* PG 检测输入引脚 */
struct gpio_node pg_vdd_core = {
	PG_VDD_CORE_PORT, PG_VDD_CORE_PIN, "PG_VDD_CORE"
};
struct gpio_node ddr_vdd_pg = {
	DDR_VDD_PG_PORT, DDR_VDD_PG_PIN, "DDR_VDD_PG"
};
struct gpio_node ddr_vddq_pg = {
	DDR_VDDQ_PG_PORT, DDR_VDDQ_PG_PIN, "DDR_VDDQ_PG"
};
struct gpio_node pg_5v = {
	PG_5V_PORT, PG_5V_PIN, "PG_5V"
};

/* ================================================================
 * 基础操作
 * ================================================================ */
void gpio_set(struct gpio_node *node)
{
	gpio_bit_set(node->port, node->pin);
}

void gpio_clear(struct gpio_node *node)
{
	gpio_bit_reset(node->port, node->pin);
}

bool gpio_get(struct gpio_node *node)
{
	return gpio_input_bit_get(node->port, node->pin) != 0;
}

/* ================================================================
 * Power Good 检测
 * 当 PE2, PE3, PE4, PE5 全部高电平时，点亮 PC9 LED
 * 否则熄灭 PC9 LED
 * ================================================================ */
static void pg_detect_process(void)
{
	static int last_pg_status;
	int all_good;

	all_good = gpio_get(&pg_vdd_core) &&
		   gpio_get(&ddr_vdd_pg) &&
		   gpio_get(&ddr_vddq_pg) &&
		   gpio_get(&pg_5v);

	if (all_good != last_pg_status) {
		last_pg_status = all_good;
		if (all_good) {
			gpio_set(&pg_ind_led);
			debug("PG: all good, LED ON\n");
		} else {
			gpio_clear(&pg_ind_led);
			debug("PG: not all good, LED OFF\n");
		}
	}
}

void gpio_control_init(void)
{
	loop_add(pg_detect_process);
}