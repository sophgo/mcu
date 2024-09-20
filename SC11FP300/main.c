#include <gd32e50x_gpio.h>
#include <gd32e50x_misc.h>
#include <tick.h>
#include <system.h>
#include <i2c_slave.h>
#include <debug.h>
#include <timer.h>
#include <power.h>
#include <pin.h>
#include <common.h>
#include <chip.h>
#include <mon.h>
#include <upgrade.h>
#include <project.h>
#include <pca9848.h>
#include <mp5475.h>
#include <console.h>
#include <stdio.h>
#include <slave.h>
#include <pcie.h>
#include <ct7451.h>
#include <dbgi2c.h>
#include <isl68224.h>
#include <freq.h>
#include <mcu.h>

int main(void)
{
	system_init();
	gpio_bit_reset(EN_VDD_3V3_PORT, EN_VDD_3V3_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);

	led_init();

	debug("\nBITMAIN SOPHONE SC11FP300\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("BITMAIN SOPHONE SC11FP300\n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif
	pca9848_init();
	ct7451_init(0);
	ct7451_init(1);
	board_power_init();
	//chip_init();
	mon_init();
	slave_init();
	console_init();
	pcie_init();
	timer1_init();

	while (1) {
		//chip_update();
		if (chip_enable()) {
			// sg2044_clk_pll_set_rate(0, 4, 1100 * MHZ, 25 * MHZ); //mpll4
			// sg2044_clk_pll_set_rate(0, 5, 901 * MHZ, 25 * MHZ); //mpll5
			mon_process();
		}

		ct7451_process();
		console_poll();
		mcu_process();
	}

	return 0;
}
