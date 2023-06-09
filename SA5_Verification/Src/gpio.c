/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

void core_board_enter_test_mode(void)
{
	/* 0b01, enter test mode, 0b10, exit test mode */
	HAL_GPIO_WritePin(GPIOC, TPU_IIC_ADD0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, TPU_IIC_ADD1_Pin, GPIO_PIN_RESET);
}

void core_board_exit_test_mode(void)
{
	HAL_GPIO_WritePin(GPIOC, TPU_IIC_ADD0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, TPU_IIC_ADD1_Pin, GPIO_PIN_SET);
}

void core_board_mcu_reset(void)
{
	HAL_GPIO_WritePin(MCU_NRST_GPIO_Port, MCU_NRST_Pin, GPIO_PIN_RESET);
}

void core_board_mcu_run(void)
{
	HAL_GPIO_WritePin(MCU_NRST_GPIO_Port, MCU_NRST_Pin, GPIO_PIN_SET);
}

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, SLOT_ID0_Pin|SLOT_ID1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = TPU_IIC_ADD0_Pin|TPU_IIC_ADD1_Pin|TPU_IIC_ADD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = MCU_NRST_Pin|STATUS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MCU_CPLD_ERR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MCU_CPLD_ERR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PBPin PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|SLOT_ID0_Pin|SLOT_ID1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = AIC_FULLIN0_Pin|AIC_FULLIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    /*Configure GPIO pin Output Level */
  /* sm5mini will check these pins to determin if it should enter test mode */
  HAL_GPIO_WritePin(GPIOC, TPU_IIC_ADD2_Pin, GPIO_PIN_RESET);
  /* no effect until gpio mode config */
  core_board_enter_test_mode();

  /*Configure GPIO pin Output Level */
  /* hold sm5 mcu nrst down until everything is ok */
  /* no effect until gpio mode config */
  core_board_mcu_reset();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

  /* sm5mini reset mcu */
  HAL_Delay(1);
  HAL_GPIO_WritePin(MCU_NRST_GPIO_Port, MCU_NRST_Pin, GPIO_PIN_SET);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
