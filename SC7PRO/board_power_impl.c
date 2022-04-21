#include <mp5475.h>
#include <debug.h>
#include <pin.h>
#include <power.h>
#include <board_power.h>
#include <tick.h>
#include <timer.h>

int  en_pmic_outc_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 3);
	mp5475_buck_on(MP5475B34, 3);
	mp5475_buck_on(MP5475B56, 3);
	mp5475_buck_on(MP5475B78, 3);
	return 0;
}

void  en_pmic_outc_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 3);
	mp5475_buck_off(MP5475B34, 3);
	mp5475_buck_off(MP5475B56, 3);
	mp5475_buck_off(MP5475B78, 3);
}

int  en_pmic_outa_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 0);
	mp5475_buck_on(MP5475B34, 0);
	mp5475_buck_on(MP5475B56, 0);
	mp5475_buck_on(MP5475B78, 0);
	return 0;
}

void  en_pmic_outa_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 0);
	mp5475_buck_off(MP5475B34, 0);
	mp5475_buck_off(MP5475B56, 0);
	mp5475_buck_off(MP5475B78, 0);
}

int  en_pmic_outb_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 1);
	mp5475_buck_on(MP5475B34, 1);
	mp5475_buck_on(MP5475B56, 1);
	mp5475_buck_on(MP5475B78, 1);
	return 0;
}

void  en_pmic_outb_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 1);
	mp5475_buck_off(MP5475B34, 1);
	mp5475_buck_off(MP5475B56, 1);
	mp5475_buck_off(MP5475B78, 1);
}

int  en_pmic_outd_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 2);
	mp5475_buck_on(MP5475B34, 2);
	mp5475_buck_on(MP5475B56, 2);
	mp5475_buck_on(MP5475B78, 2);
	return 0;
}

void  en_pmic_outd_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 2);
	mp5475_buck_off(MP5475B34, 2);
	mp5475_buck_off(MP5475B56, 2);
	mp5475_buck_off(MP5475B78, 2);
}

#if 0
int b12_vdd_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 3);
	return 0;
}

void b12_vdd_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 3);
}

int b34_vdd_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 3);
	return 0;
}

void b34_vdd_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 3);
}

int b56_vdd_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 3);
	return 0;
}

void b56_vdd_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 3);
}

int b78_vdd_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 3);
	return 0;
}

void b78_vdd_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 3);
}

int b12_vp_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 2);
	return 0;
}

void b12_vp_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 2);
}

int b34_vp_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 2);
	return 0;
}

void b34_vp_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 2);
}

int b56_vp_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 2);
	return 0;
}

void b56_vp_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 2);
}

int b78_vp_0_8v_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 2);
	return 0;
}

void b78_vp_0_8v_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 2);
}

int b12_ddr_vddq_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 0);
	return 0;
}

void b12_ddr_vddq_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 0);
}

int b34_ddr_vddq_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 0);
	return 0;
}

void b34_ddr_vddq_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 0);
}

int b56_ddr_vddq_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 0);
	return 0;
}

void b56_ddr_vddq_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 0);
}

int b78_ddr_vddq_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 0);
	return 0;
}

void b78_ddr_vddq_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 0);
}

int b12_ddr_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B12, 1);
	return 0;
}

void b12_ddr_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B12, 1);
}

int b34_ddr_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B34, 1);
	return 0;
}

void b34_ddr_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B34, 1);
}

int b56_ddr_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B56, 1);
	return 0;
}

void b56_ddr_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B56, 1);
}

int b78_ddr_vddqlp_on(void)
{
	/* add customer code here */
	mp5475_buck_on(MP5475B78, 1);
	return 0;
}

void b78_ddr_vddqlp_off(void)
{
	/* add customer code here */
	mp5475_buck_off(MP5475B78, 1);
}

#define BN_SYS_RST_ENABLE(n)					\
	do {							\
		gpio_set(B ## n ## _SYS_RST_N_PORT,		\
			 B ## n ## _SYS_RST_N_PIN);		\
	} while (0)


#define BN_SYS_RST_DISABLE(n)					\
	do {							\
		gpio_clear(B ## n ## _SYS_RST_N_PORT,		\
			   B ## n ## _SYS_RST_N_PIN);		\
	} while (0)

#define BN_DDR_PG_ENABLE(n)					\
	do {							\
		gpio_set(B ## n ## _INPUT_PG4_PORT,		\
			 B ## n ## _INPUT_PG4_PIN);		\
	} while (0)


#define BN_DDR_PG_DISABLE(n)					\
	do {							\
		gpio_clear(B ## n ## _INPUT_PG4_PORT,		\
			   B ## n ## _INPUT_PG4_PIN);		\
	} while (0)


static inline void sys_rst_en_all(void)
{
	BN_SYS_RST_ENABLE(1);
	BN_SYS_RST_ENABLE(2);
	BN_SYS_RST_ENABLE(3);
	BN_SYS_RST_ENABLE(4);
	BN_SYS_RST_ENABLE(5);
	BN_SYS_RST_ENABLE(6);
	BN_SYS_RST_ENABLE(7);
	BN_SYS_RST_ENABLE(8);
}

static inline void sys_rst_dis_all(void)
{
	BN_SYS_RST_DISABLE(1);
	BN_SYS_RST_DISABLE(2);
	BN_SYS_RST_DISABLE(3);
	BN_SYS_RST_DISABLE(4);
	BN_SYS_RST_DISABLE(5);
	BN_SYS_RST_DISABLE(6);
	BN_SYS_RST_DISABLE(7);
	BN_SYS_RST_DISABLE(8);
}

int sys_rst_deassert_on(void)
{
	/* add customer code here */
	sys_rst_en_all();
	return 0;
}

void sys_rst_deassert_off(void)
{
	/* add customer code here */
	sys_rst_dis_all();
}

int ddr_power_good_on(void)
{
	/* add customer code here */
	BN_DDR_PG_ENABLE(1);
	BN_DDR_PG_ENABLE(2);
	BN_DDR_PG_ENABLE(3);
	BN_DDR_PG_ENABLE(4);
	BN_DDR_PG_ENABLE(5);
	BN_DDR_PG_ENABLE(6);
	BN_DDR_PG_ENABLE(7);
	BN_DDR_PG_ENABLE(8);
	return 0;
}

void ddr_power_good_off(void)
{
	/* add customer code here */
	BN_DDR_PG_DISABLE(1);
	BN_DDR_PG_DISABLE(2);
	BN_DDR_PG_DISABLE(3);
	BN_DDR_PG_DISABLE(4);
	BN_DDR_PG_DISABLE(5);
	BN_DDR_PG_DISABLE(6);
	BN_DDR_PG_DISABLE(7);
	BN_DDR_PG_DISABLE(8);
}

int sys_rst_assert_on(void)
{
	/* add customer code here */
	sys_rst_dis_all();
	return 0;
}

void sys_rst_assert_off(void)
{
	/* add customer code here */
}
#endif
