#include <chip.h>
#include <pin.h>
#include <tick.h>
#include <debug.h>
#include <common.h>
#include <power.h>
#include <system.h>
#include <stdio.h>

static uint32_t uptime;
static uint32_t reset_times;
static int chip_enabled;
static int needpoweron;

uint32_t chip_reset_times(void)
{
	return reset_times;
}

uint32_t chip_uptime(void)
{
	return uptime;
}

void preset_reset(void)
{
	gpio_bit_reset(PCIE4_L0_PRESET_PORT, PCIE4_L0_PRESET_PIN);
	gpio_bit_reset(PCIE4_L1_PRESET_PORT, PCIE4_L1_PRESET_PIN);
	gpio_bit_reset(PCIE0_L0_PRESET_PORT, PCIE0_L0_PRESET_PIN);
	gpio_bit_reset(PCIE0_L1_PRESET_PORT, PCIE0_L1_PRESET_PIN);
	gpio_bit_reset(PCIE2_L0_PRESET_PORT, PCIE2_L0_PRESET_PIN);
	gpio_bit_reset(PCIE2_L1_PRESET_PORT, PCIE2_L1_PRESET_PIN);
	gpio_bit_reset(PCIE1_L0_PRESET_PORT, PCIE1_L0_PRESET_PIN);
	gpio_bit_reset(PCIE1_L1_PRESET_PORT, PCIE1_L1_PRESET_PIN);
	gpio_bit_reset(PCIE3_L0_PRESET_PORT, PCIE3_L0_PRESET_PIN);
	gpio_bit_reset(PCIE3_L1_PRESET_PORT, PCIE3_L1_PRESET_PIN);
}

void preset_set(void)
{
	gpio_bit_set(PCIE4_L0_PRESET_PORT, PCIE4_L0_PRESET_PIN);
	gpio_bit_set(PCIE4_L1_PRESET_PORT, PCIE4_L1_PRESET_PIN);
	gpio_bit_set(PCIE0_L0_PRESET_PORT, PCIE0_L0_PRESET_PIN);
	gpio_bit_set(PCIE0_L1_PRESET_PORT, PCIE0_L1_PRESET_PIN);
	gpio_bit_set(PCIE2_L0_PRESET_PORT, PCIE2_L0_PRESET_PIN);
	gpio_bit_set(PCIE2_L1_PRESET_PORT, PCIE2_L1_PRESET_PIN);
	gpio_bit_set(PCIE1_L0_PRESET_PORT, PCIE1_L0_PRESET_PIN);
	gpio_bit_set(PCIE1_L1_PRESET_PORT, PCIE1_L1_PRESET_PIN);
	gpio_bit_set(PCIE3_L0_PRESET_PORT, PCIE3_L0_PRESET_PIN);
	gpio_bit_set(PCIE3_L1_PRESET_PORT, PCIE3_L1_PRESET_PIN);
}

void chip_disable(void)
{
	gpio_clear(SYS_RST_X_H_PORT, SYS_RST_X_H_PIN);
	preset_reset();
	chip_enabled = false;
	uptime = 0;
}

void chip_enable(void)
{
	gpio_set(SYS_RST_X_H_PORT, SYS_RST_X_H_PIN);
	preset_set();
	chip_enabled = true;
}

void chip_reset(void)
{
	chip_disable();
	mdelay(30);
	uptime = 0;
	++reset_times;
	chip_enable();
}

void chip_popd_reset_early(void)
{
	power_off();
	chip_disable();
	mdelay(50);
}

void chip_popd_reset_end(void)
{
	power_on();
	uptime = 0;
	++reset_times;

	chip_enable();
}

static void chip_process(void)
{
	if (chip_enabled)
		++uptime;
}

void chip_init(void)
{
	tick_register_task(chip_process, 1000);
}

int chip_is_enabled(void)
{
	return chip_enabled;
}

void set_needpoweron(void)
{
	needpoweron = 1;
}

void clr_needpoweron(void)
{
	needpoweron = 0;
}

int get_needpoweron_satus(void)
{
	return needpoweron;
}
