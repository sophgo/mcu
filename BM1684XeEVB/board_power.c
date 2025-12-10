/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int check_pcie_reset_on(void);
void check_pcie_reset_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node board_power_nodes[] = {

	{"EN_12V_SYS", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_12V_SYS_PORT, (unsigned long)EN_12V_SYS_PIN},
	 },

	{"VDDIO18", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"VDDC", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"VDDIO33", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"VDD_PHY", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDD_PHY_PORT, (unsigned long)EN_VDD_PHY_PIN},
	 },

	{"P08_PWR_GOOD", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)P08_PWR_GOOD_PORT, (unsigned long)P08_PWR_GOOD_PIN},
	 },

	{"PCIE_PG", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)GPIO2_MCU_PORT, (unsigned long)GPIO2_MCU_PIN},
	 },

	{"VDD_TPU", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDD_TPU_PORT, (unsigned long)EN_VDD_TPU_PIN},
	 },

	{"TPU_PG", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)GPIO3_MCU_PORT, (unsigned long)GPIO3_MCU_PIN},
	 },

	{"VDDQ", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDQ_PORT, (unsigned long)EN_VDDQ_PIN},
	 },

	{"VDDQLP", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VDDQLP_PORT, (unsigned long)EN_VDDQLP_PIN},
	 },

	{"TPU_MEM_PG", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)TPU_MEM_PG_PORT, (unsigned long)TPU_MEM_PG_PIN},
	 },

	{"VQPS-1.8V", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 1000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
	 },

	{"CHECK-PCIE-RESET", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
	 {(unsigned long)check_pcie_reset_on,
	  (unsigned long)check_pcie_reset_off},
	 },

	{"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 30000,
	 {(unsigned long)sys_rst_deassert_on,
	  (unsigned long)sys_rst_deassert_off},
	 },

	{"DDR_PG", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 5000,
	 {(unsigned long)DDR_PWR_GOOD_PORT, (unsigned long)DDR_PWR_GOOD_PIN},
	 },

	{"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 30000,
	 {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
	 },

};

/* AUTO GENERATED CODE END */
