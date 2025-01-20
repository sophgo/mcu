#include <mp5475.h>
#include <chip.h>
#include <common.h>
#include <mp5475.h>
#include <timer.h>
int pmic_channel_a_on(void)
{
	return mp5475_buck_on(0x98);
	// 0x22  1001 0000
	// 0000 1000
	// 1001 1000
}

void pmic_channel_a_off(void)
{
	mp5475_buck_off(0);
}

int pmic_channel_d_on(void)
{
	return mp5475_buck_on(0x18);
	// 0x22   0001 0000
	// 0001 1000
}

void pmic_channel_d_off(void)
{
	mp5475_buck_off(3);                                                                       
}

int pmic_channel_b_on(void)
{
	return mp5475_buck_on(0xf8);
	// 0x22 1111 0000
	// 0x22 1111 0000
	// 1111 1000
}

void pmic_channel_b_off(void)
{
	mp5475_buck_off(1);
}

int pmic_channel_c_on(void)
{
	return mp5475_buck_on(0xb8);
	// 0x22  1011 0000
	// 1011 1000
}

void pmic_channel_c_off(void)
{
	mp5475_buck_off(2);
}

int sys_rst_deassert_on(void)
{
	chip_enable();
	return 0;
}

void sys_rst_deassert_off(void)
{
}

int sys_rst_assert_on(void)
{
	chip_disable();
	// gpio_clear(PCIEE_RST_X_MCU_PORT,PCIEE_RST_X_MCU_PIN);
	return 0;
}

void sys_rst_assert_off(void)
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

int powerchip_init_on(void)
{
	mp5475_init();

	return 0;
}
