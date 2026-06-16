/*
 * gpio_ctrl.h: CV84 兼容 GPIO 测试控制 (pintest / powerseq)
 *
 * 引脚名与 CV84 wishlist 一致；在 ATHENA2 上映射到对应物理脚。
 */
#ifndef __GPIO_CTRL_H__
#define __GPIO_CTRL_H__

#include <stdint.h>
#include <stdbool.h>

struct gpio_test_node {
	uint32_t port;
	uint32_t pin;
	const char *name;
};

extern struct gpio_test_node pcie_sel;
extern struct gpio_test_node en_12v0;
extern struct gpio_test_node poweren1;
extern struct gpio_test_node poweren2;
extern struct gpio_test_node poweren3;
extern struct gpio_test_node sys_rstn_h;
extern struct gpio_test_node pwr_rstn_h;
extern struct gpio_test_node pwr_button1_h;
extern struct gpio_test_node pwr_on_h;
extern struct gpio_test_node pwr_wakeup_h;

struct gpio_test_node *gpio_ctrl_find(const char *name);
void gpio_ctrl_set(struct gpio_test_node *node);
void gpio_ctrl_clear(struct gpio_test_node *node);
bool gpio_ctrl_read_output(struct gpio_test_node *node);
bool gpio_ctrl_read_pin(struct gpio_test_node *node);

#endif /* __GPIO_CTRL_H__ */
