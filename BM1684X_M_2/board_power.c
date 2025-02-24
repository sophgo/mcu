// * AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int powerchip_init_on(void);
void powerchip_init_off(void);

int pmic_channel_a_on(void);
void pmic_channel_a_off(void);

int pmic_channel_d_on(void);
void pmic_channel_d_off(void);

int pmic_channel_b_on(void);
void pmic_channel_b_off(void);

int pmic_channel_c_on(void);
void pmic_channel_c_off(void);

int check_pcie_reset_on(void);
void check_pcie_reset_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node board_power_nodes[14] = {

    {"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
    {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
    },

    {"PMIC-VDD-1.8V", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 50000,
    {(unsigned long)pmic_channel_d_on, (unsigned long)pmic_channel_d_off},
    },

    {"VDD-CORE", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 2000,
    {(unsigned long)EN_VDDC_PORT, (unsigned long)EN_VDDC_PIN},
    },

    {"VDD-3.3V", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
    {(unsigned long)EN_VDDIO33_PORT, (unsigned long)EN_VDDIO33_PIN},
    },

    {"VDD-0.8V", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 1000,
    {(unsigned long)pmic_channel_a_on, (unsigned long)pmic_channel_a_off},
    },

    {"ACK-P08", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
    {(unsigned long)P08_PWR_GOOD_PORT, (unsigned long)P08_PWR_GOOD_PIN},
    },

    {"VDD-TPU", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
    {(unsigned long)EN_VDD_TPU_PORT, (unsigned long)EN_VDD_TPU_PIN},
    },

    {"ACK-PCIE", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 4000,
    {(unsigned long)GPIO2_PORT, (unsigned long)GPIO2_PIN},
    },
    {"ACK-TPU", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
    {(unsigned long)GPIO3_PORT, (unsigned long)GPIO3_PIN},
    },

    {"DDR-VDDQ", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
    {(unsigned long)pmic_channel_c_on, (unsigned long)pmic_channel_c_off},
    },

    {"DDR-VDDQLP", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
    {(unsigned long)pmic_channel_b_on, (unsigned long)pmic_channel_b_off},
    },

    {"CHECK-PCIE-RESET", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 0,
    {(unsigned long)check_pcie_reset_on, (unsigned long)check_pcie_reset_off},
    },

    {"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 30000,
    {(unsigned long)sys_rst_deassert_on, (unsigned long)sys_rst_deassert_off},
    },

    {"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, POWER_STATUS_OFF, 50000,
    {(unsigned long)sys_rst_assert_on, (unsigned long)sys_rst_assert_off},
    },

    // {"ACK-DDR", NODE_TYPE_ENABLE, POWER_STATUS_OFF, 0,
    // {(unsigned long)DDR_PWR_GOOD_PORT, (unsigned long)DDR_PWR_GOOD_PIN},
    // },

};

/* AUTO GENERATED CODE END */
