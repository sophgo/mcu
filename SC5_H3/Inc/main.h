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
#include "i2c_regs.h"
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
#define MCU_RCV_UP_MCU_Pin GPIO_PIN_13
#define MCU_RCV_UP_MCU_GPIO_Port GPIOA
#define EN_VDD_3V3_Pin GPIO_PIN_15
#define EN_VDD_3V3_GPIO_Port GPIOA
#define MCU_ALERT_Pin GPIO_PIN_4
#define MCU_ALERT_GPIO_Port GPIOB
#define IIC1_SDA_Pin GPIO_PIN_7
#define IIC1_SDA_GPIO_Port GPIOB
#define MCU_ADDR0_Pin GPIO_PIN_14
#define MCU_ADDR0_GPIO_Port GPIOC
#define EN_VQPS_1V8_Pin GPIO_PIN_12
#define EN_VQPS_1V8_GPIO_Port GPIOA
#define PMIC_INT_Pin GPIO_PIN_3
#define PMIC_INT_GPIO_Port GPIOB
#define EN_VDD_TPU_Pin GPIO_PIN_6
#define EN_VDD_TPU_GPIO_Port GPIOB
#define IIC1_SCL_Pin GPIO_PIN_8
#define IIC1_SCL_GPIO_Port GPIOB
#define MCU_ADDR1_Pin GPIO_PIN_15
#define MCU_ADDR1_GPIO_Port GPIOC
#define EN_VDD_1V8_Pin GPIO_PIN_10
#define EN_VDD_1V8_GPIO_Port GPIOA
#define PWR_GOOD_Pin GPIO_PIN_11
#define PWR_GOOD_GPIO_Port GPIOA
#define CURRENT_PCIE_3_3V_Pin GPIO_PIN_1
#define CURRENT_PCIE_3_3V_GPIO_Port GPIOB
#define EN_VDDC_Pin GPIO_PIN_5
#define EN_VDDC_GPIO_Port GPIOB
#define MCU_CTL_DOWN_MCU_Pin GPIO_PIN_9
#define MCU_CTL_DOWN_MCU_GPIO_Port GPIOA
#define PMIC_SDA_Pin GPIO_PIN_11
#define PMIC_SDA_GPIO_Port GPIOB
#define CURRENT_PCIE_12V_5V_Pin GPIO_PIN_0
#define CURRENT_PCIE_12V_5V_GPIO_Port GPIOB
#define HW_VER_Pin GPIO_PIN_0
#define HW_VER_GPIO_Port GPIOA
#define P08_PG_Pin GPIO_PIN_8
#define P08_PG_GPIO_Port GPIOA
#define PMIC_SCL_Pin GPIO_PIN_10
#define PMIC_SCL_GPIO_Port GPIOB
#define PCIE_RST_X_Pin GPIO_PIN_6
#define PCIE_RST_X_GPIO_Port GPIOA
#define VOL_12V_5V_Pin GPIO_PIN_4
#define VOL_12V_5V_GPIO_Port GPIOA
#define PCIE_PG_Pin GPIO_PIN_2
#define PCIE_PG_GPIO_Port GPIOA
#define DDR_PG_Pin GPIO_PIN_2
#define DDR_PG_GPIO_Port GPIOB
#define TPUMEM_PG_Pin GPIO_PIN_7
#define TPUMEM_PG_GPIO_Port GPIOA
#define VOL_3V3_Pin GPIO_PIN_5
#define VOL_3V3_GPIO_Port GPIOA
#define TPU_PG_Pin GPIO_PIN_3
#define TPU_PG_GPIO_Port GPIOA
#define SYS_RST_N_Pin GPIO_PIN_1
#define SYS_RST_N_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
