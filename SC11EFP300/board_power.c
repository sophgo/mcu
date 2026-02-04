/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[29] = {

	{"EN_VDD_5V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_5V_PORT, (unsigned long)EN_VDD_5V_PIN},
	 },

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	 },

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
	 },

	{"EN_VDDC_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM0_PORT, (unsigned long)EN_VDDC_BM0_PIN},
	 },

	{"EN_VDDC_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM1_PORT, (unsigned long)EN_VDDC_BM1_PIN},
	 },

	{"EN_VDDC_BM2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM2_PORT, (unsigned long)EN_VDDC_BM2_PIN},
	 },

	{"EN_VDDC_BM3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_BM3_PORT, (unsigned long)EN_VDDC_BM3_PIN},
	 },

	{"EN_VDDR_BM0", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM0_PORT, (unsigned long)EN_VDDR_BM0_PIN},
	 },

	{"EN_VDDR_BM1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM1_PORT, (unsigned long)EN_VDDR_BM1_PIN},
	 },

	{"EN_VDDR_BM2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM2_PORT, (unsigned long)EN_VDDR_BM2_PIN},
	 },

	{"EN_VDDR_BM3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDR_BM3_PORT, (unsigned long)EN_VDDR_BM3_PIN},
	 },

	{"EN_VDD_PLL_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM01_PORT,
	  (unsigned long)EN_VDD_PLL_BM01_PIN},
	 },

	{"EN_VDD_PLL_BM23", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PLL_BM23_PORT,
	  (unsigned long)EN_VDD_PLL_BM23_PIN},
	 },

	{"EN_DDR_VDD_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD_BM01_PORT,
	  (unsigned long)EN_DDR_VDD_BM01_PIN},
	 },

	{"EN_DDR_VDD_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_DDR_VDD_BM23_PORT,
	  (unsigned long)EN_DDR_VDD_BM23_PIN},
	 },

	{"EN_VDD_1V2", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_VDD_1V2_PORT, (unsigned long)EN_VDD_1V2_PIN},
	 },

	{"EN_VPH_PCIE_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM01_PORT,
	  (unsigned long)EN_VPH_PCIE_BM01_PIN},
	 },

	{"EN_VPH_PCIE_BM23", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VPH_PCIE_BM23_PORT,
	  (unsigned long)EN_VPH_PCIE_BM23_PIN},
	 },

	{"EN_VP_PCIE_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VP_PCIE_BM01_PORT,
	  (unsigned long)EN_VP_PCIE_BM01_PIN},
	 },

	{"EN_VP_PCIE_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_VP_PCIE_BM23_PORT,
	  (unsigned long)EN_VP_PCIE_BM23_PIN},
	 },

	{"EN_DDR_VAA_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VAA_BM01_PORT,
	  (unsigned long)EN_DDR_VAA_BM01_PIN},
	 },

	{"EN_DDR_VAA_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_DDR_VAA_BM23_PORT,
	  (unsigned long)EN_DDR_VAA_BM23_PIN},
	 },

	{"EN_DDR_VDD1_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD1_BM01_PORT,
	  (unsigned long)EN_DDR_VDD1_BM01_PIN},
	 },

	{"EN_DDR_VDD1_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_DDR_VDD1_BM23_PORT,
	  (unsigned long)EN_DDR_VDD1_BM23_PIN},
	 },

	{"EN_DDR_VDD2H_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDD2H_BM01_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM01_PIN},
	 },

	{"EN_DDR_VDD2H_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_DDR_VDD2H_BM23_PORT,
	  (unsigned long)EN_DDR_VDD2H_BM23_PIN},
	 },

	{"EN_DDR_VDDQ_BM01", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_DDR_VDDQ_BM01_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM01_PIN},
	 },

	{"EN_DDR_VDDQ_BM23", NODE_TYPE_ENABLE, 5000,
	 {(unsigned long)EN_DDR_VDDQ_BM23_PORT,
	  (unsigned long)EN_DDR_VDDQ_BM23_PIN},
	 },

//	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 0,
//	 {(unsigned long)chip_assert_n_on,
//	  (unsigned long)chip_deassert_n_off},
//	 },

//	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 0,
//	 {(unsigned long)sys_rst_x_h_bm1_on,
//	  (unsigned long)sys_rst_x_h_bm1_off},
//	 },

};

/* AUTO GENERATED CODE END */
