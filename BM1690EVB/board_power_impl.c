#include <chip.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <project.h>
#include <stdbool.h>
#include <tick.h>
#include <timer.h>
#include <i2c01_slave.h>
#include <i2c_slave.h>
#include <i2c-slaves/ct7451.h>

#define PRESS_VALUE	1
#define FORCE_POWEROFF_PRESS_TIME	2000
#define WARM_POWEROFF_PRESS_TIME	500
#define POWERON_PRESS_TIME	200
#define FORCE_REBOOT_PRESS_TIME	2000
#define WARM_REBOOT_PRESS_TIME	500

extern struct i2c01_slave_ctx i2c0_slave_ctx;

bool is_evb_power_key_on(void)
{
	if (gpio_get(MCU_PS_ON_PORT, MCU_PS_ON_PIN) == 1)
		return true;
	return false;
}

static void evb_power_control(void)
{
	if (is_evb_power_key_on() == true) {
		if (power_status() == false) {
			while(gpio_get(PWR_OK_C_PORT, PWR_OK_C_PIN) == 1){
				timer_udelay(1000);
			}
			timer_udelay(10000);
			power_on();
			ct7451_init(&i2c0_slave_ctx);
		}
	}

	if (is_evb_power_key_on() == false) {
		if (power_status() == true)
			power_off();
	}
}

// void milkv_auto_power_on(void)
// {
// 	if (get_board_type() == MILKV_PIONEER) {
// 		/* Wait for 12V to be stable */
// 		timer_mdelay(1000);
// 		/* power on automatically after ATX powered */
// 		power_on();
// 	}
// }

void board_power_control(void)
{
	switch (get_board_type()) {
	case BM1690EVB:
		evb_power_control();
		break;
	default:
		break;
	}
}

// int milkv_atx_ctl_on(void)
// {
// 	if (get_board_type() == MILKV_PIONEER)
// 		gpio_set(MCU_ATX_ON_PORT, MCU_ATX_ON_PIN);

// 	return 0;
// }

// void milkv_atx_ctl_off(void)
// {
// 	if (get_board_type() == MILKV_PIONEER)
// 		gpio_clear(MCU_ATX_ON_PORT, MCU_ATX_ON_PIN);
// }

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