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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_slave.h"
#include "mcu.h"
#include "wdt.h"
#include "tmp451.h"
#include "soc_eeprom.h"
#include "eeprom.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "upgrade.h"
#include "sm5_gpioex.h"
#include "se5_gpioex.h"
#include "tca6416a.h"
#include "pic.h"
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
struct i2c_slave_ctx i2c_ctx1;
struct i2c_slave_ctx i2c_ctx3;
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
#define PMIC_ADDR 0x3c

#define IO_CHIPNAME		0x01
#define IO_MODECTRL		0x24
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
#define BUCK4_DVS0CFG0		0x97

int clean_pmic(void);

#define LOG_ERROR_LINE()								\
	do {												\
		i2c_regs.error_line_l = __LINE__ & 0xff;		\
		i2c_regs.error_line_h = (__LINE__ >> 8) & 0xff;	\
	} while(0)

#define LOG_ERROR_CAUSE(cause)							\
	do {												\
		i2c_regs.cause_pwr_down = cause;				\
	} while(0)

#define LOG_ERROR_CODE(code)							\
	do {												\
		i2c_regs.error_code = code;						\
	} while(0)

#define PMIC_PWR_ON_I2C_CHECK(op)						\
	do {												\
		int err = (op);									\
		if (err != HAL_OK) {							\
			LOG_ERROR_CAUSE(ERR_PMIC_I2C_IO);			\
			LOG_ERROR_LINE();							\
			LOG_ERROR_CODE(err);						\
			goto poweron_fail;							\
		}												\
	} while (0)

void I2C_ClearBusyFlagErratum(I2C_HandleTypeDef *instance)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    int timeout =100;
    int timeout_cnt=0;

    // 1. Clear PE bit.
    instance->Instance->CR1 &= ~(0x0001);

    //  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    GPIO_InitStruct.Pin = PMIC_SCL_Pin|PMIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //fix tmp451 hold I2C bus BUG. stay low for 40ms ,make sure I2C slave can be reset by themselves
    HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_RESET);
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    HAL_GPIO_WritePin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin, GPIO_PIN_RESET);
    HAL_Delay(30);

    HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_SET);
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    HAL_GPIO_WritePin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin, GPIO_PIN_SET);


    // 3. Check SCL and SDA High level in GPIOx_IDR.
    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin))
    {
        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin))
    {
        //Move clock to release I2C
        HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_RESET);
        asm("nop");
        HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_SET);

        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    // 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin, GPIO_PIN_RESET);

    //  5. Check SDA Low level in GPIOx_IDR.
    while (GPIO_PIN_RESET != HAL_GPIO_ReadPin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin))
    {
        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    // 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_RESET);

    //  7. Check SCL Low level in GPIOx_IDR.
    while (GPIO_PIN_RESET != HAL_GPIO_ReadPin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin))
    {
        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    // 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin, GPIO_PIN_SET);

    // 9. Check SCL High level in GPIOx_IDR.
    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(PMIC_SCL_GPIO_Port, PMIC_SCL_Pin))
    {
        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    // 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
    HAL_GPIO_WritePin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin, GPIO_PIN_SET);

    // 11. Check SDA High level in GPIOx_IDR.
    while (GPIO_PIN_SET != HAL_GPIO_ReadPin(PMIC_SDA_GPIO_Port, PMIC_SDA_Pin))
    {
        timeout_cnt++;
        if(timeout_cnt>timeout)
            return;
    }

    // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
    GPIO_InitStruct.Pin = PMIC_SCL_Pin|PMIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#if 0
    // STM32L071 donnot have SWRST bit.
    // It is RXDMAEN at that bit position.
    // 13. Set SWRST bit in I2Cx_CR1 register.
    instance->Instance->CR1 |= 0x8000;

    asm("nop");

    // 14. Clear SWRST bit in I2Cx_CR1 register.
    instance->Instance->CR1 &= ~0x8000;

    asm("nop");
#endif

    // 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
    instance->Instance->CR1 |= 0x0001;

    // Call initialization function.
    HAL_I2C_Init(instance);
}

static int pin_state_error;
static int pin_state_error_line;

#define GPIO_SET_CHECK(port, pin, state)			\
	do {							\
		HAL_GPIO_WritePin(port, pin, state);		\
		HAL_Delay(1);					\
		if (HAL_GPIO_ReadPin(port, pin) != state) {	\
			pin_state_error = 1;			\
			pin_state_error_line = __LINE__;	\
		}						\
	} while (0)

void PowerON(void)
{
	unsigned char tmp[8];
	i2c_regs.cause_pwr_down = 0;
	uint8_t i = 0;
	uint8_t pcie_mode = 0;

	I2C_ClearBusyFlagErratum(&hi2c2);

	if (clean_pmic()) {
		goto poweron_fail;
	}

	tmp[0] = 0;
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_VOUTFBDIV,1, tmp, 1, 1000));// 1.2v
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_VOUTFBDIV,1, tmp, 1, 1000));// 1.2v
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_VOUTFBDIV,1, tmp, 1, 1000));// 1.2v

	tmp[0] = 0xE5;
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, IO_MODECTRL,1, tmp, 1, 1000));// enable buck1-3

	tmp[0] = 0xD0;
	tmp[1] = 0x00;
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK1_DVS0CFG1,1, tmp, 2, 1000));// LDO1V_IN

	HAL_Delay(2);
	HAL_GPIO_WritePin(GPIOB, EN_VDDIO18_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDDIO18_GPIO_Port, PG_VDDIO18_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDIO18;
		goto poweron_fail;
	}


	HAL_GPIO_WritePin(GPIOB, EN1_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDDC_GPIO_Port, PG_VDDC_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDC;
		goto poweron_fail;
	}

	HAL_GPIO_WritePin(GPIOB, EN_VDDIO33_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDDIO33_GPIO_Port, PG_VDDIO33_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDIO33;
		goto poweron_fail;
	}

	// 1 soc 2 PCIE
	for (i = 0; i <  5; i++) {
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIE_RST_MCU_GPIO_Port,PCIE_RST_MCU_Pin)) {
			pcie_mode++;
		}
	}

	if (pcie_mode > 3) {
		i2c_regs.mode_flag = 2;
	} else {
		i2c_regs.mode_flag = 1;
	}

	HAL_GPIO_WritePin(GPIOB, EN_VDD_PHY_Pin, GPIO_PIN_SET);//EN_PHY
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDD_PHY_GPIO_Port, PG_VDD_PHY_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDPHY;
		goto poweron_fail;
	}

	HAL_GPIO_WritePin(GPIOA, P08_PWR_GOOD_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOB, EN_VDD_PCIE_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDD_PCIE_GPIO_Port, PG_VDD_PCIE_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDPCIE;
		goto poweron_fail;
	}

	HAL_GPIO_WritePin(GPIOA, GPIO2_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, EN0_ISL68127_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	if (HAL_GPIO_ReadPin(PG_VDD_TPU_GPIO_Port, PG_VDD_TPU_Pin) == GPIO_PIN_RESET) {
		i2c_regs.cause_pwr_down = ERR_VDDTPU;
		goto poweron_fail;
	}

	HAL_GPIO_WritePin(GPIOA, GPIO3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	tmp[0] = 0xE5;
	tmp[1] = 0x00;
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK2_DVS0CFG1,1, tmp, 2, 1000));//DDR_VDDQ 1.1v
	HAL_Delay(1);
#if 0  //DDR4
	tmp[0] = 0xE5;
	tmp[1] = 0x00;
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, tmp, 2, 1000));//DDR*_DDR_VDDQLP 1.1v
	i2c_regs.ddr = 1;
#else //ddr4x
	tmp[0] = 0x7D;//1.8ms
	tmp[1] = 0x00;//1.8ms
	PMIC_PWR_ON_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2,PMIC_ADDR, BUCK3_DVS0CFG1,1, tmp, 2, 1000));//DDR*_DDR_VDDQLP 0.6v
	i2c_regs.ddr = 0;
#endif
	HAL_Delay(1);//5ms
	HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_RESET);
	HAL_Delay(2);
#if 0 //PG can not be detect in some board
	if (HAL_GPIO_ReadPin(PG_VDD_TPU_MEM_GPIO_Port, PG_VDD_TPU_MEM_Pin) == GPIO_PIN_RESET) {
		HAL_Delay(5);
		if (HAL_GPIO_ReadPin(PG_VDD_TPU_MEM_GPIO_Port, PG_VDD_TPU_MEM_Pin) == GPIO_PIN_RESET)
			i2c_regs.cause_pwr_down = ERR_VDDTPUMEM;
		goto poweron_fail;
	}
#endif
	HAL_GPIO_WritePin(GPIOA, GPIO1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_VQPS18_GPIO_Port, EN_VQPS18_Pin, GPIO_PIN_SET);

	i2c_slave_reset(&i2c_ctx3);
//	HAL_Delay(30);
//
//	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_SET);

	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	//POLL PCIEE_RST STATUS FOR SYS_RST
	while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIE_RST_MCU_GPIO_Port, PCIE_RST_MCU_Pin))
			  		;
	HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

poweron_fail:
	if (i2c_regs.cause_pwr_down == 0) {
		i2c_regs.power_good = 1;
	} else {
		i2c_regs.power_good = 0;
		intr_status_set(POWERON_ERR);
		PowerDOWN();
	}

	return;
}

static inline void intr_pin_set(int status)
{
	if (i2c_regs.vender == VENDER_SE5)
		return;

	status = status ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(MCU_CPLD_ERR_GPIO_Port,
			  MCU_CPLD_ERR_Pin,
			  status);
}

void intr_status_set(uint8_t intr)
{
	__disable_irq();
	i2c_regs.intr_status1 |= intr;
	intr_pin_set(i2c_regs.intr_status1 & ~(i2c_regs.intr_mask1));
	__enable_irq();
}

void intr_status_clr(uint8_t intr)
{
	__disable_irq();
	i2c_regs.intr_status1 &= ~intr;
	intr_pin_set(i2c_regs.intr_status1 & ~(i2c_regs.intr_mask1));
	__enable_irq();
}

void intr_mask_set(uint8_t mask)
{
	__disable_irq();
	i2c_regs.intr_mask1 = mask;
	intr_pin_set(i2c_regs.intr_status1 & ~(i2c_regs.intr_mask1));
	__enable_irq();
}

#define PMIC_CLEAN_I2C_CHECK(op)						\
	do {												\
		int err = (op);									\
		if (err != HAL_OK) {							\
			LOG_ERROR_CAUSE(ERR_PMIC_I2C_IO);			\
			LOG_ERROR_LINE();							\
			LOG_ERROR_CODE(err);						\
			return -1;									\
		}												\
	} while (0)

int clean_pmic(void)
{
	uint8_t tmp[2] = {0, 0};
	uint32_t tickstart = HAL_GetTick();
	int err;

	HAL_Delay(5);

	do {
		err = HAL_I2C_Mem_Read(&hi2c2, PMIC_ADDR, IO_CHIPNAME, 1, tmp, 1, 10);
		if (err == HAL_OK)
			break;
	} while(HAL_GetTick() - tickstart <= 1000);

	if (err != HAL_OK) {
		i2c_regs.cause_pwr_down = ERR_PMIC_I2C_IO;
		return -1;
	}

	tmp[0] = tmp[1] = 0;

	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, IO_MODECTRL, 1, tmp, 1, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK1_VOUTFBDIV, 1, tmp, 1, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK2_VOUTFBDIV, 1, tmp, 1, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK3_VOUTFBDIV, 1, tmp, 1, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK1_DVS0CFG1, 1, tmp, 2, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK2_DVS0CFG1, 1, tmp, 2, 1000));
	PMIC_CLEAN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, PMIC_ADDR, BUCK3_DVS0CFG1, 1, tmp, 2, 1000));
	return 0;
}

void PowerDOWN(void)
{
	I2C_ClearBusyFlagErratum(&hi2c2);

	i2c_regs.power_good = 0;
	clean_pmic();
	HAL_Delay(100);

	HAL_GPIO_WritePin(GPIOA, DDR_PWR_GOOD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_VQPS18_GPIO_Port, EN_VQPS18_Pin, GPIO_PIN_RESET);
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

	i2c_slave_reset(&i2c_ctx3);
}

void BM1684_RST(void)
{
	intr_status_set(RESET_OP);

	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	GPIO_SET_CHECK(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);

	return ;
}

void BM1684_REBOOT(void)
{

	if (i2c_regs.vender == VENDER_SE5 &&
	    (is_pic_available || is_tca6416a_available)) {
		se5_reset_board();
	} else {
		PowerDOWN();
		HAL_Delay(50);
		PowerON();
	}
}

static inline void led_on(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_SET);
}

static inline void led_off(void)
{
	HAL_GPIO_WritePin(GPIOA, MCU_LED_Pin, GPIO_PIN_RESET);
}

static inline void led_flicker(void)
{
	/* flicker at 1Hz */
	/* it is too slow by this way */
	// tick = (tick / 500) & 1;
	/* consider this */
	// tick = (tick / 512) & 1;
	// tick &= 1 << 9; //faster then divide

	if (HAL_GetTick() & (1 << 9))
		led_on();
	else
		led_off();
}

static inline void led_flicker_high(void)
{
	if (HAL_GetTick() & (1 << 6))
		led_on();
	else
		led_off();
}

static void led_update(void)
{

	if (pin_state_error)
		led_flicker_high();
	else if (i2c_regs.intr_status1 & (OVER_TEMP_POWEROFF | OVER_TEMP_ALERT))
		led_on();
	else if (i2c_regs.power_good)
		led_flicker();
	else
		led_off();
}

void SET_HW_Ver(void)
{
	  uint8_t i;
	  uint8_t PCB_Ver0 = 0;
	  uint8_t BOM_Ver0 = 0;
	  uint16_t ADC_Buf[2];

	  for (i = 0; i < 2; i++) {
		  HAL_ADC_Start(&hadc);
		  HAL_ADC_PollForConversion(&hadc, 0XFFFF);

		  if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC)) {
			  ADC_Buf[i] = HAL_ADC_GetValue(&hadc);
		  }
	  }

	  //0,0.18,0.46,0.71,1,1.28
	  //0,368, 942,1454,2048,2621
	  if (ADC_Buf[0] < 200) {
		  PCB_Ver0 = 0;
	  } else if ((ADC_Buf[0] > 250) && (ADC_Buf[0] < 600)) {
		  PCB_Ver0 = 1;
	  } else if ((ADC_Buf[0] > 700) && (ADC_Buf[0] < 1150)) {
		  PCB_Ver0 = 2;
	  } else if ((ADC_Buf[0] > 1200) && (ADC_Buf[0] < 1700)) {
		  PCB_Ver0 = 3;
	  } else if ((ADC_Buf[0] > 1750) && (ADC_Buf[0] < 2300)) {
		  PCB_Ver0 = 4;
	  } else if ((ADC_Buf[0] > 2350) && (ADC_Buf[0] < 2900)) {
		  PCB_Ver0 = 5;
	  }

	  if (ADC_Buf[1] < 200) {
		  BOM_Ver0 = 0;
	  } else if ((ADC_Buf[1] > 250) && (ADC_Buf[1] < 600)) {
		  BOM_Ver0 = 1;
	  } else if ((ADC_Buf[1] > 700) && (ADC_Buf[1] < 1150)) {
		  BOM_Ver0 = 2;
	  } else if ((ADC_Buf[1] > 1200) && (ADC_Buf[1] < 1700)) {
		  BOM_Ver0 = 3;
	  } else if ((ADC_Buf[1] > 1750) && (ADC_Buf[1] < 2300)) {
		  BOM_Ver0 = 4;
	  } else if ((ADC_Buf[1] > 2350) && (ADC_Buf[1] < 2900)) {
		  BOM_Ver0 = 5;
	  }

	  i2c_regs.hw_ver = (PCB_Ver0 << 4) | BOM_Ver0;

	  HAL_ADC_Stop(&hadc);
}

#define TMP451_SLAVE_ADDR (0x98)
uint8_t temp1684;	// before calibration
static uint8_t alert_cnt = 0;
static uint8_t powerdown_cnt = 0;

#define TMP451_MAIN_I2C_CHECK(op)				\
	do {										\
		int err, retry;							\
		for (retry = 0; retry < 5; ++retry) {	\
			err = (op);							\
			if (err == HAL_OK)					\
				break;							\
			I2C_ClearBusyFlagErratum(&hi2c2);	\
		}										\
		if (err != HAL_OK) {					\
			LOG_ERROR_CAUSE(ERR_PMIC_I2C_IO);	\
			LOG_ERROR_LINE();					\
			LOG_ERROR_CODE(err);				\
		}										\
	} while (0)

#define TMP451_ALERT		(0x22)
#define TMP451_SMBTO_MASK	(1 << 7)

void READ_Temper(void)
{
	/* enable tmp451 smbus timeout feature */
	static uint8_t smbto = 0;

	if (smbto == 0) {
		/* tmp451 smbus timeout will release sda and scl after 20ms from i2c start */
		/* 30ms delay is for safety */
		HAL_Delay(30);
		/* first time tempareture get, from stm32 bootup */
		TMP451_MAIN_I2C_CHECK(HAL_I2C_Mem_Read(&hi2c2, TMP451_SLAVE_ADDR, TMP451_ALERT, 1, &smbto, 1, 1000));
		if ((smbto & TMP451_SMBTO_MASK) == 0) {
			/* tmp451 has not enabled smbus timeout feature, enable it */
			smbto |= TMP451_SMBTO_MASK;
			TMP451_MAIN_I2C_CHECK(HAL_I2C_Mem_Write(&hi2c2, TMP451_SLAVE_ADDR, TMP451_ALERT, 1, &smbto, 1, 1000));
		}
	}

	// detection of temperature value
	TMP451_MAIN_I2C_CHECK(HAL_I2C_Mem_Read(&hi2c2,TMP451_SLAVE_ADDR, 0x0,1, (uint8_t*)&i2c_regs.temp_board, 1, 1000));
	if (i2c_regs.power_good) {
		TMP451_MAIN_I2C_CHECK(HAL_I2C_Mem_Read(&hi2c2,TMP451_SLAVE_ADDR, 0x1,1, &temp1684, 1, 1000));

		if (temp1684 < 4) {
			i2c_regs.temp1684 = 0;
		} else {
//			i2c_regs.temp1684 = (temp1684 * 232- 886) >> 8; //rough handling of tempareture calibration
			i2c_regs.temp1684 = temp1684 - 5;
		}
	} else {
		i2c_regs.temp1684 = i2c_regs.temp_board;
	}

	if ((i2c_regs.temp1684 > 95) && (i2c_regs.temp_board > 85)) {//temperature too high, powerdown
		powerdown_cnt++;
		if (powerdown_cnt == 3) {
			intr_status_set(OVER_TEMP_POWEROFF);
			PowerDOWN();
			powerdown_cnt = 0;
		}
	} else if ((i2c_regs.temp1684 > 85) && (i2c_regs.temp_board > 80)) {//temperature too high alert
		alert_cnt++;
		if (alert_cnt ==3) {
			intr_status_set(OVER_TEMP_ALERT);
			alert_cnt = 0;
		}
	} else {
		alert_cnt = 0;
		powerdown_cnt = 0;
	}
	se5_heater_ctrl(i2c_regs.temp1684);
}

#define POWER_OFF_TIME 5
static int poweroffcheck = POWER_OFF_TIME;
int needpoweroff = 0;
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
	wdt_isr();
	mcu_tick_isr();
	if (i2c_regs.vender == VENDER_SM5_S) {
		if (sm5_gpioex_getpoweroff() == 1)
		{
			poweroffcheck--;
			if (poweroffcheck == 0)
			{
				needpoweroff = 1;
			}
		}
		else
		{
			poweroffcheck = POWER_OFF_TIME;
		}
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
	//  uint16_t board_type_addr = BOARD_TYPE;
	setup_stage();
	if (i2c_regs.stage == RUN_STAGE_LOADER) {
		/* success */
		if (check_app() == 0) {
			app_start();
		}
	}
	/* USER CODE END 1 */


	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	EEPROM_ReadBytes(VENDER_Addr, (uint8_t *)&i2c_regs.vender, 1);
#if 0
	i2c_regs.vender = VENDER_SM5_S;
	EEPROM_WriteBytes(VENDER_Addr, (uint8_t *)&i2c_regs.vender, 1);
#endif
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
	MX_I2C2_Init();// iic device
	MX_I2C1_Init();// iic device
	MX_I2C3_Init();// iic device
	MX_LPTIM1_Init();
	/* USER CODE BEGIN 2 */
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	/* put led to off state */
	led_off();

	HAL_LPTIM_Start1HZ(&hlptim1);
	/*

	   1684 -> hi2c3 -> 0x17	  MCU
	   1684 -> hi2c3 -> 0x40	  GPIOEX
	   1684 -> hi2c3 -> 0x69	  WDT
	   1684 -> hi2c3 -> 0x6A	  EEPROM
	   1684 -> hi2c3 -> 0x6B	  TMP451

	   if i2c_regs.vender == VENDER_SA5
	   {
	   control -> hi2c1 -> 0x38-3F MCU
	   control -> hi2c1 -> 0x6A	  EEPROM
	   1684	  -> hi2c3 -> 0x68	  RTC DS1307
	   }
	   else //VENDER_SM5
	   {
	   1684	  -> hi2c3 -> 0x6C	  TCA6416  i2c to 16 gpio

	   hi2c1 -> TCA6416	i2c to 16 gpio
	   }
	   */

	tca6416a_probe();
	pic_probe();

	if (is_tca6416a_available && is_pic_available) {
		/* SM5 V3 or later motherboard */
		i2c_regs.vender = VENDER_SE5;
	} else if (!is_tca6416a_available &&
		   !is_pic_available &&
		   i2c_regs.vender != VENDER_SM5_P) {
		/* SA5 motherboard board */
		i2c_regs.vender = VENDER_SA5;
	} else if (i2c_regs.vender == VENDER_SM5_S ||
		   i2c_regs.vender == VENDER_SM5_P) {
		i2c_regs.vender = is_tca6416a_available ? VENDER_SM5_S : VENDER_SM5_P;
	}

	i2c_ctx1.id = 1;
	i2c_ctx3.id = 3;

	if (i2c_regs.vender == VENDER_SA5)
		i2c_init(&i2c_ctx1, hi2c1.Instance);

	i2c_init(&i2c_ctx3, hi2c3.Instance);

	switch (i2c_regs.vender) {
	case VENDER_SA5:
		break;
	case VENDER_SM5_S:
		sm5_gpioex_init();
		HAL_Delay(200);
		sm5_gpioex_12v_on();
		sm5_gpioex_led1_on();
		PowerON();
		break;
	case VENDER_SM5_P:
		PowerON();
		break;
	case VENDER_SE5:
		se5_gpioex_init();
		PowerON();
		break;
	}

	tmp451_init();
	mcu_init();
	wdt_init();
	eeprom_init();

	i2c_slave_start(&i2c_ctx3);
	if (i2c_regs.vender == VENDER_SA5)
		i2c_slave_start(&i2c_ctx1);

	// make sure PB6 is high
	//  HAL_GPIO_WritePin(GPIOB, EN_VDD_TPU_MEM_Pin, GPIO_PIN_SET);
	//  HAL_GPIO_WritePin(GPIOB, TPU_I2C_ADD3_Pin, GPIO_PIN_SET);
	// set PCB & BOM version by voltage value
	SET_HW_Ver();

	if (i2c_regs.vender == VENDER_SA5) {
		unsigned int tick_start = HAL_GetTick();
		while (!i2c_regs.power_good) {
			unsigned int tick_elapse = HAL_GetTick() - tick_start;
			if (tick_elapse > 1000)
				PowerON();
		}
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// response CPLD's commands
		extern void mcu_process_cmd_slow(void);
		mcu_process_cmd_slow();
		soc_wdt_reset_process();

		// read temperature every 2 seconds
		static uint32_t last_read_time = 0;
		static int ever_read = 0;
		if (ever_read == 0) {
			/* read temperature at boot time */
			READ_Temper();
			ever_read = 1;
			last_read_time = HAL_GetTick();
		} else {
			uint32_t read_time = HAL_GetTick();

			if (read_time - last_read_time > 2000) {
				READ_Temper();
				last_read_time = read_time;
			}
		}
		led_update();

		//POLL PCIEE_RST STATUS FOR SYS_RST
		if ((i2c_regs.vender == VENDER_SM5_P) &&
		    (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIE_RST_MCU_GPIO_Port,
							PCIE_RST_MCU_Pin))) {
			i2c_regs.mode_flag = 2;
			HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_RESET);
			HAL_Delay(30);
			while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(PCIE_RST_MCU_GPIO_Port, PCIE_RST_MCU_Pin))
				;
			HAL_GPIO_WritePin(SYS_RST_X_GPIO_Port, SYS_RST_X_Pin, GPIO_PIN_SET);
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (needpoweroff)
		{
			needpoweroff = 0;
			PowerDOWN();
			sm5_gpioex_led1_off();
			sm5_gpioex_12v_off();
		}

		if (i2c_regs.vender == VENDER_SE5)
			se5_smb_alert();
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
                              |RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
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
