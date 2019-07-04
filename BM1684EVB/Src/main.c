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
#include "eeprom.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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

#define EEPROM_BASE_ADDR	0x08080000
#define EEPROM_BANK_SIZE	0x0BFF
#define EEPROM_BANK1_START	0X08080000
#define EEPROM_BANK1_END	0x08080BFF
#define EEPROM_BANK2_START	0X08080C00
#define EEPROM_BANK2_END	0x080817FF

uint32_t writeFlashData = 0x55aa55aa;
uint8_t readFlashData[4] = {0};
uint32_t addr = 0x08080000;
uint16_t addr_offset = 0x0;

uint8_t val;


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

void PowerON(void)
{
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
	HAL_Delay(1);//  2.3ms
	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//1.6ms
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//2ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PHY_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//4ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_PCIE_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO2_Pin, GPIO_PIN_SET);//EN_PHY
	HAL_Delay(1);//4.4ms
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//4.3ms
	val = 0xE5;
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, &val, 2, 1000);//DDR_VDDQ 1.1v
	HAL_Delay(1);
	val = 0x7D;//1.8ms
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	HAL_Delay(1);//5ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_L_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);//2.7ms
	HAL_GPIO_WritePin(GPIOA, EN_VQPS18_Pin, GPIO_PIN_SET);
	//delay 30ms until power good and can detect SYS_RST signal
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_SET);
}

void PowerDOWN(void)
{
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, EN_VQPS18_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_L_Pin, GPIO_PIN_RESET);
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
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, &val, 2, 1000);// LDO1V_IN
	HAL_Delay(1);
}

void BM1684_RST(void)
{
	Convert_sysrst_gpio(0);

	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

	Convert_sysrst_gpio(1);

	return ;
}

typedef struct CURRENT_VAL_t
{
	float i_12v_atx;
	float i_vddio5;
	float i_vddio18;
	float i_vddio33;
	float i_vdd_phy;
	float i_vdd_pcie;
	float i_vdd_tpu_mem;
	float i_ddr_vddq;
	float i_ddr_vddqlp;
	float i_ldo_pcie;
}CURRENT_VAL;

CURRENT_VAL curr_evb;
int ADC_Buf[10];

void Scan_Cuerrent(void)
{
	  uint8_t i;

	  for (i = 0; i < 10; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 10);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  //calculate voltage
	  curr_evb.i_12v_atx 		= (float)ADC_Buf[0] * 2 /4096 / 0.3;  //0.18+ A
	  curr_evb.i_vddio5 		= (float)ADC_Buf[1] * 2 /4096 / 0.6;
	  curr_evb.i_vddio18 		= (float)ADC_Buf[2] * 2 /4096 / 0.6;
	  curr_evb.i_vddio33 		= (float)ADC_Buf[3] * 2 /4096 / 3;
	  curr_evb.i_vdd_phy 		= (float)ADC_Buf[4] * 2 /4096 / 0.8;
	  curr_evb.i_vdd_pcie 		= (float)ADC_Buf[5] * 2 /4096 / 0.6;
	  curr_evb.i_vdd_tpu_mem 	= (float)ADC_Buf[6] * 2 /4096 / 0.3;
	  curr_evb.i_ddr_vddq 		= (float)ADC_Buf[7] * 2 /4096 / 0.8;
	  curr_evb.i_ddr_vddqlp 	= (float)ADC_Buf[8] * 2 /4096 / 0.5;
	  curr_evb.i_ldo_pcie 		= (float)ADC_Buf[9] * 2 /4096 / 1.5;


//	  EEPROM_Write(addr, ADC_Buf[0]);

	  HAL_ADC_Stop(&hadc);
}

typedef struct factory_info_t {
	uint32_t manufacturer;
	uint32_t board_type;
	uint32_t date;
	uint32_t Serial_number;
	uint8_t  PCB_Ver;
	uint8_t  PCBA_Ver;
	uint8_t  sub_unit_Ver;
	uint32_t MAC_Addr;
}Factory_Info;

Factory_Info fty_Info;

void Factory_Info_get(void)
{
//	  EEPROM_Write(addr, writeFlashData);
	  EEPROM_ReadBytes(addr_offset, &(fty_Info.board_type), sizeof(Factory_Info));
}

void Convert_sysrst_gpio(int io)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  if (io == 1) {//input
		  /*Configure GPIO pin : PtPin */
		  GPIO_InitStruct.Pin = SYS_RST_X_Pin;
		  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		  GPIO_InitStruct.Pull = GPIO_PULLUP;
		  HAL_GPIO_Init(SYS_RST_X_GPIO_Port, &GPIO_InitStruct);
	  } else if (io == 0) {
		  /*Configure GPIO pin Output Level */
		    HAL_GPIO_WritePin(GPIOC, SYS_RST_X_Pin, GPIO_PIN_RESET);
		  /*Configure GPIO pins : PCPin PCPin PCPin */
		    GPIO_InitStruct.Pin = SYS_RST_X_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		    GPIO_InitStruct.Pull = GPIO_NOPULL;
		    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
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

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_LPTIM1_Init();
  MX_RTC_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
  i2c_init(hi2c1.Instance);
  ds1307_init();
  mcu_init();
  wdt_init();
  i2c_slave_start();
  PowerON();
//  Scan_Cuerrent();

  EEPROM_Write(addr, 0x08);
  HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

  //CHANGE SYS_RST FROM OUTPUT TO INPUT
  Convert_sysrst_gpio(1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  Scan_Cuerrent();

	  //POLL PCIEE_RST STATUS FOR SYS_RST
	  if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIEE_RST_X_MCU_GPIO_Port, PCIEE_RST_X_MCU_Pin))
	  {
		  Convert_sysrst_gpio(0);

		  HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
		  HAL_Delay(30);
		  HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

		  Convert_sysrst_gpio(1);
	  }
//	  GET_Current_Voltage();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
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
