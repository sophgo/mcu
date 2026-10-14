#include <pin.h>
#include <tick.h>
#include <timer.h>
#include <debug.h>
#include <common.h>
#include <chip.h>
#include <system.h>
#include <dbgi2c.h>

/* wait debug i2c ready */
#define CHIP_BOOT_TIME	1500
#define C2C_LINK_STATUS 0x7050000208
#define C2C_START       0xdeadbeef
#define C2C_PULLUP      0x5a5a5a5a

static volatile int is_chip_ready;
static volatile int is_chip_enabled;
static volatile int need_enable_chip;

static volatile int chip_vddr_status;

int chip_is_enabled(void)
{
	if (is_chip_enabled)
		return tick64_get() > CHIP_BOOT_TIME;
	else
		return false;
}

void chip_destroy(void)
{
	nvic_disable_irq(PCIE_RESET_NVIC);
	exti_interrupt_disable(PCIE_RESET_EXTI);
	exti_reset_request(PCIE_RESET_EXTI);
	timer_stop();
	is_chip_ready = 0;
}
void sys_rst_enable(void)
{
	/* PB0 is adc pin in this pcb, next pcb version will change other pin,
	 * so, now hw version is not work, becasuse this pin is used
	 * to control pcie switch prst
	 */
	gpio_bit_set(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	gpio_bit_set(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);

	/* count from 0 when first time enabled */

	if (!is_chip_enabled)
		tick64_set(0);

	is_chip_enabled = true;
}

void sys_rst_disable(void)
{
	is_chip_enabled = false;

	gpio_bit_reset(SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN);
	gpio_bit_reset(SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN);
}

int chip_enable(void)
{
	return is_chip_enabled;
}

void c2c_rst_enbale(void)
{
	gpio_bit_set(BM0_C2C2_L0_ERST_H_PORT, BM0_C2C2_L0_ERST_H_PIN);
	gpio_bit_set(BM1_C2C4_L0_ERST_H_PORT, BM1_C2C4_L0_ERST_H_PIN);
}

void set_vddr_wake_status(int chip, int flag)
{
	if (flag){
		if (chip == 0)
			gpio_bit_set(BM0_VDDR_WAKE_PORT, BM0_VDDR_WAKE_PIN);
		else
			gpio_bit_set(BM1_VDDR_WAKE_PORT, BM1_VDDR_WAKE_PIN);
	} else {
		if (chip == 0)
			gpio_bit_reset(BM0_VDDR_WAKE_PORT, BM0_VDDR_WAKE_PIN);
		else
			gpio_bit_reset(BM1_VDDR_WAKE_PORT, BM1_VDDR_WAKE_PIN);
	}

	chip_vddr_status &= 1 << chip;
}

int get_chip_vddr_status(int chip)
{
	return (chip_vddr_status & 1 << chip);
}

void host_powergood_init(void)
{
	gpio_mode_set(HOST_PWRGD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, HOST_PWRGD_PIN);
	gpio_output_options_set(HOST_PWRGD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, HOST_PWRGD_PIN);
	gpio_bit_reset(HOST_PWRGD_PORT, HOST_PWRGD_PIN);
	dbg_printf("HOST_PWRGD after power sequence: %d\n", gpio_output_bit_get(HOST_PWRGD_PORT, HOST_PWRGD_PIN));
}

void reset_c2c(void)
{
	uint32_t val;
	int i, retry;

	if (!gpio_output_bit_get(HOST_PWRGD_PORT, HOST_PWRGD_PIN))
		return;

	for (i = 0; i < SOC_NUM; i++) {
		for (retry = 0; retry < 3; retry++) {
			if (dbgi2c_read32(i, C2C_LINK_STATUS, &val) == 0) {
				if (val != 0x0)
					return;
				break;
			}
			mdelay(1);
		}
		if (retry >= 3) {
			dbg_printf("C2C I2C read failed on SOC%d, assume link down\n", i);
			return;
		}
	}

	gpio_bit_reset(HOST_PWRGD_PORT, HOST_PWRGD_PIN);
	dbg_printf("C2C link down, pull down HOST_PWRGD\n");
}

void c2c_check(void)
{
	uint32_t val = 0x0;
	int i, retry;

	if (gpio_output_bit_get(HOST_PWRGD_PORT, HOST_PWRGD_PIN))
		return;

	for (i = 0; i < SOC_NUM; i++) {
		for (retry = 0; retry < 3; retry++) {
			if (dbgi2c_read32(i, C2C_LINK_STATUS, &val) == 0) {
				if (val == C2C_START)
					break;
				return;
			}
			mdelay(1);
		}
		if (retry >= 3)
			return;
	}

	gpio_bit_set(HOST_PWRGD_PORT, HOST_PWRGD_PIN);
	dbg_printf("C2C link start, pull up HOST_PWRGD\n");

	for (i = 0; i < SOC_NUM; i++) {
		if (dbgi2c_write32(i, C2C_LINK_STATUS, C2C_PULLUP))
			dbg_printf("C2C PULLUP write failed on SOC%d\n", i);
	}
}

