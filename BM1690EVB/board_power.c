/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>
#include <debug.h>

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct power_node const board_power_nodes[17] = {

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 100000,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDDC", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"EN_VDDR", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_PORT, (unsigned long)EN_VDDR_PIN},
	 },

	{"EN_VDD_PLL", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_PORT, (unsigned long)EN_VDD_PLL_PIN},
	 },

	{"EN_DDR_VDD", NODE_TYPE_ENABLE, 50000,
	 {(unsigned long)EN_DDR_VDD_PORT, (unsigned long)EN_DDR_VDD_PIN},
	 },

	{"EN_VDDIO12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO12_PORT, (unsigned long)EN_VDDIO12_PIN},
	 },

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, 50000,
	 {(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
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

	{"EN_VPH_ETH", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_VPH_ETH_PORT, (unsigned long)EN_VPH_ETH_PIN},
	 },

	{"EN_DDR_VAA", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VAA_PORT, (unsigned long)EN_DDR_VAA_PIN},
	 },

	{"EN_DDR_VDD1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VDD1_PORT, (unsigned long)EN_DDR_VDD1_PIN},
	 },

	{"EN_DDR_VDD2H", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VDD2H_PORT, (unsigned long)EN_DDR_VDD2H_PIN},
	 },

	{"EN_DDR_VDDQ", NODE_TYPE_ENABLE, 30000,
	 {(unsigned long)EN_DDR_VDDQ_PORT, (unsigned long)EN_DDR_VDDQ_PIN},
	 },

	// {"SYS_RST_ASSERT", NODE_TYPE_FUNCTION, 30000,
	//  {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
	//  },

	// {"SYS_RST_DEASSERT", NODE_TYPE_FUNCTION, 30000,
	//  {(unsigned long)sys_rst_deassert_on,
	//   (unsigned long)sys_rst_deassert_off},
	//  },

};

/* AUTO GENERATED CODE END */
