/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[26] = {

	{"MODULE_12V_EN", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)MODULE_EN_PORT, (unsigned long)MODULE_EN_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
	 },

	{"EN_VDDC_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM0_PORT, (unsigned long)EN_VDDC_BM0_PIN},
	 },

	{"EN_VDDC_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM1_PORT, (unsigned long)EN_VDDC_BM1_PIN},
	 },

	{"EN_VDDR_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM0_PORT, (unsigned long)EN_VDDR_BM0_PIN},
	 },

	{"EN_VDDR_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM1_PORT, (unsigned long)EN_VDDR_BM1_PIN},
	 },

	{"EN_VDD_PLL_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM0_PORT, (unsigned long)EN_VDD_PLL_BM0_PIN},
	 },

	{"EN_VDD_PLL_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM1_PORT, (unsigned long)EN_VDD_PLL_BM1_PIN},
	 },

	{"EN_DDR_VDD_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_BM0_PORT, (unsigned long)EN_DDR_VDD_BM0_PIN},
	 },

	{"EN_DDR_VDD_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_DDR_VDD_BM1_PORT, (unsigned long)EN_DDR_VDD_BM1_PIN},
	 },

	{"EN_VDD_1V2", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_VDD_1V2_PORT, (unsigned long)EN_VDD_1V2_PIN},
	 },

	{"EN_VPH_PCIE_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM0_PORT, (unsigned long)EN_VPH_PCIE_BM0_PIN},
	 },

	{"EN_VPH_PCIE_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM1_PORT, (unsigned long)EN_VPH_PCIE_BM1_PIN},
	 },

	{"EN_VP_PCIE_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_PCIE_BM0_PORT, (unsigned long)EN_VP_PCIE_BM0_PIN},
	 },

	{"EN_VP_PCIE_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_VP_PCIE_BM1_PORT, (unsigned long)EN_VP_PCIE_BM1_PIN},
	 },

	{"EN_DDR_VAA_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VAA_BM0_PORT, (unsigned long)EN_DDR_VAA_BM0_PIN},
	 },

	{"EN_DDR_VAA_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_DDR_VAA_BM1_PORT, (unsigned long)EN_DDR_VAA_BM1_PIN},
	 },

	{"EN_DDR_VDD1_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD1_BM0_PORT, (unsigned long)EN_DDR_VDD1_BM0_PIN},
	 },

	{"EN_DDR_VDD1_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_DDR_VDD1_BM1_PORT, (unsigned long)EN_DDR_VDD1_BM1_PIN},
	 },

	{"EN_DDR_VDD2H_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD2H_BM0_PORT, (unsigned long)EN_DDR_VDD2H_BM0_PIN},
	 },

	{"EN_DDR_VDD2H_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_DDR_VDD2H_BM1_PORT, (unsigned long)EN_DDR_VDD2H_BM1_PIN},
	 },

	{"EN_DDR_VDDQ_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDDQ_BM0_PORT, (unsigned long)EN_DDR_VDDQ_BM0_PIN},
	 },

	{"EN_DDR_VDDQ_BM01", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_DDR_VDDQ_BM1_PORT, (unsigned long)EN_DDR_VDDQ_BM1_PIN},
	 },

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 40000,
	 {(unsigned long)chip_assert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

	{"CHIP_ASSERT_N", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)chip_assert_n_off,
	  (unsigned long)chip_deassert_n_on},
	 },

};

/* AUTO GENERATED CODE END */
