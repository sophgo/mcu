#include <mp5475.h>
#include <debug.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <timer.h>


int chip_deassert_n_on(void)
{
	sys_rst_disable();

	return 0;
}

int chip_assert_n_on(void)
{
	gpio_bit_set(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	gpio_bit_set(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);


	for (int i = 0; i < 90; i++)
		timer_udelay(1000);

	return 0;
}