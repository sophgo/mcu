/*
 * bm_common_funcs.c
 *
 *  Created on: 2019��11��15��
 *      Author: taiqiang.cao
 */
#include "bm_common_func.h"

#include "i2c_bm.h"
#include "mcu.h"
#include "wdt.h"
#include "ds1307.h"
#include "soc_eeprom.h"
#include "eeprom.h"
#include "stdlib.h"
#include "string.h"
#include "upgrade.h"

#define PMIC_ADDR 0x3c

#define IO_MODECTRL			0x24
#define BUCK1_VOUTFBDIV		0x3B
#define BUCK1_CFG3			0x3f
#define BUCK1_DVS0CFG1		0x48
#define BUCK1_DVS0CFG0		0x49
#define BUCK1_DVSSEL		0x53
#define BUCK1_EN_DLY		0x56
#define BUCK2_VOUTFBDIV		0x58
#define BUCK2_CFG3			0x5c
#define BUCK2_DVS0CFG1		0x62
#define BUCK2_DVS0CFG0		0x63
#define BUCK3_VOUTFBDIV		0x72
#define BUCK3_CFG3			0x76
#define BUCK3_DVS0CFG1		0x7C
#define BUCK3_DVS0CFG0		0x7D
#define BUCK4_VOUTFBDIV		0x8c
#define BUCK4_CFG3			0x90
#define BUCK4_DVS0CFG1		0x96
#define BUCK4_DVS0CFG0 		0x97

uint32_t writeFlashData = 0x55aa55aa;
uint8_t readFlashData[4] = {0};
uint32_t addr = 0x08080000;
uint16_t addr_offset = 0x0;

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay specifies the delay time length, in milliseconds.
  * @retval None
  */
//#define HAL_MAX_DELAY_BM      0x00007FFFU

void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a period to guaranty minimum wait */
//  if (wait < HAL_MAX_DELAY_BM)
//  {
//    wait++;
//  }

  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}

void clean_pmic(void)
{
	uint8_t val[2];

	val[0] = val[1] = 0x00;

	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, val, 2, 1000);// LDO1V_IN
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDD_TPU_MEM 0.7v

	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, val, 1, 1000);// 1.2v
}

void PowerON(void)
{
	uint8_t i = 0;
	uint8_t pcie_mode = 0;

	uint8_t origin_val[2];
	uint8_t chk_val[2];
	uint8_t val[2];

	HAL_GPIO_WritePin(PMIC_EN_GPIO_Port, PMIC_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(5);

	clean_pmic();

	val[0] = 0x80;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_VOUTFBDIV,1, val, 1, 1000);// 1.2v

	val[0] = 0x80;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_CFG3, 1, val, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_CFG3, 1, val, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_CFG3, 1, val, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_CFG3, 1, val, 1, 1000);

	val[0] = 0xF5;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, val, 1, 1000);// 1.2v

	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO18_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	// 1 soc 2 PCIE
	for (i = 0; i <  5; i++) {
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIEE_RST_X_MCU_GPIO_Port, PCIEE_RST_X_MCU_Pin)) {
			pcie_mode++;
		}
	}

	if (pcie_mode > 3) {
		i2c_regs.mode_flag = 2;
	} else {
		i2c_regs.mode_flag = 1;
	}


	//LDOIN_1V 0.8V
	val[0] = origin_val[0] = 0x6c;
	val[1] = origin_val[1] = 0x00;
	chk_val[0] = chk_val[1] = 0xff;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, val, 2, 1000);// LDO1V_IN 0.8V
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, chk_val, 2, 1000);// LDO1V_IN 0.8V
	if (chk_val[0] != origin_val[0] || chk_val[1] != origin_val[1])
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, val, 2, 1000);// LDO1V_IN 0.8V
	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO2_Pin, GPIO_PIN_SET);//EN_PHY
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	//DDR_VDDQ
	val[0] = origin_val[0] = 0x8a;
	val[1] = origin_val[1] = 0x00;
	chk_val[0] = chk_val[1] = 0xff;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, (uint8_t *)chk_val, 2, 1000);//DDR_VDDQ 1.1v
	if (chk_val[0] != origin_val[0] || chk_val[1] != origin_val[1])
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_Delay(1);

	//DDR_VDDQLP
#if 1
	val[0] = origin_val[0] = 0x4b; //1.8ms
	val[1] = origin_val[1] = 0x00;
	chk_val[0] = chk_val[1] = 0xff;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, (uint8_t *)&chk_val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	if (chk_val[0] != origin_val[0] || chk_val[1] != origin_val[1])
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	i2c_regs.ddr = 0;
#else
	val[0] = origin_val[0] = 0x89;//1.8ms
	val[1] = origin_val[1] = 0x00;
	chk_val[0] = chk_val[1] = 0xff;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, (uint8_t *)chk_val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
	if (chk_val[0] != origin_val[0] || chk_val[1] != origin_val[1])
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
	i2c_regs.ddr = 1;
#endif

	//VDD_TPU_MEM
	HAL_Delay(1);
	val[0] = origin_val[0] = 0x5e;
	val[1] = origin_val[1] = 0x00;
	chk_val[0] = chk_val[1] = 0xff;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_DVS0CFG1,1, val, 2, 1000);//VDD_TPU_MEM 0.7v
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, BUCK4_DVS0CFG1,1, (uint8_t *)chk_val, 2, 1000);//VDD_TPU_MEM 0.7v
	if (chk_val[0] != origin_val[0] || chk_val[1] != origin_val[1])
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK4_DVS0CFG1,1, val, 2, 1000);//DDR_VDDQ 0.7v
	HAL_Delay(1);

	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//2.7ms
	HAL_GPIO_WritePin(GPIOA, EN_VQPS18_Pin, GPIO_PIN_SET);
	//delay 30ms until power good and can detect SYS_RST signal
	HAL_Delay(30);

	HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIEE_RST_X_MCU_GPIO_Port, PCIEE_RST_X_MCU_Pin))
			;
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

	i2c_regs.power_good = 1;
	i2c_regs.cmd_reg  = 0;
}

void PowerDOWN(void)
{
	clean_pmic();
	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, EN_VQPS18_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PCIE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PHY_Pin, GPIO_PIN_RESET);//EN_PHY
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO18_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);

	i2c_regs.cmd_reg  = 0;

	wdt_reset();
}

void BM1684_RST(void)
{
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	wdt_reset();
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

	i2c_regs.cmd_reg  = 0;

	return ;
}

void BM1684_REBOOT(void)
{
	PowerDOWN();
	HAL_Delay(5);
	PowerON();
}

void Clean_ERR_INT(void)
{
	i2c_regs.cmd_reg  = 0;

	return ;
}

#define FILTER_DATA_WIDTH	16
#define FILTER_DEPTH_SHIFT	8
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

#if FILTER_DATA_WIDTH == 16
typedef uint16_t filter_data_t;
#else
#error undefined filter width or unsupported filter width
#endif

struct filter {
	filter_data_t data[FILTER_DEPTH];
	unsigned long total;
	int p;
};

static filter_data_t filter_init(struct filter *f, filter_data_t d)
{
	int i;

	f->p = 0;
	f->total = 0;
	for (i = 0; i < FILTER_DEPTH; ++i) {
		f->data[i] = d;
		f->total += d;
	}
	return d;
}

static filter_data_t filter_in(struct filter *f, filter_data_t d)
{
	f->total -= f->data[f->p];
	f->total += d;
	f->data[f->p] = d;
	f->p = (f->p + 1) & FILTER_DEPTH_MASK;

	return f->total >> FILTER_DEPTH_SHIFT;
}

static struct filter current_filter;

void Scan_Cuerrent(void)
{
	  uint8_t i;
	  int ADC_Buf[10];
	  CURRENT_VAL curr_evb;

	  for (i = 0; i < 10; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 10);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }
#if 0
	  //calculate voltage
	  curr_evb.i_12v_atx 		= (float)ADC_Buf[0] * 1.8 /4096 / 0.6;  //0.18+ A
	  curr_evb.i_vddio5 		= (float)ADC_Buf[1] * 1.8 /4096 / 0.3;
	  curr_evb.i_vddio18 		= (float)ADC_Buf[2] * 1.8 /4096 / 1.5;
	  curr_evb.i_vddio33 		= (float)ADC_Buf[3] * 1.8 /4096 / 3;
	  curr_evb.i_vdd_phy 		= (float)ADC_Buf[4] * 1.8 /4096 / 1.5;
	  curr_evb.i_vdd_pcie 		= (float)ADC_Buf[5] * 1.8 /4096 / 1.5;
	  curr_evb.i_vdd_tpu_mem 	= (float)ADC_Buf[6] * 1.8 /4096 / 0.3;
	  curr_evb.i_ddr_vddq 		= (float)ADC_Buf[7] * 1.8 /4096 / 0.8;
	  curr_evb.i_ddr_vddqlp 	= (float)ADC_Buf[8] * 1.8 /4096 / 3;
//	  curr_evb.i_ldo_pcie 		= (float)ADC_Buf[9] * 2 /4096 / 1.5;
#endif
	  //calculate voltage
	  //0.18+ A
	  curr_evb.i_12v_atx = filter_in(&current_filter, ADC_Buf[0]);
	  curr_evb.i_vddio5 		= ADC_Buf[1];
	  curr_evb.i_vddio18 		= ADC_Buf[2];
	  curr_evb.i_vddio33 		= ADC_Buf[3];
	  curr_evb.i_vdd_phy 		= ADC_Buf[4];
	  curr_evb.i_vdd_pcie 		= ADC_Buf[5];
	  curr_evb.i_vdd_tpu_mem 	= ADC_Buf[6];
	  curr_evb.i_ddr_vddq 		= ADC_Buf[7];
	  curr_evb.i_ddr_vddqlp 	= ADC_Buf[8];
//	  curr_evb.i_ldo_pcie 		= ADC_Buf[9];

	  memcpy(&i2c_regs.current, &curr_evb, sizeof(CURRENT_VAL));

	  HAL_ADC_Stop(&hadc);
}

#define NCT80		(0x50)

#define START_BIT		BIT0
#define INT_Clear		BIT3
#define CONFIG_REG		(0X00)
#define CONVERSION_RATE (0x07)
#define CHANNEL_SEL_REG	(0x08)
#define IN0_READING		(0x20)
#define IN1_READING		(0x21)
#define IN2_READING		(0x22)
#define IN3_READING		(0x23)
#define IN4_READING		(0x24)
#define IN5_READING		(0x25)
#define IN6_READING		(0x26)

void Scan_Voltage(void)
{

	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN0_READING, 1, (uint8_t *)&i2c_regs.v_vddio3v3, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN1_READING, 1, (uint8_t *)&i2c_regs.v_vddio18, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN2_READING, 1, (uint8_t *)&i2c_regs.v_vdd_phy, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN3_READING, 1, (uint8_t *)&i2c_regs.v_vdd_pcie, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN4_READING, 1, (uint8_t *)&i2c_regs.v_vdd_tpu_mem, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN5_READING, 1, (uint8_t *)&i2c_regs.v_ddr_vddq, 2, 1000);
	HAL_I2C_Mem_Read( &hi2c2, NCT80, IN6_READING, 1, (uint8_t *)&i2c_regs.v_ddr_vddqlp, 2, 1000);

	i2c_regs.v_vddio3v3 = i2c_regs.v_vddio3v3 >> 6;
	i2c_regs.v_vddio18 = i2c_regs.v_vddio18 >> 6;
	i2c_regs.v_vdd_phy = i2c_regs.v_vdd_phy >> 6;
	i2c_regs.v_vdd_pcie = i2c_regs.v_vdd_pcie >> 6;
	i2c_regs.v_vdd_tpu_mem = i2c_regs.v_vdd_tpu_mem >> 6;
	i2c_regs.v_ddr_vddq = i2c_regs.v_ddr_vddq >> 6;
	i2c_regs.v_ddr_vddqlp = i2c_regs.v_ddr_vddqlp >> 6;

	return;
}

void Set_HW_Ver(void)
{
	  uint8_t i;
	  int ADC_Buf[10];

	  for (i = 0; i < 10; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 10);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  switch(ADC_Buf[9]) {
	  case 100 ... 400:
	  	  i2c_regs.hw_ver = 0;
	  	  break;
	  case 500 ... 900:
	  	  i2c_regs.hw_ver = 1;
	  	  break;
	  case 1000 ... 1300:
	  	  i2c_regs.hw_ver = 2;
	  	  break;
	  case 1400 ... 1900:
	  	  i2c_regs.hw_ver = 3;
	  	  break;
	  case 1950 ... 2200:
	  	  i2c_regs.hw_ver = 4;
  	  	  break;
	  case 2250 ... 2700:
	  	  i2c_regs.hw_ver = 5;
	  	  break;
	  case 2750 ... 3000:
	  	  i2c_regs.hw_ver = 6;
	  	  break;
	  case 3100 ... 3600:
	  	  i2c_regs.hw_ver = 7;
	  	  break;
	  case 3650 ... 4000:
	  	  i2c_regs.hw_ver = 8;
	  	  break;
	  }


	  HAL_ADC_Stop(&hadc);

	  filter_init(&current_filter, ADC_Buf[0]);
}

void Factory_Info_get(void)
{
	  EEPROM_ReadBytes(addr_offset, (void *)&(fty_Info.board_type), sizeof(Factory_Info));
}

void led_all_on(void)
{
	HAL_GPIO_WritePin(STATUS_LED0_GPIO_Port, STATUS_LED0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STATUS_LED1_GPIO_Port, STATUS_LED1_Pin, GPIO_PIN_SET);
}

void nct80_init(void)
{
	uint8_t val = 0;

	val = 0x1;
	HAL_I2C_Mem_Write(&hi2c2, NCT80, CONFIG_REG, 1, &val, 1, 1000);
}

void config_regs(void)
{
	i2c_regs.vender = 0;
	i2c_regs.sw_ver = MCU_VERSION;

	Set_HW_Ver();
}

void module_init(void)
{
	i2c_init(hi2c1.Instance);
	/* EVB board has a real ds1307, donot enable this pseudo one */
	// ds1307_init();
	mcu_init();
	wdt_init();
	eeprom_init();
	nct80_init();
	i2c_slave_start();

	led_all_on();

	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

	config_regs();
}

void cmd_process(void)
{
	switch(i2c_regs.cmd_reg) {
	  case CMD_CPLD_PWR_ON:
		  PowerON();
		  break;
	  case CMD_CPLD_PWR_DOWN:
		  PowerDOWN();
		  break;
	  case CMD_CPLD_1684RST:
		  BM1684_RST();
		  i2c_regs.rst_1684_times++;
		  break;
	  case CMD_CPLD_SWRST:
		  BM1684_RST();
		  break;
	  case CMD_CPLD_CLR:
		  Clean_ERR_INT();
		  break;
	  case CMD_BM1684_RST:
		  BM1684_RST();
		  break;
	  case CMD_BM1684_REBOOT:
		  BM1684_REBOOT();
		  break;
	  case CMD_MCU_UPDATE:
		  /* upgrade_start is no return */
		  upgrade_start();
		  break;
	  default:
		  break;
	  }
}

static volatile int is_chip_ready = 1;
static volatile unsigned long timer_start_tick;
static volatile unsigned long timer_timeout;

static inline void timer_start(unsigned long timeout)
{
	__disable_irq();
	timer_timeout = timeout;
	timer_start_tick = HAL_GetTick();
	__enable_irq();
}

static inline int timer_is_timeout(void)
{
	int is_timeout;
	if (timer_timeout) {
		is_timeout = HAL_GetTick() - timer_start_tick >= timer_timeout;
        if (is_timeout)
            timer_timeout = 0;
	} else {
		is_timeout = 1;
    }
	return is_timeout;
}

static inline void led_set(unsigned int led, unsigned int on)
{
	const struct {
		GPIO_TypeDef *port;
		uint16_t pin;
	} led_table[] = {
		{STATUS_LED0_GPIO_Port, STATUS_LED0_Pin},
		{STATUS_LED1_GPIO_Port, STATUS_LED1_Pin},
	};

	int state = on ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(led_table[led].port,
			  led_table[led].pin,
			  state);
}

void poll_pcie_rst(void)
{
	GPIO_PinState prst = HAL_GPIO_ReadPin(PCIEE_RST_X_MCU_GPIO_Port,
					      PCIEE_RST_X_MCU_Pin);

	led_set(0, prst);

	__disable_irq();
	if (is_chip_ready) {
		if (GPIO_PIN_SET == prst) {
			HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin,
					  GPIO_PIN_SET);
		}
	} else {
		is_chip_ready = timer_is_timeout();
	}
	__enable_irq();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	timer_start(30);
	is_chip_ready = 0;
	/* change 1684 working mode to pcie */
	i2c_regs.mode_flag = 2;
	wdt_reset();
}
