/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[25] = {

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 50000,
	 {(unsigned long)chip_deassert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

	{"EN_VDD5V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_5V_PORT, (unsigned long)EN_VDD_5V_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 100000,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDDC_B0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM0_PORT, (unsigned long)EN_VDDC_BM0_PIN},
	 },

	{"EN_VDDC_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM1_PORT, (unsigned long)EN_VDDC_BM1_PIN},
	 },

	{"EN_VDDR_B0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM0_PORT, (unsigned long)EN_VDDR_BM0_PIN},
	 },

	{"EN_VDDR_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM1_PORT, (unsigned long)EN_VDDR_BM1_PIN},
	 },

	{"EN_VDD_PLL_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM0_PORT,
	  (unsigned long)EN_VDD_PLL_BM0_PIN},
	 },

	{"EN_VDD_PLL_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM1_PORT,
	  (unsigned long)EN_VDD_PLL_BM1_PIN},
	 },

	{"EN_DDR_VDD_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_BM0_PORT,
	  (unsigned long)EN_DDR_VDD_BM0_PIN},
	 },

	{"EN_DDR_VDD_BM1", NODE_TYPE_ENABLE, 100000,
	 {(unsigned long)EN_DDR_VDD_BM1_PORT,
	  (unsigned long)EN_DDR_VDD_BM1_PIN},
	 },

	{"EN_VPH_PCIE_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM0_PORT,
	  (unsigned long)EN_VPH_PCIE_BM0_PIN},
	 },

	{"EN_VPH_PCIE_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM1_PORT,
	  (unsigned long)EN_VPH_PCIE_BM1_PIN},
	 },

	{"EN_VP_PCIE_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_PCIE_BM0_PORT,
	  (unsigned long)EN_VP_PCIE_BM0_PIN},
	 },

	{"EN_VP_PCIE_BM1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_VP_PCIE_BM1_PORT,
	  (unsigned long)EN_VP_PCIE_BM1_PIN},
	 },

	{"EN_DDR_VAA_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VAA_BM0_PORT,
	  (unsigned long)EN_DDR_VAA_BM0_PIN},
	 },

	{"EN_DDR_VAA_BM1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VAA_BM1_PORT,
	  (unsigned long)EN_DDR_VAA_BM1_PIN},
	 },

	{"EN_DDR_VDD1_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD1_BM0_PORT,
	  (unsigned long)EN_DDR_VDD1_BM0_PIN},
	 },

	{"EN_DDR_VDD1_BM1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VDD1_BM1_PORT,
	  (unsigned long)EN_DDR_VDD1_BM1_PIN},
	 },

	{"EN_DDR_VDD2H_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD2H_BM0_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM0_PIN},
	 },

	{"EN_DDR_VDD2H_BM1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VDD2H_BM1_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM1_PIN},
	 },

	{"EN_DDR_VDDQ_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDDQ_BM0_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM0_PIN},
	 },

	{"EN_DDR_VDDQ_BM1", NODE_TYPE_ENABLE, 20000,
	 {(unsigned long)EN_DDR_VDDQ_BM1_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM1_PIN},
	 },

	{"CHIP_ASSERT_N", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)chip_assert_n_on, (unsigned long)chip_assert_n_off},
	 },

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)chip_deassert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

};

/* AUTO GENERATED CODE END */
