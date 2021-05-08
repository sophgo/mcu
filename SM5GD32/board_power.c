/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
// #include <common.h>
#include <stdlib.h>

int pmic_channel_a_on(void);
void pmic_channel_a_off(void);

int pmic_channel_b_on(void);
void pmic_channel_b_off(void);

int pmic_channel_c_on(void);
void pmic_channel_c_off(void);

int check_pcie_reset_on(void);
void check_pcie_reset_off(void);

int enable_tpu_mem_on(void);
void enable_tpu_mem_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node const board_power_nodes[25] = {

	{"LDO-1V", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_a_on, (unsigned long)pmic_channel_a_off},
	 },

	{"VDD-1.8V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"CHECK-VDD-IO-1.8V", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDDIO18_PORT, (unsigned long)PG_VDDIO18_PIN},
	 },

	{"VDD-CORE", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"DDR-VDDQ", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_b_on, (unsigned long)pmic_channel_b_off},
	 },

	{"DDR-VDDQLP", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_c_on, (unsigned long)pmic_channel_c_off},
	 },

	{"VDD-3.3V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"CHECK-VDD-IO-3.3V", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDDIO33_PORT, (unsigned long)PG_VDDIO33_PIN},
	 },

	{"CHECK-PCIE-RESET", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)check_pcie_reset_on,
	  (unsigned long)check_pcie_reset_off},
	 },

	{"VDD-0.8V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PHY_PORT, (unsigned long)EN_VDD_PHY_PIN},
	 },

	{"CHECK-VDD-PHY", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDD_PHY_PORT, (unsigned long)PG_VDD_PHY_PIN},
	 },

	{"ACK-P08", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PWR_GOOD_PORT, (unsigned long)P08_PWR_GOOD_PIN},
	 },

	{"VDD-PCIE", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VDD_PCIE_PORT, (unsigned long)EN_VDD_PCIE_PIN},
	 },

	{"CHECK-VDD-PCIE", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDD_PCIE_PORT, (unsigned long)PG_VDD_PCIE_PIN},
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

	{"VDD-TPU-MEM", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)enable_tpu_mem_on, (unsigned long)enable_tpu_mem_off},
	 },

	{"CHECK-TPU-MEM", NODE_TYPE_CHECK, 0,
	 {(unsigned long)PG_VDD_TPU_MEM_PORT,
	  (unsigned long)PG_VDD_TPU_MEM_PIN},
	 },

	{"ACK-TPU_MEM", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)GPIO1_PORT, (unsigned long)GPIO1_PIN},
	 },

	{"VQPS-1.8V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
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
