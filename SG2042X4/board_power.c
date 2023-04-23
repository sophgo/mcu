/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct power_node const board_power_nodes[23] = {

	{"EN_3V3_AUX_PCIE", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_3V3_AUX_PCIE_PORT,
	  (unsigned long)EN_3V3_AUX_PCIE_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDD_2V5_ASM", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_2V5_ASM_PORT,
	  (unsigned long)EN_VDD_2V5_ASM_PIN},
	 },

	{"EN_VDDIO18", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"EN_VDDC", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"EN_VDDIO33", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"EN_DDR_VDD_0V8", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_0V8_PORT,
	  (unsigned long)EN_DDR_VDD_0V8_PIN},
	 },

	{"EN_VDD_PCIE_D_0V8", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PCIE_D_0V8_PORT,
	  (unsigned long)EN_VDD_PCIE_D_0V8_PIN},
	 },

	{"EN_VDD_PLL_0V8", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_0V8_PORT,
	  (unsigned long)EN_VDD_PLL_0V8_PIN},
	 },

	{"EN_VDD_2V5", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_2V5_PORT, (unsigned long)EN_VDD_2V5_PIN},
	 },

	{"EN_VDD_1V5", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_1V5_PORT, (unsigned long)EN_VDD_1V5_PIN},
	 },

	{"EN_VDD_PCIE_H_1V8", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_PCIE_H_1V8_PORT,
	  (unsigned long)EN_VDD_PCIE_H_1V8_PIN},
	 },

	{"EN_DDR01_VPP_2V5", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VPP_2V5_PORT,
	  (unsigned long)EN_DDR01_VPP_2V5_PIN},
	 },

	{"EN_VDD_1V05_ASM", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_1V05_ASM_PORT,
	  (unsigned long)EN_VDD_1V05_ASM_PIN},
	 },

	{"EN_VDD_1V05", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_1V05_PORT, (unsigned long)EN_VDD_1V05_PIN},
	 },

	{"EN_DDR23_VPP_2V5", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VPP_2V5_PORT,
	  (unsigned long)EN_DDR23_VPP_2V5_PIN},
	 },

	{"EN_DDR01_VDDQ_1V2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR01_VDDQ_1V2_PIN},
	 },

	{"EN_VDD_1V2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_1V2_PORT, (unsigned long)EN_VDD_1V2_PIN},
	 },

	{"EN_VDD_1V25", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_1V25_PORT, (unsigned long)EN_VDD_1V25_PIN},
	 },

	{"EN_DDR23_VDDQ_1V2", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_DDR23_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR23_VDDQ_1V2_PIN},
	 },

	{"EN_VQPS18", NODE_TYPE_ENABLE, 30000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
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
