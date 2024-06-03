/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct power_node const board_power_nodes[20] = {

	{"PWR_OK_C", NODE_TYPE_CHECK, 500,
	 {(unsigned long)PWR_OK_C_PORT, (unsigned long)PWR_OK_C_PIN},
	 },

	{"EN_VDDC", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"EN_VDDR", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_PORT, (unsigned long)EN_VDDR_PIN},
	 },

	{"EN_VDD_PLL", NODE_TYPE_ENABLE, 1500,
	 {(unsigned long)EN_VDD_PLL_PORT, (unsigned long)EN_VDD_PLL_PIN},
	 },

	{"EN_DDR_VDD", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_PORT, (unsigned long)EN_DDR_VDD_PIN},
	 },

	{"EN_VDDIO12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO12_PORT, (unsigned long)EN_VDDIO12_PIN},
	 },

	{"EN_VDDIO12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO12_PORT, (unsigned long)EN_VDDIO12_PIN},
	 },

	{"SLOT1_PRSENT2_3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)SLOT1_PRSENT2_3_PORT,
	  (unsigned long)SLOT1_PRSENT2_3_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VQPS18", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
	 },

	{"EN_VP_C2C", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_C2C_PORT, (unsigned long)EN_VP_C2C_PIN},
	 },

	{"EN_VPH_C2C", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_C2C_PORT, (unsigned long)EN_VPH_C2C_PIN},
	 },

	{"EN_VP_CXP", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_CXP_PORT, (unsigned long)EN_VP_CXP_PIN},
	 },

	{"EN_VPH_CXP", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_CXP_PORT, (unsigned long)EN_VPH_CXP_PIN},
	 },

	{"EN_VP_ETH", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_ETH_PORT, (unsigned long)EN_VP_ETH_PIN},
	 },

	{"EN_VPH_ETH", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_VPH_ETH_PORT, (unsigned long)EN_VPH_ETH_PIN},
	 },

	{"EN_DDR_VAA", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_DDR_VAA_PORT, (unsigned long)EN_DDR_VAA_PIN},
	 },

	{"EN_DDR23_VTT_0V6", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_DDR23_VTT_0V6_PORT,
	  (unsigned long)EN_DDR23_VTT_0V6_PIN},
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
