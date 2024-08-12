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
//#include <ct7451.h>

int main(void)
{
	system_init();
	gpio_bit_reset(EN_VDD_3V3_PORT, EN_VDD_3V3_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	// while(1) {
	// 	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	// 	timer_udelay(1);
	// 	gpio_bit_set(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	// 	timer_udelay(1);
	// }

	led_init();
	//led_set_frequency(1);
	// debug("\nBITMAIN SOPHONE SC11FP300\n");
	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("bitmain\n");

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER)
		if (check_app() == 0)
			app_start();
#else
// #warn stand alone environment
#endif
	//pca9848_init();
	// ct7451_init(0);
	// ct7451_init(1);
	//mp5475_init();
	board_power_init();
#if 0
	timer_mdelay(30);
	sys_rst_enable();
#endif
	//chip_init();
	// mon_init();
	// slave_init();
	// console_init();

	pcie_init();
	while (1) {
		//chip_update();
		// if (chip_enable())
			//mon_process();
		//ct7451_process();
		console_poll();
	}

	return 0;
}
