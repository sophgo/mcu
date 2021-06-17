/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int pmic_aldo2_on(void);
void pmic_aldo2_off(void);

int pmic_channel_6_on(void);
void pmic_channel_6_off(void);

int pmic_channel_5_on(void);
void pmic_channel_5_off(void);

int pmic_channel_1234_on(void);
void pmic_channel_1234_off(void);

int pmic_aldo1_on(void);
void pmic_aldo1_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node const board_power_nodes[18] = {

	{"VDD-3.3V-CLOCK", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)pmic_aldo2_on, (unsigned long)pmic_aldo2_off},
	 },

	{"VDD-1.8V", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_6_on, (unsigned long)pmic_channel_6_off},
	 },

	{"VDD-CORE", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"CHECK-VDD-CORE", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDDC_PORT, (unsigned long)PG_VDDC_PIN},
	 },

	{"DDR-VDDQ", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_5_on, (unsigned long)pmic_channel_5_off},
	 },

	{"VDD-QLP-3.3V-0.8V", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)pmic_channel_1234_on,
	  (unsigned long)pmic_channel_1234_off},
	 },

	{"ACK-P08", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PWR_GOOD_PORT, (unsigned long)P08_PWR_GOOD_PIN},
	 },

	{"ACK-PCIE", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)GPIO2_PORT, (unsigned long)GPIO2_PIN},
	 },

	{"VDD-TPU", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_PORT, (unsigned long)EN_VDD_TPU_PIN},
	 },

	{"CHECK-VDD-TPU", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDD_TPU_PORT, (unsigned long)PG_VDD_TPU_PIN},
	 },

	{"ACK-TPU", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)GPIO3_PORT, (unsigned long)GPIO3_PIN},
	 },

	{"VDD-TPU-MEM", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_TPU_MEM_PORT,
	  (unsigned long)EN_VDD_TPU_MEM_PIN},
	 },

	{"CHECK-TPU-MEM", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDD_TPU_MEM_PORT,
	  (unsigned long)PG_VDD_TPU_MEM_PIN},
	 },

	{"ACK-TPU_MEM", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)GPIO1_PORT, (unsigned long)GPIO1_PIN},
	 },

	{"VQPS-1.8V", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)pmic_aldo1_on, (unsigned long)pmic_aldo1_off},
	 },

	{"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, 1000,
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
