/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[26] = {

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 27200,
	 {(unsigned long)chip_deassert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDD5V", NODE_TYPE_ENABLE, 12800,
	 {(unsigned long)EN_VDD_5V_PORT, (unsigned long)EN_VDD_5V_PIN},
	 },

	{"EN_DDR_VDD_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_BM0_PORT,
	  (unsigned long)EN_DDR_VDD_BM0_PIN},
	 },

	{"EN_DDR_VDD_BM1", NODE_TYPE_ENABLE, 18000,
	 {(unsigned long)EN_DDR_VDD_BM1_PORT,
	  (unsigned long)EN_DDR_VDD_BM1_PIN},
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

	{"EN_VDDR_B1", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDR_BM1_PORT, (unsigned long)EN_VDDR_BM1_PIN},
	 },

	{"EN_VDD_PLL_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM0_PORT,
	  (unsigned long)EN_VDD_PLL_BM0_PIN},
	 },

	{"EN_VDD_PLL_BM1", NODE_TYPE_ENABLE, 1200,
	 {(unsigned long)EN_VDD_PLL_BM1_PORT,
	  (unsigned long)EN_VDD_PLL_BM1_PIN},
	 },

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, 100,
	 {(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
	 },

	{"EN_VDD_1V2", NODE_TYPE_ENABLE, 2700,
	 {(unsigned long)EN_VDD_1V2_PORT, (unsigned long)EN_VDD_1V2_PIN},
	 },

	{"EN_DDR_VDD2H_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD2H_BM0_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM0_PIN},
	 },

	{"EN_DDR_VDD2H_BM1", NODE_TYPE_ENABLE, 2500,
	 {(unsigned long)EN_DDR_VDD2H_BM1_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM1_PIN},
	 },

	{"EN_VPH_PCIE_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM0_PORT,
	  (unsigned long)EN_VPH_PCIE_BM0_PIN},
	 },

	{"EN_VPH_PCIE_BM1", NODE_TYPE_ENABLE, 500,
	 {(unsigned long)EN_VPH_PCIE_BM1_PORT,
	  (unsigned long)EN_VPH_PCIE_BM1_PIN},
	 },

	{"EN_VP_PCIE_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_PCIE_BM0_PORT,
	  (unsigned long)EN_VP_PCIE_BM0_PIN},
	 },

	{"EN_VP_PCIE_BM1", NODE_TYPE_ENABLE, 4000,
	 {(unsigned long)EN_VP_PCIE_BM1_PORT,
	  (unsigned long)EN_VP_PCIE_BM1_PIN},
	 },

	/* The pin of DDR_VDD1_BM0 is different between the first version PCB(VER1) 
	 * and subsequent PCBs, and can be simultaneously enabled for compatibility 
	 * without using PC10 as the serial port */
	{"EN_DDR_VDD1_BM0_VER1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD1_BM0_VER0_PORT,
	  (unsigned long)EN_DDR_VDD1_BM0_VER0_PIN},
	 },

	{"EN_DDR_VDD1_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD1_BM0_PORT,
	  (unsigned long)EN_DDR_VDD1_BM0_PIN},
	 },

	{"EN_DDR_VDD1_BM1", NODE_TYPE_ENABLE, 3900,
	 {(unsigned long)EN_DDR_VDD1_BM1_PORT,
	  (unsigned long)EN_DDR_VDD1_BM1_PIN},
	 },

	{"EN_DDR_VAA_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VAA_BM0_PORT,
	  (unsigned long)EN_DDR_VAA_BM0_PIN},
	 },

	{"EN_DDR_VAA_BM1", NODE_TYPE_ENABLE, 2800,
	 {(unsigned long)EN_DDR_VAA_BM1_PORT,
	  (unsigned long)EN_DDR_VAA_BM1_PIN},
	 },

	{"EN_DDR_VDDQ_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDDQ_BM0_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM0_PIN},
	 },

	{"EN_DDR_VDDQ_BM1", NODE_TYPE_ENABLE, 8300,
	 {(unsigned long)EN_DDR_VDDQ_BM1_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM1_PIN},
	 },

	// {"CHIP_ASSERT_N", NODE_TYPE_FUNCTION, 1000,
	//  {(unsigned long)chip_assert_n_on, (unsigned long)chip_assert_n_off},
	//  },

	// {"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 1000,
	//  {(unsigned long)chip_deassert_n_on,
	//   (unsigned long)chip_deassert_n_off},
	//  },

};

/* AUTO GENERATED CODE END */
