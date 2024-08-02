#include <chip.h>
#include <common.h>
#include <pin.h>
#include <power.h>
#include <project.h>
#include <stdbool.h>
#include <tick.h>
#include <timer.h>

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

int check_pcie_reset_on(void)
{
	board_init();
	return 0;
}

void check_pcie_reset_off(void)
{
}