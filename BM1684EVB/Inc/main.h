/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
void Convert_sysrst_gpio(int io);
extern void PowerDOWN(void);
extern void PowerON(void);
extern void BM1684_RST(void);
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
#define RGMII0_RST_Pin GPIO_PIN_14
#define RGMII0_RST_GPIO_Port GPIOC
#define RGMII1_RST_Pin GPIO_PIN_15
#define RGMII1_RST_GPIO_Port GPIOC
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
#define I_LDO_PCIE_Pin GPIO_PIN_1
#define I_LDO_PCIE_GPIO_Port GPIOB
#define TEMP_THERM_N_Pin GPIO_PIN_2
#define TEMP_THERM_N_GPIO_Port GPIOB
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
#define EN_VDD_TPU_MEM_L_Pin GPIO_PIN_5
#define EN_VDD_TPU_MEM_L_GPIO_Port GPIOB
#define EN0_ISL68127_Pin GPIO_PIN_8
#define EN0_ISL68127_GPIO_Port GPIOB
#define EN1_ISL68127_Pin GPIO_PIN_9
#define EN1_ISL68127_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
