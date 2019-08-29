/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "lptim.h"
#include "rtc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_bm.h"
#include "mcu.h"
#include "wdt.h"
#include "ds1307.h"
#include "tmp451.h"
#include "soc_eeprom.h"
#include "eeprom.h"
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
I2C_REGS i2c_regs;
I2C_CTX i2c_ctx0;
I2C_CTX i2c_ctx3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
#define HAL_MAX_DELAY_BM      0x00007FFFU

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

void TurnOnLED(void);
void TurnOffLED(void);
void PowerON(void);
void PowerDOWN(void);
void BM1684_RST(void);
void Clean_ERR_INT(void);

#define PMIC_ADDR 0x3c

#define IO_MODECTRL			0x24
#define BUCK1_VOUTFBDIV		0x3B
#define BUCK1_DVS0CFG1		0x48
#define BUCK1_DVS0CFG0		0x49
#define BUCK1_DVSSEL		0x53
#define BUCK1_EN_DLY		0x56
#define BUCK2_VOUTFBDIV		0x58
#define BUCK2_DVS0CFG1		0x62
#define BUCK2_DVS0CFG0		0x63
#define BUCK3_VOUTFBDIV		0x72
#define BUCK3_DVS0CFG1		0x7C
#define BUCK3_DVS0CFG0		0x7D
#define BUCK4_VOUTFBDIV		0x8c
#define BUCK4_DVS0CFG1		0x96
#define BUCK4_DVS0CFG0 		0x97

uint8_t val;
volatile uint8_t power_on_good;
void led_on(void);
void led_filcker(void);
void clean_pmic(void);

void PowerON(void)
{
	clean_pmic();
	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOH, EN_5V_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOH, EN_3P3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	val = 0xE5;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, &val, 1, 1000);// 1.2v
	val = 0;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	val = 0xD0;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, &val, 2, 1000);// LDO1V_IN
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO18_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOB, TPU_I2C_ADD3_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PHY_Pin, GPIO_PIN_SET);//EN_PHY
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PCIE_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO2_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	val = 0xE5;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, &val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_Delay(1);
#if 0  //DDR4
	val = 0xE5;//1.8ms
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
	i2c_regs.ddr = 1;
#else //ddr4x
	val = 0x7D;//1.8ms
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	i2c_regs.ddr = 0;
#endif
	HAL_Delay(1);//5ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VQPS18_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
//#ifndef PCBV1
//	if (HAL_GPIO_ReadPin(PG_CORE_GPIO_Port, PG_CORE_Pin) == GPIO_PIN_SET) {
//		i2c_regs.power_good = 1;
//		power_on_good = 1;
//	} else {
//		i2c_regs.power_good = 0;
//		i2c_regs.intr_status1 |= POWERON_ERR;
//		PowerDOWN();
//		goto poweron_fail;
//	}
//#endif
	if (i2c_regs.vender != VENDER_SA5) {
		i2c_regs.power_good = 1;
		power_on_good = 1;
	}
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_SET);

poweron_fail:
	i2c_regs.cmd_reg = 0;
	return;
}

void clean_pmic(void)
{
	val = 0;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, &val, 1, 1000);// 1.2v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, &val, 2, 1000);// LDO1V_IN
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, &val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
}

void PowerDOWN(void)
{
	clean_pmic();
	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, EN_VQPS18_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PCIE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PHY_Pin, GPIO_PIN_RESET);//EN_PHY
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO18_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, &val, 1, 1000);// LDO1V_IN
	HAL_Delay(40);
	HAL_GPIO_WritePin(GPIOB, EN_PMIC_Pin, GPIO_PIN_RESET);// EN_PMIC 1->0
	HAL_Delay(3);
	HAL_GPIO_WritePin(GPIOH, EN_3P3_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOH, EN_5V_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);

	led_on();
}

void BM1684_RST(void)
{
	i2c_regs.intr_status1 |= RESET_OP;

	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

	return ;
}
void Clean_ERR_INT(void)
{
	return ;
}

uint8_t pg_core = 0;

uint8_t sec_count = 0;

void led_filcker(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(500);
}

void led_on(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_SET);
}

void led_off(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_RESET);
}
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
  switch (GPIO_Pin)
  {
  case GPIO_PIN_5:
	  // tempture too high
	  led_filcker();
	  i2c_regs.intr_status1 = 0x02;
	  break;
  case GPIO_PIN_14:
	  // reset 1684 pcie
	  break;
  case GPIO_PIN_15:
	  // power good
	  pg_core = 1;
	  break;
  }
}

//TODO: receive data from bm1684 or CPLD
__weak void i2c_cb(void)
{
	//set i2c_regs paramer
//	i2c_regs.cmd_reg = data;
}

uint8_t ADC_PCB_Ver0;
uint8_t ADC_PCB_Ver1;
uint8_t ADC_BOM_Ver0;
uint8_t ADC_BOM_Ver1;
uint32_t ADC_Buf[5];

/* 0\1  0   0.5   1   1.5
 * 0    0   4     8   12
 * 0.5  1   5     9   13
 * 1    2   6     10  14
 * 1.5  3   7     11  15
 */

uint8_t hw_version[4][4] = { \
		{0, 4, 8, 12}, \
		{1, 5, 9, 13}, \
		{2, 6, 10, 14}, \
		{3, 7, 11, 15}  \
};

void SET_HW_Ver(void)
{
	  uint8_t i;

	  for (i = 0; i < 5; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 0XFFFF);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  //calculate voltage
	  ADC_PCB_Ver0 = (float)ADC_Buf[0] /4096 * 2;
	  ADC_PCB_Ver1 = (float)ADC_Buf[1] /4096 * 2;
	  ADC_BOM_Ver0 = (float)ADC_Buf[2] /4096 * 2;
	  ADC_BOM_Ver1 = (float)ADC_Buf[3] /4096 * 2;

	  if (ADC_PCB_Ver0 < 0.3) {
		  ADC_PCB_Ver0 = 0;
	  } else if ((ADC_PCB_Ver0 >= 0.3) && (ADC_PCB_Ver0 < 0.8)) {
		  ADC_PCB_Ver0 = 0.5;
	  } else if ((ADC_PCB_Ver0 >= 0.8) && (ADC_PCB_Ver0 < 1.3)){
		  ADC_PCB_Ver0 = 1;
	  } else {
		  ADC_PCB_Ver0 = 1.5;
	  }

	  if (ADC_PCB_Ver1 < 0.3) {
		  ADC_PCB_Ver1 = 0;
	  } else if ((ADC_PCB_Ver1 >= 0.3) && (ADC_PCB_Ver1 < 0.8)) {
	      ADC_PCB_Ver1 = 0.5;
	  } else if ((ADC_PCB_Ver1 >= 0.8) && (ADC_PCB_Ver1 < 1.3)){
	  	  ADC_PCB_Ver1 = 1;
	  } else {
	  	  ADC_PCB_Ver1 = 1.5;
	  }

	  if (ADC_BOM_Ver0 < 0.3) {
		  ADC_BOM_Ver0 = 0;
	  } else if ((ADC_BOM_Ver0 >= 0.3) && (ADC_BOM_Ver0 < 0.8)) {
		  ADC_BOM_Ver0 = 0.5;
	  } else if ((ADC_BOM_Ver0 >= 0.8) && (ADC_BOM_Ver0 < 1.3)){
		  ADC_BOM_Ver0 = 1;
	  } else {
		  ADC_BOM_Ver0 = 1.5;
	  }

	  if (ADC_BOM_Ver1 < 0.3) {
		  ADC_BOM_Ver1 = 0;
	  } else if ((ADC_BOM_Ver1 >= 0.3) && (ADC_BOM_Ver1 < 0.8)) {
		  ADC_BOM_Ver1 = 0.5;
	  } else if ((ADC_BOM_Ver1 >= 0.8) && (ADC_BOM_Ver1 < 1.3)){
		  ADC_BOM_Ver1 = 1;
	  } else {
		  ADC_BOM_Ver1 = 1.5;
	  }


	  i2c_regs.hw_ver = (hw_version[ADC_PCB_Ver0 * 2][ADC_PCB_Ver1 * 2] << 4) | hw_version[ADC_BOM_Ver0][ADC_BOM_Ver1];


	  HAL_ADC_Stop(&hadc);
}

#define TMP451_SLAVE_ADDR (0x98)
uint8_t temp1684;	// before calibration
void READ_Temper(void)
{
#ifdef CAL_TEMPARETURE
	float t_remote, terr1, t3;
#endif //CAL_TEMPARETURE

	if (sec_count == 1) {
		// detection of temperature value
		HAL_I2C_Mem_Read(&hi2c2,TMP451_SLAVE_ADDR, 0x1,1, &temp1684, 1, 1000);
#ifdef CAL_TEMPARETURE
		HAL_I2C_Mem_Read(&hi2c2,TMP451_SLAVE_ADDR, 0x0,1, &i2c_regs.temp_board, 1, 1000);
		//1 t_remote = t_register - 6.947;
		t_remote = temp1684 - 6.947;
		//2 Terr1 = ((n-1.008)/1.008) *(273.15 + T1); n = 1.11, T1 = 25
		terr1 = ((1.11 - 1.008)/1.008)*(273.15 + 25);
		//3 T3=((1.008*(Ttremote + Terr1) - (1.11 -1.008) * 273.15)/1.11);
		t3 = (1.008*(t_remote + terr1) - (1.11 - 1.008)*273.15)/1.11;

		i2c_regs.temp1684 = (uint8_t)t3;

#else
		i2c_regs.temp1684 = temp1684 - 7; //rough handling of tempareture calibration
		HAL_I2C_Mem_Read(&hi2c2,TMP451_SLAVE_ADDR, 0x0,1, (uint8_t*)&i2c_regs.temp_board, 1, 1000);
#endif //CAL_TEMPARETURE

		if ((i2c_regs.temp1684 > 75) || (i2c_regs.temp_board > 70)) {//temperature too high alert
			led_on();
			i2c_regs.intr_status1 |= BOARD_OVER_TEMP;
		} else if ((i2c_regs.temp1684 > 85) || (i2c_regs.temp_board > 75)) {//temperature too high, powerdown
			i2c_regs.intr_status1 |= BM1684_OVER_TEMP;
			PowerDOWN();
		}
		sec_count = 0;
	} else {
		HAL_Delay(1);
		sec_count = 1;
	}
}

typedef struct factory_info_t {
	uint16_t manufacturer;
	uint16_t board_type;
	uint16_t date;
	uint16_t Serial_number;
	uint8_t  PCB_Ver;
	uint8_t  PCBA_Ver;
	uint8_t  sub_unit_Ver;
	uint16_t MAC_Addr;
}Factory_Info;

Factory_Info fty_Info;

void Factory_Info_Get(void)
{
//	  EEPROM_Write(addr, writeFlashData);
//	  EEPROM_ReadWords(addr_offset, &fty_Info.manufacturer, sizeof(Factory_Info));
}

#define TCA6416A_ADDR 0x40
#define INPUT_PORT0	0X00
#define INPUT_PORT1	0X01
#define OUTPUT_PORT0 0X02
#define OUTPUT_PORT1 0X03
#define POLARITY_INVERSION0 0X04
#define POLARITY_INVERSION1 0X05
#define CONFIGURATION0 0x06
#define CONFIGURATION1 0x07

void TCA6416_init(void)
{
	uint8_t val;
	val = 0xf6;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, CONFIGURATION0,1, &val, 1, 1000);
	HAL_Delay(1);
	val = 0x9;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, OUTPUT_PORT0,1, &val, 1, 1000);
}


void TCA6416_Deinit(void)
{
	uint8_t val;

	HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, INPUT_PORT0,1, &val, 1, 1000);

	if (((val >> 5) & 0x01) == 0)
	{
		HAL_Delay(5000);
		//power off
		val = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, OUTPUT_PORT0,1, &val, 2, 1000);
		val &= ~0x09;
		HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, OUTPUT_PORT0,1, &val, 2, 1000);
	}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t Buffer;
//  uint16_t board_type_addr = BOARD_TYPE;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  EEPROM_ReadBytes(VENDER_Addr, &i2c_regs.vender, 1);

  if (i2c_regs.vender == 0)
	  i2c_regs.vender = VENDER_SA5;

  i2c_regs.sw_ver = MCU_VERSION;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C2_Init();
  if (i2c_regs.vender != VENDER_SA5) {
	  BM_MX_I2C1_Init();
  } else {
	  MX_I2C1_Init();
  }
  MX_I2C3_Init();
  MX_LPTIM1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

//  Factory_Info_Get();
  if (i2c_regs.vender == VENDER_SA5) {
	  i2c_ctx0 = (struct i2c_ctx*)malloc(sizeof(struct i2c_ctx));
  }
  i2c_ctx3 = (struct i2c_ctx*)malloc(sizeof(struct i2c_ctx));
  if (i2c_regs.vender == VENDER_SA5) {
	  memset(i2c_ctx0, 0, sizeof(struct i2c_ctx));
  }
  memset(i2c_ctx3, 0, sizeof(struct i2c_ctx));

  if (i2c_regs.vender == VENDER_SA5) {
	  i2c_init(hi2c1.Instance,i2c_ctx0);
  }
  i2c_init(hi2c3.Instance,i2c_ctx3);
  if (i2c_regs.vender == VENDER_SA5) {
	  ds1307_init();
  }
  tmp451_init();
  mcu_init();
  wdt_init();
  eeprom_init();
  if (i2c_regs.vender == VENDER_SA5) {
	  i2c_slave_start(i2c_ctx0);
  }
  i2c_slave_start(i2c_ctx3);
  // make sure PB6 is high
//  HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOB, TPU_I2C_ADD3_Pin, GPIO_PIN_SET);
  // set PCB & BOM version by voltage value
  SET_HW_Ver();

  if (i2c_regs.vender != VENDER_SA5) {
	  TCA6416_init();
	  PowerON();
  }

  EEPROM_ReadBytes(UPDATE_FLAG_OFFSET, &Buffer, 1);

  if (Buffer == 8) {
	  EEPROM_WriteBytes(UPDATE_FLAG_OFFSET, 0x0, 1);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  // response CPLD's commands
	  switch(i2c_regs.cmd_reg) {
	  case CMD_CPLD_PWR_ON:
		  PowerON();
		  break;
	  case CMD_CPLD_PWR_DOWN:
//		  i2c_regs.cmd_reg = 0;
		  PowerDOWN();
		  break;
	  case CMD_CPLD_1684RST:
//		  i2c_regs.cmd_reg = 0;
		  BM1684_RST();
		  i2c_regs.rst_1684_times++;
		  break;
	  case CMD_CPLD_SWRST:
//		  i2c_regs.cmd_reg = 0;
		  break;
	  case CMD_CPLD_CLR:
//		  i2c_regs.cmd_reg = 0;
		  Clean_ERR_INT();
		  break;
	  case CMD_BM1684_REBOOT:
//		  i2c_regs.cmd_reg = 0;
		  BM1684_RST();
		  break;
	  case CMD_BM1684_RST:
//		  i2c_regs.cmd_reg = 0;
		  BM1684_RST();
		  break;
	  case CMD_MCU_UPDATE:
//		  i2c_regs.cmd_reg = 0;
		  Buffer = 0x08;
		  EEPROM_WriteBytes(UPDATE_FLAG_OFFSET, &Buffer,1);
		  break;
	  default:
//		  i2c_regs.cmd_reg = 0;
		  break;
	  }

	  if ((power_on_good == 1) && (i2c_regs.intr_status1 != BOARD_OVER_TEMP) && (i2c_regs.intr_status1 != BM1684_OVER_TEMP)) {
		  i2c_regs.cmd_reg = 0;
		  led_filcker();
	  }
	  // read temperature every 2 seconds
	  READ_Temper();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
