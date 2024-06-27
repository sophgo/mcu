/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include <system.h>
#include <i2c_master.h>
#include <debug.h>
#include <chip.h>
#include <mp5475.h>
#include <pin.h>
#include <timer.h>
#include <mon.h>
#include <common.h>
#include <upgrade.h>
#include <mcu.h>
#include <pic.h>
#include <tca6416a.h>
#include <project.h>
#include <power.h>
#include <se5.h>
#include <sm7.h>
#include <loop.h>
#include <keyboard.h>
#include <eeprom.h>
#include <wdt.h>
#include <pcie.h>
#include <slt.h>
#include <ct7451.h>
#include <rst_key.h>
#include <mon_print.h>
#include <at24c128c-e2prom.h>
#include <se6.h>
#include <mcu-e2prom.h>

static struct i2c_slave_ctx i2c1_slave_ctx;
static struct i2c_slave_ctx i2c2_slave_ctx;

int main(void)
{
	clock_init();
	system_init();

	//rst_key_init();
	timer_mdelay(100);

	debug("\nSOPHONE SM7_HK\n");
	debug("firmware build time:%s-%s\n", __DATE__, __TIME__);

#ifndef STANDALONE
	if (get_stage() == RUN_STAGE_LOADER && check_app() == 0)
		app_start();
#endif

	led_init();
	led_set_frequency(1000);
	i2c_master_init(I2C1);
	i2c2_master_init(I2C2);
	//i2c_master_init(I2C3);
	mp5475_init();
	mp5475_buck_on(0);
	//timer_mdelay(200);
	
	led_set_frequency(10);

	/* check if i am in test board and if we need enter test mode */
	if (detect_test_mode() == TEST_MODE_HALT) {

		mcu_set_test_mode(true);
		led_set_frequency(10);
		/* convert MCU_INT from input to output */
		gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
		gpio_set_output_options(MCU_INT_PORT, GPIO_OTYPE_PP,
					GPIO_OSPEED_VERYHIGH, MCU_INT_PIN);
		gpio_mode_setup(MCU_INT_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
				MCU_INT_PIN);

		set_board_type(SM7M);

		i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2_BASE,
			       I2C2_OA1, I2C2_OA2, I2C2_OA2_MASK);
		mcu_test_init(&i2c2_slave_ctx);
		nvic_enable_irq(NVIC_I2C2_IRQ);
		i2c_slave_start(&i2c2_slave_ctx);

		while (detect_test_mode() != TEST_MODE_RUN) {
			mcu_process();
			if (!mcu_get_test_mode())
				break;
		}

		set_board_type(SM7_HK);
		nvic_disable_irq(NVIC_I2C2_IRQ);
		i2c_slave_stop(&i2c2_slave_ctx);
	}
	/* reset MCU_INT */
	gpio_clear(MCU_INT_PORT, MCU_INT_PIN);
	gpio_mode_setup(MCU_INT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
			MCU_INT_PIN);

	tca6416a_probe();
	pic_probe();
	timer_udelay(50*1000);
	power_init();
	timer_udelay(1000);
	mp5475_init();
	power_on();
	chip_init();

	debug("%s %s working at ",
	      get_board_type_name(),
	      get_stage() == RUN_STAGE_LOADER ? "loader" : "application");
	if (get_work_mode() == WORK_MODE_SOC)
		debug("soc mode\n");
	else if (get_work_mode() == WORK_MODE_PCIE)
		debug("pcie mode\n");
	else if (get_work_mode() == WORK_MODE_MIXED)
		debug("mix mode\n");
	else
		debug("unkown mode\n");

	nvic_enable_irq(NVIC_I2C1_IRQ);
	i2c_slave_init(&i2c1_slave_ctx, (void *)I2C1_BASE,
		       I2C1_OA1, I2C1_OA2, I2C1_OA2_MASK);

	mon_init();
	mon_print_init();

	/* check if i am se8 crtl board */
	if (is_se6ctrl_board()){
		set_board_type(SM7M_MP_SE8M);
		set_hardware_version(0,0);
		set_soc_forever();
		}
	else if (mcu_get_se6_aiucore()){
                set_soc_forever();
	 	set_board_type(SM7_HK);
	}
	else{
		set_board_type(SM7_HK);
	}
	
	mcu_init(&i2c1_slave_ctx);
	mcu_eeprom_init(&i2c1_slave_ctx);
	wdt_init(&i2c1_slave_ctx);
	//slt_init(&i2c1_slave_ctx);

	if (tca6416a_available())
		tca6416a_init(&i2c1_slave_ctx);

	if (get_board_type() == SM7M_MP_SE8M)
		kbd_init(&i2c1_slave_ctx);
	if (pic_available())
		pic_init(&i2c1_slave_ctx);

	ct7451_init(&i2c1_slave_ctx);

	if (get_work_mode() == WORK_MODE_SOC) {
		sm7_init();
		if (get_board_type() == SM7M_MP_SE8M) {
			se6_init();
			if (get_eeprom_type() == AT24C01D)
				at24c01d_init(&i2c1_slave_ctx);
			else
				at24c128c_init(&i2c1_slave_ctx);
		}
	}

	/* start i2c slaves */
	i2c_slave_start(&i2c1_slave_ctx);

	if (get_work_mode() == WORK_MODE_SOC)
		chip_enable();
	else
		pcie_init();

	/* never return */
	loop_start();

	return 0;
}

void i2c1_isr(void)
{
	i2c_slave_isr(&i2c1_slave_ctx);
}

void i2c2_isr(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}
