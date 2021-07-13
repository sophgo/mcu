/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int pmic_init_on(void);
void pmic_init_off(void);

int pmic_channel_a_on(void);
void pmic_channel_a_off(void);

int pmic_channel_d_on(void);
void pmic_channel_d_off(void);

int pmic_channel_b_on(void);
void pmic_channel_b_off(void);

int pmic_channel_c_on(void);
void pmic_channel_c_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node const board_power_nodes[23] = {

	{"PMIC-INIT", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)pmic_init_on, (unsigned long)pmic_init_off},
	 },

	{"PMIC-VDD-1.8V", NODE_TYPE_FUNCTION, 0,
	 {(unsigned long)pmic_channel_a_on, (unsigned long)pmic_channel_a_off},
	 },

	{"VDD-1.8V", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO18_PORT, (unsigned long)EN_VDDIO18_PIN},
	 },

	{"VDD-CORE", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
	 },

	{"VDD-3.3V", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
	 },

	{"VDD-0.8V", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_d_on, (unsigned long)pmic_channel_d_off},
	 },

	{"ACK-PCIE", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)GPIO2_PORT, (unsigned long)GPIO2_PIN},
	 },

	{"ACK-P08", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)P08_PG_PORT, (unsigned long)P08_PG_PIN},
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

	{"DDR-VDDQ", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_b_on, (unsigned long)pmic_channel_b_off},
	 },

	{"DDR-VDDQLP", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)pmic_channel_c_on, (unsigned long)pmic_channel_c_off},
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

	{"VQPS-1.8V", NODE_TYPE_ENABLE, 1000,
	 {(unsigned long)EN_VQPS18_PORT, (unsigned long)EN_VQPS18_PIN},
	 },

	{"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, 1000,
	 {(unsigned long)sys_rst_deassert_on,
	  (unsigned long)sys_rst_deassert_off},
	 },

	{"ACK-DDR", NODE_TYPE_ENABLE, 29000,
	 {(unsigned long)DDR_PG_PORT, (unsigned long)DDR_PG_PIN},
	 },

	{"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, 30000,
	 {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
	 },

	{"USB-5V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)USB_5V_EN_PORT, (unsigned long)USB_5V_EN_PIN},
	 },

	{"ASM3142-3.3V", NODE_TYPE_ENABLE, 10,
	 {(unsigned long)ASM3142_3P3V_EN_PORT,
	  (unsigned long)ASM3142_3P3V_EN_PIN},
	 },

	{"HDMI-1.2V", NODE_TYPE_ENABLE, 0,
	 {(unsigned long)VDD1P2V_EN_PORT, (unsigned long)VDD1P2V_EN_PIN},
	 },

};

/* AUTO GENERATED CODE END */
