/* Sophgo Technology. Created by Donghao on 7/13/23. */


#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>
#include <project.h>
#include <power_node.h>


#define EN_12V0_EVB_DELAY				1000
#define EN_12V_SYS_ACP_DELAY			1000
#define EN_VDD_5V0_DELAY				1000
#define EN_VDDC_0V8_DELAY				68800
#define EN_VDD_0V8_RTC_EVB_DELAY		160
#define EN_VDD_0V8_RTC_ACP_DELAY		160
#define EN_VDD_1V8_RTC_EVB_DELAY		380
#define EN_VDD_1V8_RTC_ACP_DELAY		380
#define EN_VDD_DDR_0V6_DELAY			800
#define EN_VDD_0V8_DELAY				250
#define EN_VDD_1V8_DELAY				300
#define EN_VDD_3V3_DELAY				1700
#define EN_VDD_DDR_1V1_DELAY			0
#define EN_VDD_DDR_1V8_DELAY			650
#define EN_VDD_0V8_PCIE_DELAY			150
#define EN_VDD_1V8_PCIE_DELAY			10


int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);


struct board_power_nodes const board_power_node[POWER_NODE_SUM] = {

	{"EN_12V0_EVB", NODE_TYPE_ENABLE, EN_12V0_EVB_DELAY,
	 		1, {ATHENA2EVB},
			{(unsigned long)EN_12V0_EVB_PORT, (unsigned long)EN_12V0_EVB_PIN},
	},

	{"EN_12V_SYS_ACP", NODE_TYPE_ENABLE, EN_12V_SYS_ACP_DELAY,
			1, {ATHENA2ACP},
			{(unsigned long)EN_12V_SYS_ACP_PORT, (unsigned long)EN_12V_SYS_ACP_PIN},
	},

	{"EN_VDD_5V0", NODE_TYPE_ENABLE, EN_VDD_5V0_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_5V0_PORT, (unsigned long)EN_VDD_5V0_PIN},
	},

	{"EN_VDDC_0V8", NODE_TYPE_ENABLE, EN_VDDC_0V8_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDDC_0V8_PORT, (unsigned long)EN_VDDC_0V8_PIN},
	},

	{"EN_VDD_0V8_RTC_EVB", NODE_TYPE_ENABLE, EN_VDD_0V8_RTC_EVB_DELAY,
			1, {ATHENA2EVB},
			{(unsigned long)EN_VDD_0V8_RTC_EVB_PORT, (unsigned long)EN_VDD_0V8_RTC_EVB_PIN},
	},

	{"EN_VDD_0V8_RTC_ACP", NODE_TYPE_ENABLE, EN_VDD_0V8_RTC_ACP_DELAY,
			1, {ATHENA2ACP},
			{(unsigned long)EN_VDD_0V8_RTC_ACP_PORT, (unsigned long)EN_VDD_0V8_RTC_ACP_PIN},
	},

	{"EN_VDD_1V8_RTC_EVB", NODE_TYPE_ENABLE, EN_VDD_1V8_RTC_EVB_DELAY,
			1, {ATHENA2EVB},
			{(unsigned long)EN_VDD_1V8_RTC_EVB_PORT, (unsigned long)EN_VDD_1V8_RTC_EVB_PIN},
	},

	{"EN_VDD_1V8_RTC_ACP", NODE_TYPE_ENABLE, EN_VDD_1V8_RTC_ACP_DELAY,
			1, {ATHENA2ACP},
			{(unsigned long)EN_VDD_1V8_RTC_ACP_PORT, (unsigned long)EN_VDD_1V8_RTC_ACP_PIN},
	},

	{"EN_VDD_DDR_0V6", NODE_TYPE_ENABLE, EN_VDD_DDR_0V6_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_DDR_0V6_PORT, (unsigned long)EN_VDD_DDR_0V6_PIN},
	},

	{"EN_VDD_0V8", NODE_TYPE_ENABLE, EN_VDD_0V8_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_0V8_PORT, (unsigned long)EN_VDD_0V8_PIN},
	},

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, EN_VDD_1V8_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_1V8_PORT, (unsigned long)EN_VDD_1V8_PIN},
	},

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, EN_VDD_3V3_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_3V3_PORT, (unsigned long)EN_VDD_3V3_PIN},
	},

	{"EN_VDD_DDR_1V1", NODE_TYPE_ENABLE, EN_VDD_DDR_1V1_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_DDR_1V1_PORT, (unsigned long)EN_VDD_DDR_1V1_PIN},
	},

	{"EN_VDD_DDR_1V8", NODE_TYPE_ENABLE, EN_VDD_DDR_1V8_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_DDR_1V8_PORT, (unsigned long)EN_VDD_DDR_1V8_PIN},
	},

	{"EN_VDD_0V8_PCIE", NODE_TYPE_ENABLE, EN_VDD_0V8_PCIE_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_0V8_PCIE_PORT, (unsigned long)EN_VDD_0V8_PCIE_PIN},
	},

	{"EN_VDD_1V8_PCIE", NODE_TYPE_ENABLE, EN_VDD_1V8_PCIE_DELAY,
			2, {ATHENA2EVB, ATHENA2ACP},
			{(unsigned long)EN_VDD_1V8_PCIE_PORT, (unsigned long)EN_VDD_1V8_PCIE_PIN},
	},
};
