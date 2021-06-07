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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define HIGH_U16(a) ((uint8_t)(a>>8))
#define LOW_U16(a) ((uint8_t)a)
#define FORM_U16(high, low) (((uint16_t)high<<8)|((uint16_t)low))
#define TRUE 1
#define FALSE 0
#define SET 1
#define RESET 0

#define MAX_REG_SIZE 32
#define FACTORY_TEST_FLAG 1
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Error(const char *fmt, ...);
void Info(const char *fmt, ...);
void Notify(const char *fmt, ...);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TPU_IIC_ADD0_Pin GPIO_PIN_13
#define TPU_IIC_ADD0_GPIO_Port GPIOC
#define TPU_IIC_ADD1_Pin GPIO_PIN_14
#define TPU_IIC_ADD1_GPIO_Port GPIOC
#define TPU_IIC_ADD2_Pin GPIO_PIN_15
#define TPU_IIC_ADD2_GPIO_Port GPIOC
#define UART2_RX_Pin GPIO_PIN_0
#define UART2_RX_GPIO_Port GPIOA
#define UART2_TX_Pin GPIO_PIN_1
#define UART2_TX_GPIO_Port GPIOA
#define UART1_RX_Pin GPIO_PIN_2
#define UART1_RX_GPIO_Port GPIOA
#define UART1_TX_Pin GPIO_PIN_3
#define UART1_TX_GPIO_Port GPIOA
#define MCU_NRST_Pin GPIO_PIN_4
#define MCU_NRST_GPIO_Port GPIOA
#define STATUS_LED_Pin GPIO_PIN_5
#define STATUS_LED_GPIO_Port GPIOA
#define MCU_CPLD_ERR_Pin GPIO_PIN_6
#define MCU_CPLD_ERR_GPIO_Port GPIOA
#define MCU_CPLD_ERR_EXTI_IRQn EXTI4_15_IRQn
#define CORE_3V3_Pin GPIO_PIN_0
#define CORE_3V3_GPIO_Port GPIOB
#define CPLD_TPU_SCL_Pin GPIO_PIN_8
#define CPLD_TPU_SCL_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_9
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_10
#define UART_RX_GPIO_Port GPIOA
#define AIC_FULLIN0_Pin GPIO_PIN_11
#define AIC_FULLIN0_GPIO_Port GPIOA
#define AIC_FULLIN1_Pin GPIO_PIN_12
#define AIC_FULLIN1_GPIO_Port GPIOA
#define CPLD_TPU_SDA_Pin GPIO_PIN_4
#define CPLD_TPU_SDA_GPIO_Port GPIOB
#define CPLD_MCU_SCL_Pin GPIO_PIN_6
#define CPLD_MCU_SCL_GPIO_Port GPIOB
#define CPLD_MCU_SDA_Pin GPIO_PIN_7
#define CPLD_MCU_SDA_GPIO_Port GPIOB
#define SLOT_ID0_Pin GPIO_PIN_8
#define SLOT_ID0_GPIO_Port GPIOB
#define SLOT_ID1_Pin GPIO_PIN_9
#define SLOT_ID1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define AD_TO_VOLTAGE(ADC_DATA)  ((ADC_DATA)*2000/2457)
#define AD_TO_VOLTAGE_MINI(ADC_DATA)  ((ADC_DATA)*4200/4095)
typedef enum testStage_t {
	STAGE_IDLE = 0x00,
	STAGE_FULLIN,
	STAGE_POWER,
	STAGE_KERNEL
} testStage;
extern volatile testStage tStage;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
