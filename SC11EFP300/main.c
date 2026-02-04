#define DEBUG
#include <gd32f4xx_gpio.h>
#include <system.h>
#include <pin.h>
#include <common.h>
#include <console.h>
#include <power.h>
#include <timer.h>
#include <debug.h>
#include <chip.h>
#include <tick.h>
#include <pca9848.h>
#include <ct7451.h>
#include <raa228234.h>
#include <mon.h>
#include <slave.h>
#include <aw95124.h>
#include <pcie.h>
#include <mcu.h>
#include <dvfs.h>

void HardFault_Handler(void)
{
	while (1)
		;
}
void NMI_Handler(void)
{
	while (1)
		;
}

int main()
{
	/* init mcu itself, inlcuding gpio,adc,iic and uart*/
	system_init();

	/*set card available singnal and set led on*/
	gpio_bit_set(CARD_PWR_PRES_PORT,CARD_PWR_PRES_PIN);
	led_init();

	dbg_printf("firmware build time:%s-%s\n", __DATE__, __TIME__);
	dbg_printf("SOPHONE SC11EFP300\n");

	// Begin
//	rcu_periph_clock_enable(RCU_GPIOA);
//	rcu_periph_clock_enable(RCU_GPIOD);
//	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_11);
//	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
//	gpio_bit_reset(GPIOA, GPIO_PIN_11);

//	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14);
//	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14);
//	gpio_bit_reset(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14);

//	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
//	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
//	gpio_bit_reset(GPIOD, GPIO_PIN_4);
	// END

	/*set board power*/
	board_power_init();

	/* pca9848 init */
	pca9848_init();

	/* CT7451 init */
	ct7451_init(0);
	ct7451_init(1);
	ct7451_init(2);
	ct7451_init(3);

	/* set process to collect basic info including temperature*/
	mon_init();
	dbg_printf("mon process init done\n");

	/* set console for debug */
	console_init();

	slave_init();
	pcie_init();
	dvfs_init();

	/* raa228234 init, instead of isl68224*/
	raa228234_init();


	check_gpio_power_good();
	check_pg_node();

	while(1) {
		if (chip_enable()) {
			mon_process();
		}

		ct7451_process();
		mdelay(1);
		mcu_process();
		console_poll();
//		dvfs_process();
	}

	return 0;
}
