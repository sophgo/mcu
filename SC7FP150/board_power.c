/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int pmic_outc_on(void);
void pmic_outc_off(void);

int pmic_outa_outd_on(void);
void pmic_outa_outd_off(void);

int pmic_outb_on(void);
void pmic_outb_off(void);

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[44] = {

	{"PMIC_OUTC", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_outc_on, (unsigned long)pmic_outc_off},
	 },

	{"EN_VDDC_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B1_PORT, (unsigned long)EN_VDDC_B1_PIN},
	 },

	{"EN_VDDC_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B2_PORT, (unsigned long)EN_VDDC_B2_PIN},
	 },

	{"EN_VDDC_B3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B3_PORT, (unsigned long)EN_VDDC_B3_PIN},
	 },

        {"EN_VDDC_B4", NODE_TYPE_ENABLE, 0,
         {(unsigned long)EN_VDDC_B4_PORT, (unsigned long)EN_VDDC_B4_PIN},
         },

        {"EN_VDDC_B5", NODE_TYPE_ENABLE, 0,
         {(unsigned long)EN_VDDC_B5_PORT, (unsigned long)EN_VDDC_B5_PIN},
         },

	{"EN_VDDC_B6", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_B6_PORT, (unsigned long)EN_VDDC_B6_PIN},
	 },

	{"EN_VDDIO33_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B12_PORT,
	  (unsigned long)EN_VDDIO33_B12_PIN},
	 },

	{"EN_VDDIO33_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B34_PORT,
	  (unsigned long)EN_VDDIO33_B34_PIN},
	 },

	{"EN_VDDIO33_B56", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_B56_PORT,
	  (unsigned long)EN_VDDIO33_B56_PIN},
	 },

	{"EN_VDD_PHY_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B1_PORT,
	  (unsigned long)EN_VDD_PHY_B1_PIN},
	 },

	{"EN_VDD_PHY_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B2_PORT,
	  (unsigned long)EN_VDD_PHY_B2_PIN},
	 },

	{"EN_VDD_PHY_B3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B3_PORT,
	  (unsigned long)EN_VDD_PHY_B3_PIN},
	 },

	{"EN_VDD_PHY_B4", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B4_PORT,
	  (unsigned long)EN_VDD_PHY_B4_PIN},
	 },

	{"EN_VDD_PHY_B5", NODE_TYPE_ENABLE, 0,
         {(unsigned long)EN_VDD_PHY_B5_PORT,
          (unsigned long)EN_VDD_PHY_B5_PIN},
         },

	{"EN_VDD_PHY_B6", NODE_TYPE_ENABLE, 1000,
         {(unsigned long)EN_VDD_PHY_B6_PORT,
          (unsigned long)EN_VDD_PHY_B6_PIN},
         },

	{"P08_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B12_PORT, (unsigned long)P08_PG_B12_PIN},
	 },

	{"P08_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B34_PORT, (unsigned long)P08_PG_B34_PIN},
	 },

	{"P08_PG_B56", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)P08_PG_B56_PORT, (unsigned long)P08_PG_B56_PIN},
	 },

	{"PCIE_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B12_PORT, (unsigned long)PCIE_PG_B12_PIN},
	 },

	{"PCIE_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B34_PORT, (unsigned long)PCIE_PG_B34_PIN},
	 },

	{"PCIE_PG_B56", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)PCIE_PG_B56_PORT, (unsigned long)PCIE_PG_B56_PIN},
	 },

	{"EN_VDD_TPU_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B1_PORT,
	  (unsigned long)EN_VDD_TPU_B1_PIN},
	 },

	{"EN_VDD_TPU_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B2_PORT,
	  (unsigned long)EN_VDD_TPU_B2_PIN},
	 },

	{"EN_VDD_TPU_B3", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B3_PORT,
	  (unsigned long)EN_VDD_TPU_B3_PIN},
	 },

	{"EN_VDD_TPU_B4", NODE_TYPE_ENABLE, 0,
         {(unsigned long)EN_VDD_TPU_B4_PORT,
          (unsigned long)EN_VDD_TPU_B4_PIN},
         },

	{"EN_VDD_TPU_B5", NODE_TYPE_ENABLE, 0,
         {(unsigned long)EN_VDD_TPU_B5_PORT,
          (unsigned long)EN_VDD_TPU_B5_PIN},
         },

	{"EN_VDD_TPU_B6", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_TPU_B6_PORT,
	  (unsigned long)EN_VDD_TPU_B6_PIN},
	 },

	{"TPU_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B12_PORT, (unsigned long)TPU_PG_B12_PIN},
	 },

	{"TPU_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B34_PORT, (unsigned long)TPU_PG_B34_PIN},
	 },

	{"TPU_PG_B56", NODE_TYPE_ENABLE, 4000,
	 {(unsigned long)TPU_PG_B56_PORT, (unsigned long)TPU_PG_B56_PIN},
	 },

	{"PMIC_OUTA_OUTD", NODE_TYPE_FUNCTION, 2000,
	 {(unsigned long)pmic_outa_outd_on, (unsigned long)pmic_outa_outd_off},
	 },

	{"PMIC_OUTB", NODE_TYPE_FUNCTION, 2000,
	 {(unsigned long)pmic_outb_on, (unsigned long)pmic_outb_off},
	 },

	{"TPUMEM_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPUMEM_PG_B12_PORT, (unsigned long)TPUMEM_PG_B12_PIN},
	 },

	{"TPUMEM_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPUMEM_PG_B34_PORT, (unsigned long)TPUMEM_PG_B34_PIN},
	 },

	{"TPUMEM_PG_B56", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)TPUMEM_PG_B56_PORT, (unsigned long)TPUMEM_PG_B56_PIN},
	 },

	{"EN_VQPS18_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_B12_PORT, (unsigned long)EN_VQPS18_B12_PIN},
	 },

	{"EN_VQPS18_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_B34_PORT, (unsigned long)EN_VQPS18_B34_PIN},
	 },

	{"EN_VQPS18_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_B56_PORT, (unsigned long)EN_VQPS18_B56_PIN},
	 },

	{"CHIP_ASSERT_N", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)chip_assert_n_on, (unsigned long)chip_assert_n_off},
	 },

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)chip_deassert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

	{"DDR_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B12_PORT, (unsigned long)DDR_PG_B12_PIN},
	 },

	{"DDR_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B34_PORT, (unsigned long)DDR_PG_B34_PIN},
	 },

	{"DDR_PG_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B56_PORT, (unsigned long)DDR_PG_B56_PIN},
	 },

};

/* AUTO GENERATED CODE END */
