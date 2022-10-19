#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <chip.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <common.h>
#include <loop.h>
#include <power.h>
#include <stdbool.h>
#include <tca6416a.h>
#include <tmp451.h>
#include <wol.h>

static uint32_t uptime;
static uint32_t reset_times;
static int chip_enabled;
extern struct i2c_slave_ctx i2c1_slave_ctx;

uint32_t chip_reset_times(void)
{
	return reset_times;
}

uint32_t chip_uptime(void)
{
	return uptime;
}

void chip_disable(void)
{
	tca6416a_reset();
	gpio_clear(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	i2c_slave_reset(&i2c1_slave_ctx);
	tca6416a_error_led_on();
	gpio_set(SYS_RST_PORT, SYS_RST_PIN);
	chip_enabled = true;
}

int chip_is_enabled(void)
{
	return chip_enabled;
}

void chip_reset(void)
{
	chip_disable();
	mdelay(30);
	uptime = 0;
	++reset_times;
	chip_enable();
}

void chip_power_on_enable(void)
{
	int board, soc;

	wol_stop();

	/* wait temperature greater than 0 */
	tmp451_get_temp(&board, &soc);

	while (board < 0) {
		gpio_clear(THERMAL_OFF_PORT, THERMAL_OFF_PIN);
		mdelay(1000);
		tmp451_get_temp(&board, &soc);
	}

	power_on();
	chip_enable();
}

void chip_power_off_disable(void)
{
	chip_disable();
	power_off();
	gpio_set(THERMAL_OFF_PORT, THERMAL_OFF_PIN);
	/* for power down stable */
	mdelay(200);
	wol_start();
}

void chip_popd_reset(void)
{
	chip_power_off_disable();

	mdelay(2000);
	++reset_times;
	chip_power_on_enable();
}

static void chip_process(void)
{
	if (chip_enabled)
		++uptime;
	else
		uptime = 0;
}

void chip_init(void)
{
	tick_register_task(chip_process, 1000);
}

