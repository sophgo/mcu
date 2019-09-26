/**
  ******************************************************************************
  * File Name          : I2C.h
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_H
#define __i2c_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

/* USER CODE BEGIN Private defines */
#define CORE_MCU_ADDR           (0x38 << 1)
#define MCU_TYPE_IIC            0x00
#define MCU_VERSION_IIC         0x01
#define MCU_RESET_IIC           0x03
#define MCU_CTEMP_IIC           0x04   //bm1684 chip temperature register
#define MCU_BTEMP_IIC           0x05   //core board temperature register
#define MCU_INTR_IIC            0x07
#define MCU_LOCATION_IIC        0x14
#define MCU_EEPROM_LADDR_IIC    0x3E    //MCU eeprom low addr register
#define MCU_EEPROM_HADDR_IIC    0x3F    //MCU eeprom hign addr register
#define MCU_EEPROM_DATA_IIC     0x40    //MCU eeprom data iic

#define MCU_EEPROM_SN_ADDR      0x0000
#define MCU_EEPROM_MAC0_ADDR    0x0020
#define MCU_EEPROM_MAC1_ADDR    0x0040
#define MCU_EEPROM_TYPE_ADDR    0x0060

#define I2C2_SLAVE_ADDR    (0x34 << 1)

#define MCU_I2C_TIMEOUT 200

#define I2C3_SLAVE_BASE_ADDR 0x28
#define I2C3_SLAVE_ADDR(reg) ((I2C3_SLAVE_BASE_ADDR + (reg))<<1)

/* USER CODE END Private defines */

void MX_I2C1_Init(void);
void MX_I2C2_Init(void);
void MX_I2C3_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
