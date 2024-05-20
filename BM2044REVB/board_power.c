/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct power_node const board_power_nodes[16] = {

	{"EN_VDDC_0V75", NODE_TYPE_ENABLE, 4000,
	 {(unsigned long)EN_VDDC_0V75_PORT, (unsigned long)EN_VDDC_0V75_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 10000,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_DDR_VDD_0V75", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_DDR_VDD_0V75_PORT,
	  (unsigned long)EN_DDR_VDD_0V75_PIN},
	 },

	{"EN_VDDIO_1V2", NODE_TYPE_ENABLE, 1800,
	 {(unsigned long)EN_VDDIO_1V2_PORT, (unsigned long)EN_VDDIO_1V2_PIN},
	 },

	{"EN_VDDR_0V75", NODE_TYPE_ENABLE, 1200,
	 {(unsigned long)EN_VDDR_0V75_PORT, (unsigned long)EN_VDDR_0V75_PIN},
	 },

	{"EN_VDD_PLL_0V75", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_PLL_0V75_PORT,
	  (unsigned long)EN_VDD_PLL_0V75_PIN},
	 },

	{"EN_VDD_DDR_1V8", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_DDR_1V8_PORT,
	  (unsigned long)EN_VDD_DDR_1V8_PIN},
	 },

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
	 },

	{"EN_CXP_VP", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_CXP_VP_PORT, (unsigned long)EN_CXP_VP_PIN},
	 },

	{"EN_CXP_VPH", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_CXP_VPH_PORT, (unsigned long)EN_CXP_VPH_PIN},
	 },

	{"EN_DDR_VDDQ_0V6", NODE_TYPE_ENABLE, 1300,
	 {(unsigned long)EN_DDR_VDDQ_0V6_PORT,
	  (unsigned long)EN_DDR_VDDQ_0V6_PIN},
	 },

	{"EN_VDDR_2H_1V1", NODE_TYPE_ENABLE, 2400,
	 {(unsigned long)EN_VDDR_2H_1V1_PORT,
	  (unsigned long)EN_VDDR_2H_1V1_PIN},
	 },

	{"EN_DDR_VAA", NODE_TYPE_ENABLE, 13000,
	 {(unsigned long)EN_DDR_VAA_PORT, (unsigned long)EN_DDR_VAA_PIN},
	 },

	{"VDDIO_EFUSE_EN", NODE_TYPE_ENABLE, 10000,
	 {(unsigned long)VDDIO_EFUSE_EN_PORT,
	  (unsigned long)VDDIO_EFUSE_EN_PIN},
	 },

	{"SYS_RST_ASSERT", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
	 },

	{"SYS_RST_DEASSERT", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)sys_rst_deassert_on,
	  (unsigned long)sys_rst_deassert_off},
	 },

};

/* AUTO GENERATED CODE END */
