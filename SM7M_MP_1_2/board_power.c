/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int pmic_dcdc_1_on(void);
void pmic_dcdc_1_off(void);

int pmic_dcdc_5_on(void);
void pmic_dcdc_5_off(void);

int pmic_dcdc_4_on(void);
void pmic_dcdc_4_off(void);

int pmic_dcdc_3_on(void);
void pmic_dcdc_3_off(void);

int pciee_rst_x_mcu_on(void);
void pciee_rst_x_mcu_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node const board_power_nodes[21] = {

	{"PMIC-VDD-1.8V", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_dcdc_1_on, (unsigned long)pmic_dcdc_1_off},
	 },

	// {"VDD-3.3V", NODE_TYPE_ENABLE, 1000,
	//  {(unsigned long)EN_VDD3V3_PORT, (unsigned long)EN_VDD3V3_PIN},
	//  },

	// {"VDD-5V", NODE_TYPE_ENABLE, 1000,
	//  {(unsigned long)EN_VDDIO5_PORT, (unsigned long)EN_VDDIO5_PIN},
	//  },

	{"VDD-1.8V", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"VDD-CORE", NODE_TYPE_ENABLE, 3000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"VDD-3.3V", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"VDD-0.8V", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_dcdc_5_on, (unsigned long)pmic_dcdc_5_off},
	 },

	{"ACK-P08", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)P08_PWR_GOOD_PORT, (unsigned long)P08_PWR_GOOD_PIN},
	 },

	{"ACK-PCIE", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)GPIO2_PORT, (unsigned long)GPIO2_PIN},
	 },

	// {"ACK-3V3", NODE_TYPE_ENABLE, 1000,
	//  {(unsigned long)PG_VDD3V3_PORT, (unsigned long)PG_VDD3V3_PIN},
	//  },

	// {"ACK-VDDIO5", NODE_TYPE_ENABLE, 1000,
	//  {(unsigned long)PG_VDDIO5_PORT, (unsigned long)PG_VDDIO5_PIN},
	//  },

	{"VDD-TPU", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDD_TPU_PORT, (unsigned long)EN_VDD_TPU_PIN},
	 },

	{"ACK-TPU", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)GPIO3_PORT, (unsigned long)GPIO3_PIN},
	 },

	{"DDR-VDDQ", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_dcdc_4_on, (unsigned long)pmic_dcdc_4_off},
	 },

	{"DDR-VDDQLP", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_dcdc_3_on, (unsigned long)pmic_dcdc_3_off},
	 },

	{"VQPS-1.8V", NODE_TYPE_ENABLE, 4000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
	 },

	{"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, 31000,
	 {(unsigned long)sys_rst_deassert_on,
	  (unsigned long)sys_rst_deassert_off},
	 },

	{"ACK-DDR", NODE_TYPE_ENABLE, 29000,
	 {(unsigned long)DDR_PWR_GOOD_PORT, (unsigned long)DDR_PWR_GOOD_PIN},
	 },

	{"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
	 },

};

/* AUTO GENERATED CODE END */
