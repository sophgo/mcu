/*
 * bm_common_funcs.c
 *
 *  Created on: 2019Äê11ÔÂ15ÈÕ
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
#include "i2c_regs.h"
#include "upgrade.h"
#include "gpio.h"

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

static inline int pcie_reset_state(void);

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
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, val, 2, 1000);// LDO1V_IN
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v

	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, val, 1, 1000);// 1.2v
}

#define GPIO_RESET(gpio)	HAL_GPIO_WritePin(gpio##_GPIO_Port,gpio##_Pin, GPIO_PIN_RESET)
#define GPIO_SET(gpio)	HAL_GPIO_WritePin(gpio##_GPIO_Port,gpio##_Pin, GPIO_PIN_SET)
#define GPIO_GET(gpio)	HAL_GPIO_ReadPin(gpio##_GPIO_Port,gpio##_Pin)

void init_pmic(void)
{
	uint8_t val[1];
	val[0] = 0x80;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, val, 1, 1000);// 1.2v

	val[0] = 0x80;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_CFG3, 1, val, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_CFG3, 1, val, 1, 1000);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_CFG3, 1, val, 1, 1000);

	val[0] = 0xE5;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, val, 1, 1000);//enable buck1\2\3
}

void detect_mode(void)
{
	uint8_t i = 0;
	uint8_t pcie_mode = 0;

	// 1 soc 2 PCIE
	for (i = 0; i <  5; i++) {
		if (pcie_reset_state() == 0)
			pcie_mode++;
	}

	if (pcie_mode > 3) {
		i2c_regs.mode_flag = 2;
	} else {
		i2c_regs.mode_flag = 1;
	}
}

uint8_t pmic_voltage_set(uint16_t MemAddress, uint8_t *pData)
{
	uint8_t value[2];
	uint8_t origin_val[2];
	uint8_t chk_val[2];

	origin_val[0] = value[0] = *pData;
	origin_val[1] = value[1] = 0;

	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, MemAddress,1, value, 2, 1000);// LDO1V_IN 0.8V
	HAL_I2C_Mem_Read(&hi2c2,PMIC_ADDR, MemAddress,1, (uint8_t *)chk_val, 2, 1000);// LDO1V_IN 0.8V
	if (chk_val != origin_val)
		HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, MemAddress,1, value, 2, 1000);// LDO1V_IN 0.8V
	HAL_Delay(1);

	return 0;
}

void PowerON(void)
{
	uint8_t val[2];

	val[1] = 0;

	clean_pmic();
	HAL_Delay(50);

	init_pmic();
	HAL_Delay(30);

	GPIO_SET(EN_VDD_1V8);
	HAL_Delay(1);
	GPIO_SET(EN_VDDC);
	HAL_Delay(1);
	GPIO_SET(EN_VDD_3V3);
	HAL_Delay(1);

	detect_mode();
	//enable 0.8v
    GPIO_SET(EN_0P8V);
	HAL_Delay(1);
	GPIO_SET(P08_PG);
	HAL_Delay(1);
	GPIO_SET(PCIE_PG);
	HAL_Delay(1);
	GPIO_SET(EN_VDD_TPU);
	HAL_Delay(1);
	GPIO_SET(TPU_PG);
	HAL_Delay(1);
	//EN_PMIC_OUT3 1.1v
	val[0] = 0x8a;
	pmic_voltage_set(BUCK3_DVS0CFG1, val);
	//EN_PMIC_OUT2 0.6v
	val[0] = 0x4b;
	pmic_voltage_set(BUCK2_DVS0CFG1, val);
	i2c_regs.ddr = 0;
	//EN_PMIC_OUT1 0.75v
	val[0] = 0x5e;
	pmic_voltage_set(BUCK1_DVS0CFG1, val);

	GPIO_SET(TPUMEM_PG);
	HAL_Delay(1);
	while(!GPIO_GET(PWR_GOOD))
		;
	HAL_Delay(30);
	GPIO_SET(SYS_RST_N);
	HAL_Delay(30);
	GPIO_SET(DDR_PG);

	GPIO_SET(SYS_RST_N);
	HAL_Delay(30);
	GPIO_RESET(SYS_RST_N);

	i2c_regs.power_good = 1;
	i2c_regs.cmd_reg  = 0;
}

void PowerDOWN(void)
{
	clean_pmic();
	HAL_Delay(100);

	GPIO_RESET(DDR_PG);
	HAL_Delay(1);
	HAL_Delay(1);
	GPIO_RESET(SYS_RST_N);
	HAL_Delay(1);
	GPIO_RESET(TPUMEM_PG);
	HAL_Delay(1);
	GPIO_RESET(TPU_PG);
	HAL_Delay(1);
	GPIO_RESET(EN_VDD_TPU);
	HAL_Delay(1);
	GPIO_RESET(PCIE_PG);
	HAL_Delay(1);
	GPIO_RESET(P08_PG);
	HAL_Delay(1);
    GPIO_RESET(EN_0P8V);
	HAL_Delay(1);
	GPIO_RESET(EN_VDD_3V3);
	HAL_Delay(1);
	GPIO_RESET(EN_VDDC);
	HAL_Delay(1);
	GPIO_RESET(EN_VDD_1V8);
	HAL_Delay(1);
}

void BM1684_RST(void)
{
//	Convert_sysrst_gpio(0);

	GPIO_RESET(SYS_RST_N);
	HAL_Delay(30);
	GPIO_SET(SYS_RST_N);

//	Convert_sysrst_gpio(1);

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

	return ;
}

#define ADC_NUM		(3)
#define ADC_TIMEOUT (1)

void Scan_Cuerrent(void)
{
	  uint8_t i;
	  int ADC_Buf[ADC_NUM];
	  CURRENT_VAL curr_evb;

	  for (i = 0; i < ADC_NUM; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, ADC_TIMEOUT);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  //calculate voltage
	  curr_evb.i_12v_atx 		= ADC_Buf[1];
	  curr_evb.i_vddio33 		= ADC_Buf[2];

	  memcpy(&i2c_regs.current, &curr_evb, sizeof(CURRENT_VAL));

//	  EEPROM_Write(addr, ADC_Buf[0]);

	  HAL_ADC_Stop(&hadc);
}

void Set_HW_Ver(void)
{
	  uint8_t i;
	  int ADC_Buf[ADC_NUM];

	  for (i = 0; i < ADC_NUM; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, ADC_TIMEOUT);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  switch(ADC_Buf[0]) {
	  case 1900 ... 2200:
	  	  i2c_regs.hw_ver = 0;
	  	  break;
	  case 2500 ... 2900:
	  	  i2c_regs.hw_ver = 1;
	  	  break;
	  case 3000 ... 3500:
	  	  i2c_regs.hw_ver = 2;
	  	  break;
	  case 3600 ... 3900:
	  	  i2c_regs.hw_ver = 3;
	  	  break;
	  }

	  HAL_ADC_Stop(&hadc);
}

void Convert_sysrst_gpio(int io)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  if (io == 1) {//input
		  /*Configure GPIO pin : PtPin */
		  GPIO_InitStruct.Pin = SYS_RST_N_Pin;
		  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		  GPIO_InitStruct.Pull = GPIO_PULLUP;
		  HAL_GPIO_Init(SYS_RST_N_GPIO_Port, &GPIO_InitStruct);
	  } else if (io == 0) {
		  /*Configure GPIO pin Output Level */
		    HAL_GPIO_WritePin(GPIOC, SYS_RST_N_Pin, GPIO_PIN_RESET);
		  /*Configure GPIO pins : PCPin PCPin PCPin */
		    GPIO_InitStruct.Pin = SYS_RST_N_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		    GPIO_InitStruct.Pull = GPIO_NOPULL;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	  }
}

void clean_update_flag(void)
{
	uint8_t Buffer;

	EEPROM_ReadBytes(UPDATE_FLAG_OFFSET, &Buffer, 1);

	if (Buffer == 8) {
		EEPROM_WriteBytes(UPDATE_FLAG_OFFSET, 0x0, 1);
	}
}

static inline int pcie_reset_state(void)
{
	return (GPIO_PIN_SET == GPIO_GET(PCIE_RST_X)) ? 1 : 0;
}

void poll_pcie_rst(void)
{
	if (pcie_reset_state() == 0) {
		GPIO_RESET(SYS_RST_N);
		HAL_Delay(30);
		while (pcie_reset_state() == 0)
			  ;
		GPIO_SET(SYS_RST_N);
	}
}

void Set_Addr(void)
{
	i2c_regs.mcu_addr = (GPIO_GET(MCU_ADDR1) << 1) | (GPIO_GET(MCU_ADDR0));
}

uint8_t Get_Addr(void)
{
	return i2c_regs.mcu_addr;
}

void config_regs(void)
{
	i2c_regs.vender = VENDER_SC5_H3;
	i2c_regs.sw_ver = MCU_VERSION;

	Set_HW_Ver();

	Set_Addr();
}

void module_init(void)
{
	i2c_init(hi2c1.Instance);
	/* EVB board has a real ds1307, donot enable this pseudo one */
	/* SC5 board has no ds1307 */
	ds1307_init();
	mcu_init();
//	wdt_init();
	eeprom_init();
	i2c_slave_start();

	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

	config_regs();

	/* reconfig pa13 and pa6 according to mcu position */
	if (Get_Addr() == 0) {
		/* first chip */
	} else {
		/* not first chip */
		/* add pull-up, default is invalid (low level valid) */
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = PCIE_RST_X_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}

	PowerON();

	//CHANGE SYS_RST FROM OUTPUT TO INPUT
//	Convert_sysrst_gpio(1);

//	clean_update_flag();
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
