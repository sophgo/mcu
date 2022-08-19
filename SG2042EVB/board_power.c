/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

struct power_node const board_power_nodes[16] = {

	{"EN_VDDC_B12", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDDC_B34", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"EN_VDDC_B56", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"EN_VDDC_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"EN_VDDIO33_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_0V8_PORT,
	  (unsigned long)EN_DDR_VDD_0V8_PIN},
	 },

	{"EN_VDDIO33_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PCIE_D_0V8_PORT,
	  (unsigned long)EN_VDD_PCIE_D_0V8_PIN},
	 },

	{"EN_VDDIO33_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_0V8_PORT,
	  (unsigned long)EN_VDD_PLL_0V8_PIN},
	 },

	{"EN_VDDIO33_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_PCIE_H_1V8_PORT,
	  (unsigned long)EN_VDD_PCIE_H_1V8_PIN},
	 },

	{"EN_VDD_PHY_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VPP_2V5_PORT,
	  (unsigned long)EN_DDR01_VPP_2V5_PIN},
	 },

	{"EN_VDD_PHY_B34", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VPP_2V5_PORT,
	  (unsigned long)EN_DDR23_VPP_2V5_PIN},
	 },

	{"EN_VDD_PHY_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR01_VDDQ_1V2_PIN},
	 },

	{"EN_VDD_PHY_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR23_VDDQ_1V2_PIN},
	 },

	{"P08_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VTT_0V6_PORT,
	  (unsigned long)EN_DDR01_VTT_0V6_PIN},
	 },

	{"P08_PG_B34", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VTT_0V6_PORT,
	  (unsigned long)EN_DDR23_VTT_0V6_PIN},
	 },

	{"P08_PG_B56", NODE_TYPE_ENABLE, 30000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
	 },

	{"P08_PG_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)SYS_RST_X_H_PORT, (unsigned long)SYS_RST_X_H_PIN},
	 },

};

/* AUTO GENERATED CODE END */
