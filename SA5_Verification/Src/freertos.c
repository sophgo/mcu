/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "uart_streaming.h"
#include "i2c.h"
#include "i2creg_slave.h"
#include "adc.h"
#include "FreeRTOS_CLI.h"
#include "freertos_cmd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum signalFlag_t {
	SIGNAL_ERROR = 0x01,
	SIGNAL_KERNAL
} signalFlag;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define cmdMAX_INPUT_SIZE		50
#define cmdASCII_DEL	0x7F
#define cmdASCII_ETX	0x03

const uint8_t VERSION = 0x00;

static const char * const pcWelcomeMessage =
		"\fSA5 Test Board Shell\r\nType help to view all commands\r\n";
static const char * const pcPrompt = ">>> ";
static const char * const pcNewLine = "\r\n";
static const char * const pcBackSpace = "\b \b";
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#define MAX_INPUT_BUF_SIZE 32
char cRxBuffer[MAX_INPUT_BUF_SIZE] = { 0 };
volatile testStage tStage = STAGE_IDLE;
volatile uint8_t cmdRunning = 0;
volatile uint8_t uartBuf[6] = {0};
uint8_t reg[MAX_REG_SIZE] = { 0xFF };

uint8_t ucInputIndex = 0;
char cInputString[cmdMAX_INPUT_SIZE];

/* USER CODE END Variables */
osThreadId ShellThreadHandle;
osThreadId ExecThreadHandle;
osTimerId ledCtrlTimerHandle;
osTimerId timeoutTimerHandle;
osSemaphoreId uartTxSemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void runShell(void const * argument);
void runExec(void const * argument);
void ledCtrlCallback(void const * argument);
void timeoutCallback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	// Core board detect
	uint8_t data[1] = { 0x01 };
	if ((HAL_GPIO_ReadPin(AIC_FULLIN1_GPIO_Port, AIC_FULLIN1_Pin)
			== GPIO_PIN_RESET)
			&& (HAL_GPIO_ReadPin(AIC_FULLIN0_GPIO_Port, AIC_FULLIN0_Pin)
					== GPIO_PIN_RESET)) {
		HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
		HAL_ADC_Start(&hadc);
		HAL_Delay(500);
		if (HAL_I2C_Mem_Write(&hi2c1, 0x17 << 1, 0x03, 1, data, 1, 100)
				!= HAL_OK)
			Error("[initial] I2C write error");
		HAL_Delay(100);
		HAL_ADC_PollForConversion(&hadc, 10);
		tStage = STAGE_FULLIN;
	}

	HAL_UART_Transmit(&huart1, (uint8_t *) pcWelcomeMessage,
			strlen(pcWelcomeMessage), 0xFFFF);

	// auto power-up detect
	if (tStage == STAGE_FULLIN) {
		*data = 0x00;
		if (HAL_I2C_Mem_Read(&hi2c1, 0x17 << 1, 0x03, 1, data, 1, 100)
				!= HAL_OK)
			Error("[initial] I2C read error");
		Info("[initial] REG[0x03]=0x%02X", *data);
		int adValue = (int) AD_TO_VOLTAGE(HAL_ADC_GetValue(&hadc));
		Info("[initial] VCC=%dmV", adValue);
		if (adValue > 3100 && *data == 0x01) {
			tStage = STAGE_POWER;
			if (I2CReg_SlaveInit(&hi2c3, reg, MAX_REG_SIZE) != HAL_OK)
				Info("[initial] I2C Slave initial failed");
			Info("QA_PASS_INIT");
		} else {
			Info("QA_FAIL_INIT");
			tStage = STAGE_FULLIN;
		}
	} else if (tStage == STAGE_IDLE) {
		Info("[initial] Board NOT Found");
		Info("QA_FAIL_INIT");
	}

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of uartTxSem */
  osSemaphoreDef(uartTxSem);
  uartTxSemHandle = osSemaphoreCreate(osSemaphore(uartTxSem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of ledCtrlTimer */
  osTimerDef(ledCtrlTimer, ledCtrlCallback);
  ledCtrlTimerHandle = osTimerCreate(osTimer(ledCtrlTimer), osTimerPeriodic, NULL);

  /* definition and creation of timeoutTimer */
  osTimerDef(timeoutTimer, timeoutCallback);
  timeoutTimerHandle = osTimerCreate(osTimer(timeoutTimer), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of ShellThread */
  osThreadDef(ShellThread, runShell, osPriorityNormal, 0, 128);
  ShellThreadHandle = osThreadCreate(osThread(ShellThread), NULL);

  /* definition and creation of ExecThread */
  osThreadDef(ExecThread, runExec, osPriorityIdle, 0, 128);
  ExecThreadHandle = osThreadCreate(osThread(ExecThread), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

	//HAL_ADC_Start(&hadc);
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_runShell */
/**
 * @brief  Function implementing the ShellThread thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_runShell */
void runShell(void const * argument)
{

  /* USER CODE BEGIN runShell */
	osEvent evt;
	char *pcOutputString;
	BaseType_t xReturned;
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();
//	HAL_UART_Transmit(&huart1, (uint8_t *) pcWelcomeMessage,
//			strlen(pcWelcomeMessage), 0xFFFF);
//	if (tStage == STAGE_IDLE)
//
//	else if (tStage == STAGE_FULLIN)
//
//	else if (tStage == STAGE_POWER)
//
	Shell_RegisterCommand();
	HAL_UART_Transmit(&huart1, (uint8_t *) pcPrompt, strlen(pcPrompt), 0xFFFF);
	//HAL_UART_Receive_IT(&huart2, (uint8_t *) cRxBuffer, 1);
	UARTStr_RxInit(&huart1, (uint8_t *) cRxBuffer, MAX_INPUT_BUF_SIZE);
	/* Infinite loop */
	for (;;) {
		//HAL_UART_Receive_IT(&huart1, (uint8_t *) cRxBuffer, 1);
		evt = osSignalWait(0x00, osWaitForever);
		osSemaphoreWait(uartTxSemHandle, osWaitForever);
		for (int i = 0; i < evt.value.signals; i++) {
			if (cRxBuffer[i] == 0x00) { // should not be here
				memset(cRxBuffer, 0x00, sizeof(cRxBuffer));
				break;
			}
			if (cRxBuffer[i] == '\r') {

				/* See if the command is empty, indicating that the last command
				 is to be executed again. */
				if (ucInputIndex == 0) {
					HAL_UART_Transmit(&huart1, (uint8_t *) pcNewLine,
							strlen(pcNewLine), 0xFFFF);
					HAL_UART_Transmit(&huart1, (uint8_t *) pcPrompt,
							strlen(pcPrompt), 0xFFFF);
					osSemaphoreRelease(uartTxSemHandle);
					break;
				}
				HAL_UART_Transmit(&huart1, (uint8_t *) pcNewLine,
						strlen(pcNewLine), 0xFFFF);
				/* Pass the received command to the command interpreter.  The
				 command interpreter is called repeatedly until it returns
				 pdFALSE	(indicating there is no more output) as it might
				 generate more than one string. */
				cmdRunning = 1;
				do {
					/* Get the next output string from the command interpreter. */
					xReturned = FreeRTOS_CLIProcessCommand(cInputString,
							pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

					/* Write the generated string to the UART. */
					HAL_UART_Transmit(&huart1, (uint8_t *) pcOutputString,
							strlen(pcOutputString), 0xFFFF);
					memset(pcOutputString, 0x00,
					configCOMMAND_INT_MAX_OUTPUT_SIZE);
				} while (xReturned != pdFALSE);
				cmdRunning = 0;

				/* All the strings generated by the input command have been
				 sent.  Clear the input string ready to receive the next command.
				 Remember the command that was just processed first in case it is
				 to be processed again. */
				ucInputIndex = 0;
				memset(cInputString, 0x00, cmdMAX_INPUT_SIZE);

				HAL_UART_Transmit(&huart1, (uint8_t *) pcPrompt,
						strlen(pcPrompt), 0xFFFF);
			} else if ((cRxBuffer[i] == '\b') || (*cRxBuffer == cmdASCII_DEL)) {
				/* Backspace was pressed.  Erase the last character in the
				 string - if any. */
				if (ucInputIndex > 0) {
					HAL_UART_Transmit(&huart1, (uint8_t *) pcBackSpace,
							strlen(pcBackSpace), 0xFFFF);
					ucInputIndex--;
					cInputString[ucInputIndex] = '\0';
				}
			} else if ((cRxBuffer[i] >= ' ') && (cRxBuffer[i] <= '~')) {
				/* A character was entered.  Add it to the string entered so
				 far.  When a \n is entered the complete	string will be
				 passed to the command interpreter. */
				if (ucInputIndex < cmdMAX_INPUT_SIZE) {
					HAL_UART_Transmit(&huart1, (uint8_t *) (cRxBuffer + i), 1,
							0xFFFF);
					cInputString[ucInputIndex] = cRxBuffer[i];
					ucInputIndex++;
				}
			} else if (cRxBuffer[i] == cmdASCII_ETX) {
				//Notify("%c",0x03);
			}
			osSemaphoreRelease(uartTxSemHandle);
		}

	}
  /* USER CODE END runShell */
}

/* USER CODE BEGIN Header_runExec */
/**
 * @brief Function implementing the ExecThread thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_runExec */
void runExec(void const * argument)
{
  /* USER CODE BEGIN runExec */
	osEvent evt;
	uint8_t data[2] = { 0 };
	/* Infinite loop */
	for (;;) {
		evt = osSignalWait(0x00, osWaitForever);
		switch (HIGH_U16(evt.value.signals)) {
		case SIGNAL_ERROR:
			Notify("[notify] MCU_CPLD_ERR Detected");
			*data = 0x00;
			*(data + 1) = 0x00;
			if (HAL_I2C_Mem_Read(&hi2c1, 0x17 << 1, 0x06, 1, data, 2, 100)
					!= HAL_OK)
				Error("[notify] I2C Read Error");
			Notify("[notify] REG[0x06]=0b%08b\r\n[notify] REG[0x07]=0b%08b",
					*data, *(data + 1));
			break;
		case SIGNAL_KERNAL:
			if (LOW_U16(evt.value.signals) == 0x01)
				tStage = STAGE_KERNAL;
			break;

		}
	}
  /* USER CODE END runExec */
}

/* ledCtrlCallback function */
void ledCtrlCallback(void const * argument)
{
  /* USER CODE BEGIN ledCtrlCallback */

  /* USER CODE END ledCtrlCallback */
}

/* timeoutCallback function */
void timeoutCallback(void const * argument)
{
  /* USER CODE BEGIN timeoutCallback */
	//osSignalSet(ShellThreadHandle, 0x01);
  /* USER CODE END timeoutCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//	if (huart->Instance == huart1.Instance) {
//		osSignalSet(ShellThreadHandle, 0x01);
//	}
//}
void UARTStr_RxCpltCallback(uint8_t size) {
	osSignalSet(ShellThreadHandle, size);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == MCU_CPLD_ERR_Pin) {
		osSignalSet(ExecThreadHandle, FORM_U16(SIGNAL_ERROR, 0x00));
		//Notify("[notify] MCU_CPLD_ERR Detected");
	}
//		Debug("full in detected");
//		if (osMessagePut(msgQueueHandle, FORM_U16(MSG_FULLIN, 0x01),
//				osWaitForever) != osOK) {
//			Error("msg put fail: full in detected");
//		}
//	} else if (GPIO_Pin == AIC_FULLIN1_Pin) {
//		Debug("remove detected");
//		if (osMessagePut(msgQueueHandle, FORM_U16(MSG_FULLIN, 0x00),
//				osWaitForever) != osOK) {
//			Error("msg put fail: remove detected");
//		}
//	}
}

void Notify(const char *fmt, ...) {
	osSemaphoreWait(uartTxSemHandle, osWaitForever);
	char notifyMsg[79] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(notifyMsg, fmt, args);
	va_end(args);
	HAL_UART_Transmit(&huart1, (uint8_t*) "\r\n", 2, 0xFFFF);
	HAL_UART_Transmit(&huart1, (uint8_t*) notifyMsg, strlen(notifyMsg), 0xFFFF);
	if (cmdRunning == 0) {
		HAL_UART_Transmit(&huart1, (uint8_t *) pcPrompt, strlen(pcPrompt), 0xFFFF);
		if (ucInputIndex > 0)
			HAL_UART_Transmit(&huart1, (uint8_t *) cInputString,
					strlen(cInputString), 0xFFFF);
	}
	osSemaphoreRelease(uartTxSemHandle);
}

void I2CReg_SlaveTxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	Notify("Slave Tx triggered\r\n");
}

void I2CReg_SlaveRxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	switch (addr) {
	case 0x00:
		osSignalSet(ExecThreadHandle, FORM_U16(SIGNAL_KERNAL, reg[0x00]));
	}
	Notify("Slave Rx triggered\r\n");
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
