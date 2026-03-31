#include <gd32f4xx_gpio.h>
#include <stdint.h>
#include <project.h>
#include <common.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <system.h>

#define DDR_TYPE	DDR_TYPE_LPDDR4X

static uint8_t location;

void led_on(void)
{
	gpio_bit_set(STATUS_LED0_PORT, STATUS_LED0_PIN);
}

void led_off(void)
{
	gpio_bit_reset(STATUS_LED0_PORT, STATUS_LED0_PIN);
}

int led_tick_handle;
int led_status;

void led_isr(void);

void led_init(void)
{
	/* donnot start now */
	led_tick_handle = tick_register_task(led_isr, 0);
}

/* if freq > 500, turn on led
 * if freq == 0, turn off led
 */
void led_set_frequency(unsigned long freq)
{
	if (freq > 500) {
		/* turn off tick task */
		tick_set_task_interval(led_tick_handle, 0);
		led_on();
	} else if (freq == 0) {
		tick_set_task_interval(led_tick_handle, 0);
		led_off();
	} else {
		tick_set_task_interval(led_tick_handle, 1000 / freq / 2);
	}
}

void led_isr(void)
{
	led_status = !led_status;
	if (led_status)
		led_on();
	else
		led_off();
}

uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

void set_module_id(void)
{
	int i;
	uint32_t pins[4] = {MODULE_ID_0_PIN, MODULE_ID_1_PIN, MODULE_ID_2_PIN, MODULE_ID_3_PIN};

	for (i = 0; i < 4; i++) {
		// 检测module id的pin脚状态， 高电平表示为模组在当前id位置，低电平表示模组不在当前id位置
		if (gpio_input_bit_get(GPIOC, pins[i]) == 1) {
			location = i + 1; // 模组位置从1开始编号
			return;
		}
	}

	/* 如果所有pin脚都是低电平，表示模组位置为0 */
	location = 0;
	// dbg_printf("warning: module id is 0, please check the module id pins\n");
}

uint8_t get_module_id(void)
{
	return location;
}
