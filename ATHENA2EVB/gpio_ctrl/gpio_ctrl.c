/*
 * gpio_ctrl.c: CV84 wishlist GPIO 名 -> ATHENA2 物理脚映射
 *
 * POWEREN1/2/3 在 A2 上对应 EN_VDD_0V8_RTC/EN_VDD_1V8_RTC/EN_VDDC_0V8
 * (CV84 为 PA10/PA9/PA8，A2 无 PA8 输出，POWEREN3 映射到 PE2)
 * SYS_RSTN_H 映射到 A2 的 SYS_RSTN_H_HASH (PD10)
 */
#include <gpio_ctrl/gpio_ctrl.h>
#include <pin.h>
#include <gd32e50x_gpio.h>
#include <common/common.h>
#include <strings.h>

struct gpio_test_node pcie_sel = {
	PCIE0_SEL_PORT, PCIE0_SEL_PIN, "PCIE_SEL"
};
struct gpio_test_node en_12v0 = {
	EN_12V0_PORT, EN_12V0_PIN, "EN_12V0"
};
struct gpio_test_node poweren1 = {
	EN_VDD_0V8_RTC_PORT, EN_VDD_0V8_RTC_PIN, "POWEREN1"
};
struct gpio_test_node poweren2 = {
	EN_VDD_1V8_RTC_PORT, EN_VDD_1V8_RTC_PIN, "POWEREN2"
};
struct gpio_test_node poweren3 = {
	EN_VDDC_0V8_PORT, EN_VDDC_0V8_PIN, "POWEREN3"
};
struct gpio_test_node sys_rstn_h = {
	SYS_RSTN_H_HASH_PORT, SYS_RSTN_H_HASH_PIN, "SYS_RSTN_H"
};
struct gpio_test_node pwr_rstn_h = {
	PWR_RSTN_H_PORT, PWR_RSTN_H_PIN, "PWR_RSTN_H"
};
struct gpio_test_node pwr_button1_h = {
	PWR_BUTTON1_H_PORT, PWR_BUTTON1_H_PIN, "PWR_BUTTON1_H"
};
struct gpio_test_node pwr_on_h = {
	PWR_ON_H_PORT, PWR_ON_H_PIN, "PWR_ON_H"
};
struct gpio_test_node pwr_wakeup_h = {
	PWR_WAKEUP_H_PORT, PWR_WAKEUP_H_PIN, "PWR_WAKEUP_H"
};

static struct gpio_test_node *all_nodes[] = {
	&pcie_sel, &en_12v0, &poweren1, &poweren2, &poweren3,
	&sys_rstn_h, &pwr_rstn_h, &pwr_button1_h, &pwr_on_h, &pwr_wakeup_h,
};

struct gpio_test_node *gpio_ctrl_find(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(all_nodes); ++i) {
		if (strcasecmp(all_nodes[i]->name, name) == 0)
			return all_nodes[i];
	}
	return NULL;
}

void gpio_ctrl_set(struct gpio_test_node *node)
{
	gpio_bit_set(node->port, node->pin);
}

void gpio_ctrl_clear(struct gpio_test_node *node)
{
	gpio_bit_reset(node->port, node->pin);
}

bool gpio_ctrl_read_output(struct gpio_test_node *node)
{
	return gpio_output_bit_get(node->port, node->pin) != RESET;
}

bool gpio_ctrl_read_pin(struct gpio_test_node *node)
{
	return gpio_input_bit_get(node->port, node->pin) != RESET;
}
