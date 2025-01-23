/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int pmic_outa_on(void);
void pmic_outa_off(void);

int pmic_outd_on(void);
void pmic_outd_off(void);

int pmic_outc_on(void);
void pmic_outc_off(void);

int pmic_outb_on(void);
void pmic_outb_off(void);

int chip_assert_n_on(void);
void chip_assert_n_off(void);

int chip_deassert_n_on(void);
void chip_deassert_n_off(void);

struct power_node const board_power_nodes[33] = {

	{"PMIC_OUTA", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_outa_on, (unsigned long)pmic_outa_off},
	 },

	{"EN_VDDC_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B1_PORT, (unsigned long)EN_VDDC_B1_PIN},
	 },

	{"EN_VDDC_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B2_PORT, (unsigned long)EN_VDDC_B2_PIN},
	 },

	{"EN_VDDC_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_B3_PORT, (unsigned long)EN_VDDC_B3_PIN},
	 },

	{"EN_VDDIO33_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B1_PORT, (unsigned long)EN_VDDIO33_B1_PIN},
	 },

	{"EN_VDDIO33_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B2_PORT, (unsigned long)EN_VDDIO33_B2_PIN},
	 },

	{"EN_VDDIO33_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_B3_PORT, (unsigned long)EN_VDDIO33_B3_PIN},
	 },

	{"PMIC_OUTD", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_outd_on, (unsigned long)pmic_outd_off},
	 },

	{"P08_PG_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B1_PORT, (unsigned long)P08_PG_B1_PIN},
	 },

	{"P08_PG_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B2_PORT, (unsigned long)P08_PG_B2_PIN},
	 },

	{"P08_PG_B3", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)P08_PG_B3_PORT, (unsigned long)P08_PG_B3_PIN},
	 },

	{"PCIE_PG_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B1_PORT, (unsigned long)PCIE_PG_B1_PIN},
	 },

	{"PCIE_PG_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B2_PORT, (unsigned long)PCIE_PG_B2_PIN},
	 },

	{"PCIE_PG_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)PCIE_PG_B3_PORT, (unsigned long)PCIE_PG_B3_PIN},
	 },

	{"EN_VDD_TPU_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B1_PORT, (unsigned long)EN_VDD_TPU_B1_PIN},
	 },

	{"EN_VDD_TPU_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B2_PORT, (unsigned long)EN_VDD_TPU_B2_PIN},
	 },

	{"EN_VDD_TPU_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_TPU_B3_PORT, (unsigned long)EN_VDD_TPU_B3_PIN},
	 },

	{"TPU_PG_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B1_PORT, (unsigned long)TPU_PG_B1_PIN},
	 },

	{"TPU_PG_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B2_PORT, (unsigned long)TPU_PG_B2_PIN},
	 },

	{"TPU_PG_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)TPU_PG_B3_PORT, (unsigned long)TPU_PG_B3_PIN},
	 },

	{"PMIC_OUTC", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_outc_on, (unsigned long)pmic_outc_off},
	 },

	{"PMIC_OUTB", NODE_TYPE_FUNCTION, 2000,
	 {(unsigned long)pmic_outb_on, (unsigned long)pmic_outb_off},
	 },

	{"TPUMEM_PG_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPUMEM_PG_B1_PORT, (unsigned long)TPUMEM_PG_B1_PIN},
	 },

	{"TPUMEM_PG_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPUMEM_PG_B2_PORT, (unsigned long)TPUMEM_PG_B2_PIN},
	 },

	{"TPUMEM_PG_B3", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)TPUMEM_PG_B3_PORT, (unsigned long)TPUMEM_PG_B3_PIN},
	 },

	{"EN_VQPS18_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_B1_PORT, (unsigned long)EN_VQPS18_B1_PIN},
	 },

	{"EN_VQPS18_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_B2_PORT, (unsigned long)EN_VQPS18_B2_PIN},
	 },

	{"EN_VQPS18_B3", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_VQPS18_B3_PORT, (unsigned long)EN_VQPS18_B3_PIN},
	 },

	{"CHIP_ASSERT_N", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)chip_assert_n_on, (unsigned long)chip_assert_n_off},
	 },

	{"CHIP_DEASSERT_N", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)chip_deassert_n_on,
	  (unsigned long)chip_deassert_n_off},
	 },

	{"DDR_PG_B1", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B1_PORT, (unsigned long)DDR_PG_B1_PIN},
	 },

	{"DDR_PG_B2", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B2_PORT, (unsigned long)DDR_PG_B2_PIN},
	 },

	{"DDR_PG_B3", NODE_TYPE_ENABLE, 10,
	 {(unsigned long)DDR_PG_B3_PORT, (unsigned long)DDR_PG_B3_PIN},
	 },

};

/* AUTO GENERATED CODE END */
