/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int milkv_atx_ctl_on(void);
void milkv_atx_ctl_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct power_node const board_power_nodes[22] = {

	{"MILKV_ATX_CTL", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)milkv_atx_ctl_on, (unsigned long)milkv_atx_ctl_off},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDDIO18", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"EN_VDDC", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"PG_VDDC", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDDC_PORT, (unsigned long)PG_VDDC_PIN},
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

	{"EN_VDD_PCIE_H_1V8", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_PCIE_H_1V8_PORT,
	  (unsigned long)EN_VDD_PCIE_H_1V8_PIN},
	 },

	{"PG_DDR_VDD_0V8", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_DDR_VDD_0V8_PORT,
	  (unsigned long)PG_DDR_VDD_0V8_PIN},
	 },

	{"EN_DDR01_VPP_2V5", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VPP_2V5_PORT,
	  (unsigned long)EN_DDR01_VPP_2V5_PIN},
	 },

	{"EN_DDR23_VPP_2V5", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VPP_2V5_PORT,
	  (unsigned long)EN_DDR23_VPP_2V5_PIN},
	 },

	{"EN_DDR01_VDDQ_1V2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR01_VDDQ_1V2_PIN},
	 },

	{"EN_DDR23_VDDQ_1V2", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VDDQ_1V2_PORT,
	  (unsigned long)EN_DDR23_VDDQ_1V2_PIN},
	 },

	{"PG_DDR01_VDDQ_1V2", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_DDR01_VDDQ_1V2_PORT,
	  (unsigned long)PG_DDR01_VDDQ_1V2_PIN},
	 },

	{"PG_DDR23_VDDQ_1V2", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_DDR23_VDDQ_1V2_PORT,
	  (unsigned long)PG_DDR23_VDDQ_1V2_PIN},
	 },

	{"EN_DDR01_VTT_0V6", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR01_VTT_0V6_PORT,
	  (unsigned long)EN_DDR01_VTT_0V6_PIN},
	 },

	{"EN_DDR23_VTT_0V6", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_DDR23_VTT_0V6_PORT,
	  (unsigned long)EN_DDR23_VTT_0V6_PIN},
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
