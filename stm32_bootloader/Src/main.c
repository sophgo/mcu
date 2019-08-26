/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "eeprom.h"
//#include "bootloader.h"
#include "flash_if.h"
#include "i2c_slave.h"
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
void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}

#define APPLICATION_ADDRESS 0x8008000

#define EEPROM_BASE 0x08080c00

/* Private variables ---------------------------------------------------------*/
typedef  void (*pFunction)(void);

pFunction JumpToApplication;
uint32_t JumpAddress;

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
	val = 0xE5;//1.8ms
	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 1.1v
//	val = 0x7D;//1.8ms
//	HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, &val, 2, 1000);//DDR*_DDR_VDDQLP 0.6v
	HAL_Delay(1);//5ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN_VQPS18_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_SET);
	led_on();
}

#define MAX_REG_SIZE 0x8
volatile uint8_t reg[MAX_REG_SIZE] = { 0x01,0,0,0,0,0,0,0 };

extern uint32_t ramsource;
extern uint32_t packet_length;

/* Initialize flashdestination variable */
extern uint32_t flashdestination;

extern uint8_t flash_write;

uint32_t addr = 0xBF0;

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
  MX_ADC_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_LPTIM1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  I2C_SlaveInit(&hi2c3, (uint8_t *)reg, MAX_REG_SIZE);
  I2C_SlaveInit(&hi2c1, (uint8_t *)reg, MAX_REG_SIZE);

  uint8_t Buffer;
//  EEPROM_WriteBytes(addr, &Buffer, 1);
  EEPROM_ReadBytes(addr, &Buffer, 1);

//#if debug
  if (Buffer == 8) {
	  PowerON();
//	  while(1);

	  while(1) {
		  if (reg[7] == 1){
			  Buffer = 0;
			  EEPROM_WriteBytes(addr, &Buffer, 1);
			  break;
		  }
	  }
  }
  /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
  if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000) {
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
    JumpToApplication = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
    JumpToApplication();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
