/*
 * gpio.h: CV84X6EVB GPIO 信号控制
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <stdbool.h>
#include <gd32e50x_gpio.h>
#include <common/common.h>

struct gpio_node {
	uint32_t port;
	uint32_t pin;
	const char *name;
};

/* 输出控制引脚 */
extern struct gpio_node pcie_sel;
extern struct gpio_node en_12v0;
extern struct gpio_node poweren1;
extern struct gpio_node poweren2;
extern struct gpio_node poweren3;
extern struct gpio_node sys_rstn_h;
extern struct gpio_node pwr_rstn_h;
extern struct gpio_node pwr_button1_h;
extern struct gpio_node pwr_on_h;
extern struct gpio_node pwr_wakeup_h;
extern struct gpio_node pg_ind_led;

/* PG 检测输入引脚 */
extern struct gpio_node pg_vdd_core;
extern struct gpio_node ddr_vdd_pg;
extern struct gpio_node ddr_vddq_pg;
extern struct gpio_node pg_5v;

void gpio_set(struct gpio_node *node);
void gpio_clear(struct gpio_node *node);
bool gpio_get(struct gpio_node *node);
void pg_detect(void);
void gpio_control_init(void);

#endif /* __SIGNAL_H__ */