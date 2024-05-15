#include <chip.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <project.h>
#include <stdbool.h>
#include <tick.h>
#include <timer.h>

#define PRESS_VALUE	1
#define FORCE_POWEROFF_PRESS_TIME	2000
#define WARM_POWEROFF_PRESS_TIME	500
#define POWERON_PRESS_TIME	200
#define FORCE_REBOOT_PRESS_TIME	2000
#define WARM_REBOOT_PRESS_TIME	500

bool is_evb_power_key_on(void)
{
	if (gpio_get(PWR_OK_C_PORT, PWR_OK_C_PIN) == 0)
		return true;
	return false;
}

static bool is_power_button_pressed(void)
{
	if (gpio_get(POWER_KEY_PORT, POWER_KEY_PIN) == PRESS_VALUE)
		return true;
	return false;
}

static bool is_reset_button_pressed(void)
{
	if (gpio_get(RESET_KEY_PORT, RESET_KEY_PIN) == PRESS_VALUE)
		return true;
	return false;
}

void milkv_warm_poweroff(void)
{
	gpio_set(LINK_GPIO22_PORT, LINK_GPIO22_PIN);
	timer_mdelay(150);
	gpio_clear(LINK_GPIO22_PORT, LINK_GPIO22_PIN);
}

void milkv_warm_reboot(void)
{
	gpio_set(LINK_GPIO23_PORT, LINK_GPIO23_PIN);
	timer_mdelay(150);
	gpio_clear(LINK_GPIO23_PORT, LINK_GPIO23_PIN);
}

static void milkv_power_button_control(void)
{
	uint16_t press_time, curren_time, last_time;

	curren_time = last_time = tick_get();
	while (is_power_button_pressed()) {
		curren_time = tick_get();
		if(curren_time - last_time > FORCE_POWEROFF_PRESS_TIME)
			break;
	}
	press_time = curren_time - last_time;

	if (power_status() == true) {
		if (press_time > FORCE_POWEROFF_PRESS_TIME) {
			power_off();
			while (is_power_button_pressed())
				;
		} else if (press_time > WARM_POWEROFF_PRESS_TIME)
			milkv_warm_poweroff();
	} else {
		if (press_time > POWERON_PRESS_TIME)
			power_on();
	}
}

static void milkv_reset_button_control(void)
{
	uint16_t press_time, curren_time, last_time;

	curren_time = last_time = tick_get();
	while ((power_status() == true) && is_reset_button_pressed()) {
		curren_time = tick_get();
		if (curren_time - last_time > FORCE_REBOOT_PRESS_TIME)
			break;
	}
	press_time = curren_time - last_time;

	if (press_time > FORCE_REBOOT_PRESS_TIME) {
		power_off();
		timer_mdelay(50);
		power_on();
	} else if (press_time > WARM_REBOOT_PRESS_TIME)
		milkv_warm_reboot();
}

static void milkv_power_control(void)
{
	milkv_power_button_control();
	milkv_reset_button_control();
}

static void evb_power_control(void)
{
	if (is_evb_power_key_on() == true) {
		if (power_status() == false) {
			timer_udelay(1000);
			power_on();
		}
	}

	if (is_evb_power_key_on() == false) {
		if (power_status() == true)
			power_off();
	}
}

void milkv_auto_power_on(void)
{
	if (get_board_type() == MILKV_PIONEER) {
		/* Wait for 12V to be stable */
		timer_mdelay(1000);
		/* power on automatically after ATX powered */
		power_on();
	}
}

void board_power_control(void)
{
	switch (get_board_type()) {
	case MILKV_PIONEER:
		milkv_power_control();
		break;
	case SG2042EVB:
		evb_power_control();
		break;
	default:
		break;
	}
}

int milkv_atx_ctl_on(void)
{
	if (get_board_type() == MILKV_PIONEER)
		gpio_set(MCU_ATX_ON_PORT, MCU_ATX_ON_PIN);

	return 0;
}

void milkv_atx_ctl_off(void)
{
	if (get_board_type() == MILKV_PIONEER)
		gpio_clear(MCU_ATX_ON_PORT, MCU_ATX_ON_PIN);
}

int sys_rst_assert_on(void)
{
	chip_enable();
	return 0;
}

void sys_rst_assert_off(void)
{
}

int sys_rst_deassert_on(void)
{
	chip_disable();
	return 0;
}

void sys_rst_deassert_off(void)
{
	/* reset chip firstly when power off */
	chip_disable();
}