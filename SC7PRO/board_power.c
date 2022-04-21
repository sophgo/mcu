/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int en_pmic_outc_on(void);
void en_pmic_outc_off(void);

int en_pmic_outa_on(void);
void en_pmic_outa_off(void);

int en_pmic_outb_on(void);
void en_pmic_outb_off(void);

int en_pmic_outd_on(void);
void en_pmic_outd_off(void);

struct power_node board_power_nodes[26] = {

	{"PMIC-OUTC", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
	 {(unsigned long)en_pmic_outc_on, (unsigned long)en_pmic_outc_off},
	 },

	{"VDDC", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"VDD-3.3V", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"VDD-PHY", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDD_PHY_PORT, (unsigned long)EN_VDD_PHY_PIN},
	 },

	{"VP-0.8V-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)P08_PG_1_PORT, (unsigned long)P08_PG_1_PIN},
	 },

	{"VP-0.8V-2", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)P08_PG_2_PORT, (unsigned long)P08_PG_2_PIN},
	 },

	{"PG-PCIE-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)PCIE_PG_1_PORT, (unsigned long)PCIE_PG_1_PIN},
	 },

	{"PG-PCIE-2", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)PCIE_PG_2_PORT, (unsigned long)PCIE_PG_2_PIN},
	 },

	{"VDD-TPU", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDD_TPU_PORT, (unsigned long)EN_VDD_TPU_PIN},
	 },

	{"PG-TPU-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)TPU_PG_1_PORT, (unsigned long)TPU_PG_1_PIN},
	 },

	{"PG-TPU-2", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)TPU_PG_2_PORT, (unsigned long)TPU_PG_2_PIN},
	 },

	{"PMIC-OUTA", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 1000,
	 {(unsigned long)en_pmic_outa_on, (unsigned long)en_pmic_outa_off},
	 },

	{"PMIC-OUTB", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 1000,
	 {(unsigned long)en_pmic_outb_on, (unsigned long)en_pmic_outb_off},
	 },

	{"PG-TPUMEM-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 2000,
	 {(unsigned long)TPUMEM_PG_1_PORT, (unsigned long)TPUMEM_PG_1_PIN},
	 },

	{"PG-TPUMEM-2", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)TPUMEM_PG_2_PORT, (unsigned long)TPUMEM_PG_2_PIN},
	 },

	{"PMIC-OUTD", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 1000,
	 {(unsigned long)en_pmic_outd_on, (unsigned long)en_pmic_outd_off},
	 },

	{"SYS-RST-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 2000,
	 {(unsigned long)B1_SYS_RST_N_PORT, (unsigned long)B1_SYS_RST_N_PIN},
	 },

	{"SYS-RST-2", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B2_SYS_RST_N_PORT, (unsigned long)B2_SYS_RST_N_PIN},
	 },

	{"SYS-RST-3", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B3_SYS_RST_N_PORT, (unsigned long)B3_SYS_RST_N_PIN},
	 },

	{"SYS-RST-4", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B4_SYS_RST_N_PORT, (unsigned long)B4_SYS_RST_N_PIN},
	 },

	{"SYS-RST-5", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B5_SYS_RST_N_PORT, (unsigned long)B5_SYS_RST_N_PIN},
	 },

	{"SYS-RST-6", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B6_SYS_RST_N_PORT, (unsigned long)B6_SYS_RST_N_PIN},
	 },

	{"SYS-RST-7", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B7_SYS_RST_N_PORT, (unsigned long)B7_SYS_RST_N_PIN},
	 },

	{"SYS-RST-8", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)B8_SYS_RST_N_PORT, (unsigned long)B8_SYS_RST_N_PIN},
	 },

	{"PG-DDR-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 30000,
	 {(unsigned long)DDR_PG_1_PORT, (unsigned long)DDR_PG_1_PIN},
	 },

	{"PG-DDR-1", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
	 {(unsigned long)DDR_PG_2_PORT, (unsigned long)DDR_PG_2_PIN},
	 },

};

/* AUTO GENERATED CODE END */
