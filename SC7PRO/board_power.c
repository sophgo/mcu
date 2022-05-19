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

struct power_node const board_power_nodes[51] = {

	{"PMIC_OUTC", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_outc_on, (unsigned long)pmic_outc_off},
	 },

	{"EN_VDDC_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B12_PORT, (unsigned long)EN_VDDC_B12_PIN},
	 },

	{"EN_VDDC_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B34_PORT, (unsigned long)EN_VDDC_B34_PIN},
	 },

	{"EN_VDDC_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_B56_PORT, (unsigned long)EN_VDDC_B56_PIN},
	 },

	{"EN_VDDC_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_B78_PORT, (unsigned long)EN_VDDC_B78_PIN},
	 },

	{"EN_VDDIO33_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B12_PORT,
	  (unsigned long)EN_VDDIO33_B12_PIN},
	 },

	{"EN_VDDIO33_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B34_PORT,
	  (unsigned long)EN_VDDIO33_B34_PIN},
	 },

	{"EN_VDDIO33_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_B56_PORT,
	  (unsigned long)EN_VDDIO33_B56_PIN},
	 },

	{"EN_VDDIO33_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_B78_PORT,
	  (unsigned long)EN_VDDIO33_B78_PIN},
	 },

	{"EN_VDD_PHY_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B12_PORT,
	  (unsigned long)EN_VDD_PHY_B12_PIN},
	 },

	{"EN_VDD_PHY_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B34_PORT,
	  (unsigned long)EN_VDD_PHY_B34_PIN},
	 },

	{"EN_VDD_PHY_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_B56_PORT,
	  (unsigned long)EN_VDD_PHY_B56_PIN},
	 },

	{"EN_VDD_PHY_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_PHY_B78_PORT,
	  (unsigned long)EN_VDD_PHY_B78_PIN},
	 },

	{"P08_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B12_PORT, (unsigned long)P08_PG_B12_PIN},
	 },

	{"P08_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B34_PORT, (unsigned long)P08_PG_B34_PIN},
	 },

	{"P08_PG_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_B56_PORT, (unsigned long)P08_PG_B56_PIN},
	 },

	{"P08_PG_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)P08_PG_B78_PORT, (unsigned long)P08_PG_B78_PIN},
	 },

	{"PCIE_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B12_PORT, (unsigned long)PCIE_PG_B12_PIN},
	 },

	{"PCIE_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B34_PORT, (unsigned long)PCIE_PG_B34_PIN},
	 },

	{"PCIE_PG_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)PCIE_PG_B56_PORT, (unsigned long)PCIE_PG_B56_PIN},
	 },

	{"PCIE_PG_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)PCIE_PG_B78_PORT, (unsigned long)PCIE_PG_B78_PIN},
	 },

	{"EN_VDD_TPU_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B12_PORT,
	  (unsigned long)EN_VDD_TPU_B12_PIN},
	 },

	{"EN_VDD_TPU_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B34_PORT,
	  (unsigned long)EN_VDD_TPU_B34_PIN},
	 },

	{"EN_VDD_TPU_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_B56_PORT,
	  (unsigned long)EN_VDD_TPU_B56_PIN},
	 },

	{"EN_VDD_TPU_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_TPU_B78_PORT,
	  (unsigned long)EN_VDD_TPU_B78_PIN},
	 },

	{"TPU_PG_B12", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B12_PORT, (unsigned long)TPU_PG_B12_PIN},
	 },

	{"TPU_PG_B34", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B34_PORT, (unsigned long)TPU_PG_B34_PIN},
	 },

	{"TPU_PG_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPU_PG_B56_PORT, (unsigned long)TPU_PG_B56_PIN},
	 },

	{"TPU_PG_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)TPU_PG_B78_PORT, (unsigned long)TPU_PG_B78_PIN},
	 },

	{"PMIC_OUTA_OUTD", NODE_TYPE_FUNCTION, 1000,
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

	{"TPUMEM_PG_B56", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)TPUMEM_PG_B56_PORT, (unsigned long)TPUMEM_PG_B56_PIN},
	 },

	{"TPUMEM_PG_B78", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)TPUMEM_PG_B78_PORT, (unsigned long)TPUMEM_PG_B78_PIN},
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

	{"EN_VQPS18_B78", NODE_TYPE_ENABLE, 2000,
	 {(unsigned long)EN_VQPS18_B78_PORT, (unsigned long)EN_VQPS18_B78_PIN},
	 },

	{"B1_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B1_SYS_RST_N_PORT, (unsigned long)B1_SYS_RST_N_PIN},
	 },

	{"B2_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B2_SYS_RST_N_PORT, (unsigned long)B2_SYS_RST_N_PIN},
	 },

	{"B3_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B3_SYS_RST_N_PORT, (unsigned long)B3_SYS_RST_N_PIN},
	 },

	{"B4_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B4_SYS_RST_N_PORT, (unsigned long)B4_SYS_RST_N_PIN},
	 },

	{"B5_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B5_SYS_RST_N_PORT, (unsigned long)B5_SYS_RST_N_PIN},
	 },

	{"B6_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B6_SYS_RST_N_PORT, (unsigned long)B6_SYS_RST_N_PIN},
	 },

	{"B7_SYS_RST_N", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)B7_SYS_RST_N_PORT, (unsigned long)B7_SYS_RST_N_PIN},
	 },

	{"B8_SYS_RST_N", NODE_TYPE_ENABLE, 30000,
	 {(unsigned long)B8_SYS_RST_N_PORT, (unsigned long)B8_SYS_RST_N_PIN},
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

	{"DDR_PG_B78", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)DDR_PG_B78_PORT, (unsigned long)DDR_PG_B78_PIN},
	 },

};

/* AUTO GENERATED CODE END */
