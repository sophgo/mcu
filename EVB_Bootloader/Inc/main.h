/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYS_RST_X_Pin GPIO_PIN_13
#define SYS_RST_X_GPIO_Port GPIOC
#define STATUS_LED0_Pin GPIO_PIN_14
#define STATUS_LED0_GPIO_Port GPIOC
#define STATUS_LED1_Pin GPIO_PIN_15
#define STATUS_LED1_GPIO_Port GPIOC
#define I_12V_ATX_Pin GPIO_PIN_0
#define I_12V_ATX_GPIO_Port GPIOA
#define I_VDDIO5_Pin GPIO_PIN_1
#define I_VDDIO5_GPIO_Port GPIOA
#define I_VDDIO18_Pin GPIO_PIN_2
#define I_VDDIO18_GPIO_Port GPIOA
#define I_VDDIO33_Pin GPIO_PIN_3
#define I_VDDIO33_GPIO_Port GPIOA
#define I_VDD_PHY_Pin GPIO_PIN_4
#define I_VDD_PHY_GPIO_Port GPIOA
#define I_VDD_PCIE_Pin GPIO_PIN_5
#define I_VDD_PCIE_GPIO_Port GPIOA
#define I_VDD_TPU_MEM_Pin GPIO_PIN_6
#define I_VDD_TPU_MEM_GPIO_Port GPIOA
#define I_DDR_VDDQ_Pin GPIO_PIN_7
#define I_DDR_VDDQ_GPIO_Port GPIOA
#define I_DDR_VDDQLP_Pin GPIO_PIN_0
#define I_DDR_VDDQLP_GPIO_Port GPIOB
#define PCB_VER_Pin GPIO_PIN_1
#define PCB_VER_GPIO_Port GPIOB
#define NCT_INT_Pin GPIO_PIN_2
#define NCT_INT_GPIO_Port GPIOB
#define PMIC_SCL_Pin GPIO_PIN_10
#define PMIC_SCL_GPIO_Port GPIOB
#define PMIC_SDA_Pin GPIO_PIN_11
#define PMIC_SDA_GPIO_Port GPIOB
#define EN_VDDIO18_Pin GPIO_PIN_12
#define EN_VDDIO18_GPIO_Port GPIOB
#define EN_VDDIO33_Pin GPIO_PIN_13
#define EN_VDDIO33_GPIO_Port GPIOB
#define EN_VDD_PHY_Pin GPIO_PIN_14
#define EN_VDD_PHY_GPIO_Port GPIOB
#define EN_VDD_PCIE_Pin GPIO_PIN_15
#define EN_VDD_PCIE_GPIO_Port GPIOB
#define GPIO1_Pin GPIO_PIN_8
#define GPIO1_GPIO_Port GPIOA
#define GPIO2_Pin GPIO_PIN_9
#define GPIO2_GPIO_Port GPIOA
#define GPIO3_Pin GPIO_PIN_10
#define GPIO3_GPIO_Port GPIOA
#define P08_PWR_GOOD_Pin GPIO_PIN_11
#define P08_PWR_GOOD_GPIO_Port GPIOA
#define DDR_PWR_GOOD_Pin GPIO_PIN_12
#define DDR_PWR_GOOD_GPIO_Port GPIOA
#define EN_VQPS18_Pin GPIO_PIN_15
#define EN_VQPS18_GPIO_Port GPIOA
#define PG_ALL_Pin GPIO_PIN_3
#define PG_ALL_GPIO_Port GPIOB
#define PCIEE_RST_X_MCU_Pin GPIO_PIN_4
#define PCIEE_RST_X_MCU_GPIO_Port GPIOB
#define PMIC_EN_Pin GPIO_PIN_5
#define PMIC_EN_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define EN0_ISL68127_Pin GPIO_PIN_8
#define EN0_ISL68127_GPIO_Port GPIOB
#define EN1_ISL68127_Pin GPIO_PIN_9
#define EN1_ISL68127_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define MAX_REG_SIZE 0x20
extern volatile uint8_t reg[MAX_REG_SIZE];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
