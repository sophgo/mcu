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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

typedef struct CURRENT_VAL_t
{
	uint16_t i_12v_atx;
	uint16_t i_vddio5;
	uint16_t i_vddio18;
	uint16_t i_vddio33;
	uint16_t i_vdd_phy;
	uint16_t i_vdd_pcie;
	uint16_t i_vdd_tpu_mem;
	uint16_t i_ddr_vddq;
	uint16_t i_ddr_vddqlp;
	uint16_t i_ldo_pcie;
}CURRENT_VAL;
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
#define PCIE_RST_MCU_Pin GPIO_PIN_14
#define PCIE_RST_MCU_GPIO_Port GPIOC
#define PCIE_RST_MCU_EXTI_IRQn EXTI4_15_IRQn
#define PMIC_INT_Pin GPIO_PIN_15
#define PMIC_INT_GPIO_Port GPIOC
#define PG_VDDC_Pin GPIO_PIN_0
#define PG_VDDC_GPIO_Port GPIOH
#define PG_VDD_TPU_Pin GPIO_PIN_1
#define PG_VDD_TPU_GPIO_Port GPIOH
#define PCB_VER0_Pin GPIO_PIN_0
#define PCB_VER0_GPIO_Port GPIOA
#define BOM_VER0_Pin GPIO_PIN_1
#define BOM_VER0_GPIO_Port GPIOA
#define PG_VDD_PHY_Pin GPIO_PIN_2
#define PG_VDD_PHY_GPIO_Port GPIOA
#define PG_VDDIO18_Pin GPIO_PIN_3
#define PG_VDDIO18_GPIO_Port GPIOA
#define MCU_LED_Pin GPIO_PIN_5
#define MCU_LED_GPIO_Port GPIOA
#define MCU_CPLD_ERR_Pin GPIO_PIN_6
#define MCU_CPLD_ERR_GPIO_Port GPIOA
#define GPIO1_Pin GPIO_PIN_7
#define GPIO1_GPIO_Port GPIOA
#define EN0_ISL68127_Pin GPIO_PIN_0
#define EN0_ISL68127_GPIO_Port GPIOB
#define EN1_ISL68127_Pin GPIO_PIN_1
#define EN1_ISL68127_GPIO_Port GPIOB
#define PG_VDD_TPU_MEM_Pin GPIO_PIN_2
#define PG_VDD_TPU_MEM_GPIO_Port GPIOB
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
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOA
#define GPIO2_Pin GPIO_PIN_9
#define GPIO2_GPIO_Port GPIOA
#define GPIO3_Pin GPIO_PIN_10
#define GPIO3_GPIO_Port GPIOA
#define P08_PWR_GOOD_Pin GPIO_PIN_11
#define P08_PWR_GOOD_GPIO_Port GPIOA
#define DDR_PWR_GOOD_Pin GPIO_PIN_12
#define DDR_PWR_GOOD_GPIO_Port GPIOA
#define PG_VDDIO33_Pin GPIO_PIN_15
#define PG_VDDIO33_GPIO_Port GPIOA
#define EN_VQPS18_Pin GPIO_PIN_3
#define EN_VQPS18_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_4
#define I2C1_SDA_GPIO_Port GPIOB
#define TWARN_VDD_TPU_Pin GPIO_PIN_5
#define TWARN_VDD_TPU_GPIO_Port GPIOB
#define EN_VDD_TPU_MEM_Pin GPIO_PIN_6
#define EN_VDD_TPU_MEM_GPIO_Port GPIOB
#define PG_VDD_PCIE_Pin GPIO_PIN_7
#define PG_VDD_PCIE_GPIO_Port GPIOB
#define CPLD_MCU_SCL_Pin GPIO_PIN_8
#define CPLD_MCU_SCL_GPIO_Port GPIOB
#define CPLD_MCU_SDA_Pin GPIO_PIN_9
#define CPLD_MCU_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define MCU_EEPROM_DATA_MAX    (32)
typedef struct I2C_REGS_t
{
	volatile uint8_t vender;
	volatile uint8_t sw_ver;
	volatile uint8_t hw_ver;
	volatile uint8_t cmd_reg;

	volatile uint8_t temp1684;
	volatile uint8_t temp_board;
	volatile uint8_t intr_status1;
	volatile uint8_t test_intr_en;

	volatile uint8_t intr_mask1;
	volatile uint8_t reserved0;
	volatile uint8_t rst_1684_times;

	volatile uint8_t uptime0;

	volatile uint8_t uptime1;
	volatile uint8_t cause_pwr_down;
	volatile uint8_t rtc[6];
	volatile uint8_t cmd;
	volatile uint8_t ddr;//0x15,21
	volatile uint8_t power_good;
	volatile uint8_t mode_flag;
	volatile uint8_t pmic_status;

	volatile uint8_t power_on_cmd;//0x19
	volatile uint8_t cmd_reg_bkup;
	volatile uint8_t reserved1[13];
	volatile CURRENT_VAL current;
	volatile uint8_t stage;			/* 0: application, 1: loader, 2: upgrader */
	volatile uint8_t reserved2[1];
	volatile uint8_t eeprom_offset_l;
	volatile uint8_t eeprom_offset_h;
	volatile uint8_t eeprom_data[MCU_EEPROM_DATA_MAX];
	volatile uint8_t error_line_l; /* log error line during power on */
	volatile uint8_t error_line_h; /* log error line during power on */
	volatile uint8_t error_code; /* last error code */
	volatile uint8_t i2c2_state; /* i2c2 software status */
}I2C_REGS;

extern I2C_REGS i2c_regs;

struct i2c_slave_ctx;
extern struct i2c_slave_ctx i2c_ctx1;
extern struct i2c_slave_ctx i2c_ctx3;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

void PowerON(void);
void PowerDOWN(void);
void BM1684_RST(void);
void BM1684_REBOOT(void);

void intr_status_set(uint8_t intr);
void intr_status_clr(uint8_t intr);
void intr_mask_set(uint8_t mask);

#define REG_VENDER			0x00
#define REG_SW_VER			0x01
#define REG_HW_VER			0x02
#define REG_CMD_REG			0x03

#define REG_TEMP1684		0X04
#define REG_TEMP_BOARD		0x05
#define REG_INTR_STATUS1	0x06
#define REG_TEST_INTR_EN	0x07

#define REG_INTR_MASK1		0x08
#define REG_RESERVED0		0x09
#define REG_1684_RST_TIMES	0x0a
#define REG_UPTIME0			0x0b

#define REG_UPTIME1			0x0c
#define REG_CAUSE_PWR_DOWN	0x0d

#define REG_SYS_RTC_YEAR	0x0e
#define REG_SYS_RTC_MON		0x0f
#define REG_SYS_RTC_DAY		0x10
#define REG_SYS_RTC_HOUR	0x11
#define REG_SYS_RTC_MIN		0x12
#define REG_SYS_RTC_SEC		0x13

#define REG_CMD				0x14

#define REG_DDR				0x15
#define REG_PWR_GOOD		0x16
#define REG_MODE_FLAG		0x17

#define REG_PMIC_STATUS		0x18
#define REG_PWRON_CMD		0x19
#define REG_CMD_BKUP		0x1a

/* some reserved here, original MAC0 and MAC1 */
#define I_12V_ATX_L			0x28
#define I_12V_ATX_H			0x29
#define I_VDDIO5_L			0x2a
#define I_VDDIO5_H			0x2B
#define I_VDDIO18_L			0x2c
#define I_VDDIO18_H			0x2D
#define I_VDDIO33_L			0x2e
#define I_VDDIO33_H			0x2f
#define I_VDD_PHY_L			0x30
#define I_VDD_PHY_H			0x31
#define I_VDD_PCIE_L		0x32
#define I_VDD_PCIE_H		0x33
#define I_VDD_TPU_MEM_L		0x34
#define I_VDD_TPU_MEM_H		0x35
#define I_DDR_VDDQ_L		0x36
#define I_DDR_VDDQ_H		0x37
#define I_DDR_VDDQLP_L		0x38
#define I_DDR_VDDQLP_H		0x39
#define I_LDO_PCIE_L		0x3a
#define I_LDO_PCIE_H		0x3b
#define REG_STAGE		0x3c
#define REG_EEPROM_OFFSET_L	0x3e	/* 16bit eeprom address, low 8bits */
#define REG_EEPROM_OFFSET_H	0x3f	/* 16bit eeprom address, high 8bits */
#define REG_EEPROM_DATA		0x40	/* eeprom data */
#define REG_ERROR_LINE_L	0x60	/* error line low byte */
#define REG_ERROR_LINE_H	0x61	/* error line high byte */
#define REG_ERROR_CODE		0x62	/* error code */
#define REG_I2C2_STATE		0x63	/* i2c2 state */
#define REG_NUMBER		sizeof(I2C_REGS)

#define BIT0   (0x01 << 0)
#define BIT1   (0x01 << 1)
#define BIT2   (0x01 << 2)
#define BIT3   (0x01 << 3)
#define BIT4   (0x01 << 4)
#define BIT5   (0x01 << 5)
#define BIT6   (0x01 << 6)
#define BIT7   (0x01 << 7)

#define OVER_TEMP_ALERT		BIT0
#define OVER_TEMP_POWEROFF	BIT1
#define POWERON_ERR		BIT2
#define WDT_RST			BIT3
#define RESET_OP		BIT4
#define TEST_INTR		BIT7

#define ERR_VDDIO18 BIT0
#define ERR_VDDC	BIT1
#define ERR_VDDIO33 BIT2
#define ERR_VDDPHY	BIT3
#define ERR_VDDPCIE	BIT4
#define ERR_VDDTPU	BIT5
#define ERR_VDDTPUMEM	BIT6
#define ERR_PMIC_I2C_IO	BIT7


//CPLD Command
#define CMD_CPLD_PWR_ON			0x01       //1684 power on
#define CMD_CPLD_PWR_DOWN		0x02	   //1684 power down
#define CMD_CPLD_1684RST		0x03       //reset 1684
#define CMD_CPLD_SWRST			0x04	   //soft resetting
#define CMD_CPLD_CLR			0x05       // clean MCU_ERR_INT, set 0.
//BM1684 Command
#define CMD_BM1684_RST			0x06       // power is always on
#define CMD_BM1684_REBOOT		0x07       // power down
#define CMD_MCU_UPDATE			0x08       // MCU UPDATE

//EEPROM  FLASH
#define EEPROM_BASE_ADDR	0x08080C00
#define EEPROM_BANK_SIZE	0x0C00

//
//uint32_t writeFlashData = 0x55aa55aa;
//uint8_t readFlashData[4] = {0};
//uint32_t addr = 0x08080000;
//uint16_t addr_offset = 0x0;
//
/*
 *  MAC0 Addr: EEPROM_BANK1_START
 *  MAC1 Addr: EEPROM_BANK1_START +  4
 *   SN Addr  : EEPROM_BANK1_START + 12
 */
#define MAC0_Addr	(EEPROM_BASE_ADDR + 32 * 0)
#define MAC1_Addr	(EEPROM_BASE_ADDR + 32 * 1)
#define SN_Addr		(EEPROM_BASE_ADDR + 32 * 2)
#define VENDER_Addr	(32 * 3)

#define UPDATE_FLAG_OFFSET (0xbf0)

#define MCU_VERSION	0x28

#define VENDER_SA5	0x01
#define VENDER_SC5	0x02
#define VENDER_SE5	0x03
#define VENDER_SM5_P	0x04
#define VENDER_SM5_S	0x05
#define VENDER_SA6	0x06

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
