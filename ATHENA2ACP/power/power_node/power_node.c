/*
 * power_node.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#include <power/power.h>
#include <pin.h>
#include <common/common.h>
#include <stdlib.h>
#include <project.h>
#include <power/power_node/power_node.h>

#if DDR_TYPE == 1	/* LP_DDR4 */

#define EN_12V_SYS_DELAY		1000
#define EN_VDD_5V0_DELAY		1000
#define EN_VDD_1V8_RTC_DELAY		0
#define EN_VDD_0V8_RTC_DELAY		2000
#define EN_VDDC_0V8_DELAY		0
#define EN_VDD_0V8_DELAY		0
#define EN_VDD_1V8_DELAY		2000
#define EN_VDD_0V8_PCIE_DELAY		0
#define EN_VDD_1V8_PCIE_DELAY		0
#define EN_VDD_3V3_DELAY		2000
#define EN_VDD_DDR_1V1_DELAY		0
#define EN_VDD_DDR_0V6_DELAY		0
#define EN_VDD_DDR_1V8_DELAY		0

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);
int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

struct board_power_nodes const board_power_node[POWER_NODE_SUM] = {

	{"EN_12V_SYS", NODE_TYPE_ENABLE, EN_12V_SYS_DELAY,
		{(unsigned long)EN_12V_SYS_PORT,
		 (unsigned long)EN_12V_SYS_PIN},
	},

	{"EN_VDD_5V0", NODE_TYPE_ENABLE, EN_VDD_5V0_DELAY,
		{(unsigned long)EN_VDD_5V0_PORT,
		 (unsigned long)EN_VDD_5V0_PIN},
	},

	{"EN_VDD_1V8_RTC", NODE_TYPE_ENABLE, EN_VDD_1V8_RTC_DELAY,
		{(unsigned long)EN_VDD_1V8_RTC_PORT,
		 (unsigned long)EN_VDD_1V8_RTC_PIN},
	},

	{"EN_VDD_0V8_RTC", NODE_TYPE_ENABLE, EN_VDD_0V8_RTC_DELAY,
		{(unsigned long)EN_VDD_0V8_RTC_PORT,
		 (unsigned long)EN_VDD_0V8_RTC_PIN},
	},

	{"EN_VDDC_0V8", NODE_TYPE_ENABLE, EN_VDDC_0V8_DELAY,
		{(unsigned long)EN_VDDC_0V8_PORT,
		 (unsigned long)EN_VDDC_0V8_PIN},
	},

	{"EN_VDD_0V8", NODE_TYPE_ENABLE, EN_VDD_0V8_DELAY,
		{(unsigned long)EN_VDD_0V8_PORT,
		 (unsigned long)EN_VDD_0V8_PIN},
	},

	{"EN_VDD_1V8", NODE_TYPE_ENABLE, EN_VDD_1V8_DELAY,
		{(unsigned long)EN_VDD_1V8_PORT,
		 (unsigned long)EN_VDD_1V8_PIN},
	},

	{"EN_VDD_0V8_PCIE", NODE_TYPE_ENABLE, EN_VDD_0V8_PCIE_DELAY,
		{(unsigned long)EN_VDD_0V8_PCIE_PORT,
		 (unsigned long)EN_VDD_0V8_PCIE_PIN},
	},

	{"EN_VDD_1V8_PCIE", NODE_TYPE_ENABLE, EN_VDD_1V8_PCIE_DELAY,
		{(unsigned long)EN_VDD_1V8_PCIE_PORT,
		 (unsigned long)EN_VDD_1V8_PCIE_PIN},
	},

	{"EN_VDD_3V3", NODE_TYPE_ENABLE, EN_VDD_3V3_DELAY,
		{(unsigned long)EN_VDD_3V3_PORT,
		 (unsigned long)EN_VDD_3V3_PIN},
	},

	{"EN_VDD_DDR_1V1", NODE_TYPE_ENABLE, EN_VDD_DDR_1V1_DELAY,
		{(unsigned long)EN_VDD_DDR_1V1_PORT,
		 (unsigned long)EN_VDD_DDR_1V1_PIN},
	},

	{"EN_VDD_DDR_0V6", NODE_TYPE_ENABLE, EN_VDD_DDR_0V6_DELAY,
		{(unsigned long)EN_VDD_DDR_0V6_PORT,
		 (unsigned long)EN_VDD_DDR_0V6_PIN},
	},

	{"EN_VDD_DDR_1V8", NODE_TYPE_ENABLE, EN_VDD_DDR_1V8_DELAY,
		{(unsigned long)EN_VDD_DDR_1V8_PORT,
		 (unsigned long)EN_VDD_DDR_1V8_PIN},
	},

};

#endif /* LP_DDR4 */
