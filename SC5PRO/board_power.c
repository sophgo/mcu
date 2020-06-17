/* AUTO GENERATED CODE */

#include <power.h>
#include <pin.h>
#include <common.h>
#include <stdlib.h>

int b12_vdd_0_8v_on(void);
void b12_vdd_0_8v_off(void);
int b34_vdd_0_8v_on(void);
void b34_vdd_0_8v_off(void);
int b56_vdd_0_8v_on(void);
void b56_vdd_0_8v_off(void);
int b78_vdd_0_8v_on(void);
void b78_vdd_0_8v_off(void);

int b12_vp_0_8v_on(void);
void b12_vp_0_8v_off(void);
int b34_vp_0_8v_on(void);
void b34_vp_0_8v_off(void);
int b56_vp_0_8v_on(void);
void b56_vp_0_8v_off(void);
int b78_vp_0_8v_on(void);
void b78_vp_0_8v_off(void);

int b12_ddr_vddq_on(void);
void b12_ddr_vddq_off(void);
int b34_ddr_vddq_on(void);
void b34_ddr_vddq_off(void);
int b56_ddr_vddq_on(void);
void b56_ddr_vddq_off(void);
int b78_ddr_vddq_on(void);
void b78_ddr_vddq_off(void);

int b12_ddr_vddqlp_on(void);
void b12_ddr_vddqlp_off(void);
int b34_ddr_vddqlp_on(void);
void b34_ddr_vddqlp_off(void);
int b56_ddr_vddqlp_on(void);
void b56_ddr_vddqlp_off(void);
int b78_ddr_vddqlp_on(void);
void b78_ddr_vddqlp_off(void);

int sys_rst_deassert_on(void);
void sys_rst_deassert_off(void);

int ddr_power_good_on(void);
void ddr_power_good_off(void);

int sys_rst_assert_on(void);
void sys_rst_assert_off(void);

struct power_node const board_power_nodes[] = {

	{"VDD-1.8V", NODE_TYPE_ENABLE, 100, 600, {
						  {(unsigned long)
						   EN_B12_VDD_1_8V_PORT,
						   (unsigned long)
						   EN_B12_VDD_1_8V_PIN},
						  {(unsigned long)
						   EN_B34_VDD_1_8V_PORT,
						   (unsigned long)
						   EN_B34_VDD_1_8V_PIN},
						  {(unsigned long)
						   EN_B56_VDD_1_8V_PORT,
						   (unsigned long)
						   EN_B56_VDD_1_8V_PIN},
						  {(unsigned long)
						   EN_B78_VDD_1_8V_PORT,
						   (unsigned long)
						   EN_B78_VDD_1_8V_PIN},
						  }},

	{"VDD-CORE", NODE_TYPE_ENABLE, 100, 600, {
						  {(unsigned long)
						   EN_B12_VDDC_PORT,
						   (unsigned long)
						   EN_B12_VDDC_PIN},
						  {(unsigned long)
						   EN_B34_VDDC_PORT,
						   (unsigned long)
						   EN_B34_VDDC_PIN},
						  {(unsigned long)
						   EN_B56_VDDC_PORT,
						   (unsigned long)
						   EN_B56_VDDC_PIN},
						  {(unsigned long)
						   EN_B78_VDDC_PORT,
						   (unsigned long)
						   EN_B78_VDDC_PIN},
						  }},

	{"VDD-3.3V", NODE_TYPE_ENABLE, 100, 600, {
						  {(unsigned long)
						   EN_B12_VDD_3_3V_PORT,
						   (unsigned long)
						   EN_B12_VDD_3_3V_PIN},
						  {(unsigned long)
						   EN_B34_VDD_3_3V_PORT,
						   (unsigned long)
						   EN_B34_VDD_3_3V_PIN},
						  {(unsigned long)
						   EN_B56_VDD_3_3V_PORT,
						   (unsigned long)
						   EN_B56_VDD_3_3V_PIN},
						  {(unsigned long)
						   EN_B78_VDD_3_3V_PORT,
						   (unsigned long)
						   EN_B78_VDD_3_3V_PIN},
						  }},

	{"VDD-0.8V", NODE_TYPE_FUNCTION, 100, 600, {
						    {(unsigned long)
						     b12_vdd_0_8v_on,
						     (unsigned long)
						     b12_vdd_0_8v_off},
						    {(unsigned long)
						     b34_vdd_0_8v_on,
						     (unsigned long)
						     b34_vdd_0_8v_off},
						    {(unsigned long)
						     b56_vdd_0_8v_on,
						     (unsigned long)
						     b56_vdd_0_8v_off},
						    {(unsigned long)
						     b78_vdd_0_8v_on,
						     (unsigned long)
						     b78_vdd_0_8v_off},
						    }},

	{"VP-0.8V", NODE_TYPE_FUNCTION, 100, 600, {
						   {(unsigned long)
						    b12_vp_0_8v_on,
						    (unsigned long)
						    b12_vp_0_8v_off},
						   {(unsigned long)
						    b34_vp_0_8v_on,
						    (unsigned long)
						    b34_vp_0_8v_off},
						   {(unsigned long)
						    b56_vp_0_8v_on,
						    (unsigned long)
						    b56_vp_0_8v_off},
						   {(unsigned long)
						    b78_vp_0_8v_on,
						    (unsigned long)
						    b78_vp_0_8v_off},
						   }},

	{"PG-PCIE-0", NODE_TYPE_ENABLE, 100, 600, {
						   {(unsigned long)
						    B1_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B1_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B3_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B3_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B5_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B5_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B7_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B7_INPUT_PG0_1_PIN},
						   }},

	{"PG-PCIE-1", NODE_TYPE_ENABLE, 100, 600, {
						   {(unsigned long)
						    B2_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B2_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B4_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B4_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B6_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B6_INPUT_PG0_1_PIN},
						   {(unsigned long)
						    B8_INPUT_PG0_1_PORT,
						    (unsigned long)
						    B8_INPUT_PG0_1_PIN},
						   }},

	{"VDD-TPU", NODE_TYPE_ENABLE, 100, 600, {
						 {(unsigned long)
						  EN_B12_VDD_TPU_PORT,
						  (unsigned long)
						  EN_B12_VDD_TPU_PIN},
						 {(unsigned long)
						  EN_B34_VDD_TPU_PORT,
						  (unsigned long)
						  EN_B34_VDD_TPU_PIN},
						 {(unsigned long)
						  EN_B56_VDD_TPU_PORT,
						  (unsigned long)
						  EN_B56_VDD_TPU_PIN},
						 {(unsigned long)
						  EN_B78_VDD_TPU_PORT,
						  (unsigned long)
						  EN_B78_VDD_TPU_PIN},
						 }},

	{"DDR-VDDQ", NODE_TYPE_FUNCTION, 100, 600, {
						    {(unsigned long)
						     b12_ddr_vddq_on,
						     (unsigned long)
						     b12_ddr_vddq_off},
						    {(unsigned long)
						     b34_ddr_vddq_on,
						     (unsigned long)
						     b34_ddr_vddq_off},
						    {(unsigned long)
						     b56_ddr_vddq_on,
						     (unsigned long)
						     b56_ddr_vddq_off},
						    {(unsigned long)
						     b78_ddr_vddq_on,
						     (unsigned long)
						     b78_ddr_vddq_off},
						    }},

	{"DDR-VDDQLP", NODE_TYPE_FUNCTION, 100, 600, {
						      {(unsigned long)
						       b12_ddr_vddqlp_on,
						       (unsigned long)
						       b12_ddr_vddqlp_off},
						      {(unsigned long)
						       b34_ddr_vddqlp_on,
						       (unsigned long)
						       b34_ddr_vddqlp_off},
						      {(unsigned long)
						       b56_ddr_vddqlp_on,
						       (unsigned long)
						       b56_ddr_vddqlp_off},
						      {(unsigned long)
						       b78_ddr_vddqlp_on,
						       (unsigned long)
						       b78_ddr_vddqlp_off},
						      }},

	{"VDD-TMEM", NODE_TYPE_ENABLE, 100, 600, {
						  {(unsigned long)
						   EN_B12_VDD_TMEM_PORT,
						   (unsigned long)
						   EN_B12_VDD_TMEM_PIN},
						  {(unsigned long)
						   EN_B34_VDD_TMEM_PORT,
						   (unsigned long)
						   EN_B34_VDD_TMEM_PIN},
						  {(unsigned long)
						   EN_B56_VDD_TMEM_PORT,
						   (unsigned long)
						   EN_B56_VDD_TMEM_PIN},
						  {(unsigned long)
						   EN_B78_VDD_TMEM_PORT,
						   (unsigned long)
						   EN_B78_VDD_TMEM_PIN},
						  }},

	{"PG-TMEM-0", NODE_TYPE_ENABLE, 100, 600, {
						   {(unsigned long)
						    B1_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B1_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B3_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B3_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B5_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B5_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B7_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B7_INPUT_PG2_3_PIN},
						   }},

	{"PG-TMEM-1", NODE_TYPE_ENABLE, 100, 600, {
						   {(unsigned long)
						    B2_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B2_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B4_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B4_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B6_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B6_INPUT_PG2_3_PIN},
						   {(unsigned long)
						    B8_INPUT_PG2_3_PORT,
						    (unsigned long)
						    B8_INPUT_PG2_3_PIN},
						   }},

	{"VQPS-1.8V", NODE_TYPE_ENABLE, 100, 600, {
						   {(unsigned long)
						    EN_B12_VQPS_1_8V_PORT,
						    (unsigned long)
						    EN_B12_VQPS_1_8V_PIN},
						   {(unsigned long)
						    EN_B34_VQPS_1_8V_PORT,
						    (unsigned long)
						    EN_B34_VQPS_1_8V_PIN},
						   {(unsigned long)
						    EN_B56_VQPS_1_8V_PORT,
						    (unsigned long)
						    EN_B56_VQPS_1_8V_PIN},
						   {(unsigned long)
						    EN_B78_VQPS_1_8V_PORT,
						    (unsigned long)
						    EN_B78_VQPS_1_8V_PIN},
						   }},

	{"SYS-RST-DEASSERT", NODE_TYPE_FUNCTION, 0, 1000, {
							   {(unsigned long)
							    sys_rst_deassert_on,
							    (unsigned long)
							    sys_rst_deassert_off},
							   {(unsigned long)NULL,
							    (unsigned long)
							    NULL},
							   {(unsigned long)NULL,
							    (unsigned long)
							    NULL},
							   {(unsigned long)NULL,
							    (unsigned long)
							    NULL},
							   }},

	{"PG-DDR-0", NODE_TYPE_FUNCTION, 0, 30000, {
						    {(unsigned long)
						     ddr_power_good_on,
						     (unsigned long)
						     ddr_power_good_off},
						    {(unsigned long)NULL,
						     (unsigned long)NULL},
						    {(unsigned long)NULL,
						     (unsigned long)NULL},
						    {(unsigned long)NULL,
						     (unsigned long)NULL},
						    }},

	{"SYS-RST-ASSERT", NODE_TYPE_FUNCTION, 0, 30000, {
							  {(unsigned long)
							   sys_rst_assert_on,
							   (unsigned long)
							   sys_rst_assert_off},
							  {(unsigned long)NULL,
							   (unsigned long)NULL},
							  {(unsigned long)NULL,
							   (unsigned long)NULL},
							  {(unsigned long)NULL,
							   (unsigned long)NULL},
							  }},

};

int board_power_init(void)
{
	return power_on(board_power_nodes, ARRAY_SIZE(board_power_nodes));
}

/* AUTO GENERATED CODE END */
