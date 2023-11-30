#include <axp15060.h>
#include <chip.h>

int pmic_dcdc_1_on(void)
{
	axp15060_channel_on(AXP15060_DCDC1);
	return 0;
}

void pmic_dcdc_1_off(void)
{
	axp15060_channel_off(AXP15060_DCDC1);
}

int  pmic_dcdc_3_on(void)
{
	axp15060_channel_on(AXP15060_DCDC3);
	return 0;
}

void  pmic_dcdc_3_off(void)
{
	axp15060_channel_off(AXP15060_DCDC3);
}

int pmic_dcdc_4_on(void)
{
	axp15060_channel_on(AXP15060_DCDC4);
	return 0;
}

void pmic_dcdc_4_off(void)
{
	axp15060_channel_off(AXP15060_DCDC4);
}

int pmic_dcdc_5_on(void)
{
	axp15060_channel_on(AXP15060_DCDC5);
	return 0;
}

int pmic_dcdc_5_off(void)
{
	axp15060_channel_off(AXP15060_DCDC5);
	return 0;
}

int pmic_aldo_1_on(void)
{
	axp15060_channel_on(AXP15060_ALDO1);
	return 0;
}

void pmic_aldo_1_off(void)
{
	axp15060_channel_off(AXP15060_ALDO1);
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
	return 0;
}

void sys_rst_assert_off(void)
{
	chip_disable();
}

