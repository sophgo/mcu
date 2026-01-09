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

void atx_power_on()
{
	gpio_bit_set(ATX_CTRL_PORT, ATX_CTRL_PIN);
}

void atx_power_off()
{
	gpio_bit_reset(ATX_CTRL_PORT, ATX_CTRL_PIN);
}

bool is_clock_buffer_stable(void)
{
	if (gpio_get(CLK_LOSb_PORT,CLK_LOSb_PIN) == 0)
		return false;
	return true;
}

/*
static void evb_power_control(void)
{
	if(is_clock_buffer_stable()){
		if (power_status() == false) {
			timer_udelay(10000);
			power_on();
			ct7451_init();
		}
	}

	if (power_status() == true)
		power_off();
}

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
*/

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
