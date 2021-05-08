#include <pin.h>
#include <isl91212.h>

int pmic_channel_a_on(void)
{
	/* LDO 1V */
	return isl91212_buck_on(0);
}

void pmic_channel_a_off(void)
{
	isl91212_buck_off(0);
}

int pmic_channel_b_on(void)
{
	return isl91212_buck_on(1);
}

void pmic_channel_b_off(void)
{
	isl91212_buck_off(1);
}

int pmic_channel_c_on(void)
{
	return isl91212_buck_on(2);
}

void pmic_channel_c_off(void)
{
	isl91212_buck_off(2);
}

int check_pcie_reset_on(void)
{
	/* add customer code here */
	return 0;
}

void check_pcie_reset_off(void)
{
	/* add customer code here */
}

int enable_tpu_mem_on(void)
{
	gpio_bit_reset(EN_VDD_TPU_MEM_PORT, EN_VDD_TPU_MEM_PIN);
	return 0;
}

void enable_tpu_mem_off(void)
{
	gpio_bit_set(EN_VDD_TPU_MEM_PORT, EN_VDD_TPU_MEM_PIN);
}

int sys_rst_deassert_on(void)
{
	gpio_bit_set(SYS_RST_PORT, SYS_RST_PIN);
	return 0;
}

void sys_rst_deassert_off(void)
{
}

int sys_rst_assert_on(void)
{
	gpio_bit_reset(SYS_RST_PORT, SYS_RST_PIN);
	return 0;
}

void sys_rst_assert_off(void)
{
	gpio_bit_reset(SYS_RST_PORT, SYS_RST_PIN);
}

