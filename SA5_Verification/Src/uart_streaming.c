/*****************************************************************************
 * @filename 	uart_streaming.c
 * 
 * @brief 		UART receive unknown length message
 * 
 * @date 		Jun 11, 2019
 * 
 * @author		ziyi.xiong
 *
 ****************************************************************************/
/** Includes **/
#include "uart_streaming.h"
#include "cmsis_os.h"
#include "timers.h"
#include <string.h>
/** Declarations **/
#define TIMEOUT 10
osTimerId UartTimeoutHandle;
extern UART_HandleTypeDef huart1;
static void UARTStr_RxISR(UART_HandleTypeDef *huart);
static void uartTimeoutCallback(void const * argument);

/** Source Code **/
HAL_StatusTypeDef UARTStr_RxInit(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size) {
	/* Check that a Rx process is not already ongoing */
	if (huart->RxState == HAL_UART_STATE_READY) {
		if ((pData == NULL) || (Size <= 0)) {
			return HAL_ERROR;
		}
		if (huart->Instance == USART1) {
			osTimerDef(UartTimeout, uartTimeoutCallback);
			UartTimeoutHandle = osTimerCreate(osTimer(UartTimeout), osTimerOnce, NULL);
		}

		/* Process Locked */
		__HAL_LOCK(huart);

		huart->pRxBuffPtr = pData;
		huart->RxXferSize = Size;
		huart->RxXferCount = 0;
		huart->RxISR = UARTStr_RxISR;

		/* Computation of UART mask to apply to RDR register */
		UART_MASK_COMPUTATION(huart);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;

		/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
		SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

//    /* Set the Rx ISR function pointer according to the data word length */
//    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
//    {
//      huart->RxISR = UART_RxISR_16BIT;
//    }
//    else
//    {
//      huart->RxISR = UART_RxISR_8BIT;
//    }

		/* Process Unlocked */
		__HAL_UNLOCK(huart);

		/* Enable the UART Parity Error interrupt and Data Register Not Empty interrupt */
		SET_BIT(huart->Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);

		return HAL_OK;
	} else {
		return HAL_BUSY;
	}
}

HAL_StatusTypeDef UARTStr_RxAbort(UART_HandleTypeDef *huart) {
	/* Disable the UART Parity Error Interrupt and RXNE interrupts */
	CLEAR_BIT(huart->Instance->CR1,
			(USART_CR1_RXNEIE | USART_CR1_PEIE));

	/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

	/* Rx process is completed, restore huart->RxState to Ready */
	huart->RxState = HAL_UART_STATE_READY;

	/* Clear RxISR function pointer */
	huart->RxISR = NULL;

	return HAL_OK;
}

static void UARTStr_RxISR(UART_HandleTypeDef *huart) {
	uint16_t uhMask = huart->Mask;
	uint16_t uhdata;
	//static uint8_t pBuffer[MAX_BUFFER_SIZE] = {0};

	/* Check that a Rx process is ongoing */
	if (huart->RxState == HAL_UART_STATE_BUSY_RX) {
		if (huart->RxXferCount <= huart->RxXferSize) { // aviod overflow
			uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
			*huart->pRxBuffPtr = (uint8_t) (uhdata & (uint8_t) uhMask);
			huart->pRxBuffPtr++;
			huart->RxXferCount++;
			if (huart->Instance == USART1)
				osTimerStart(UartTimeoutHandle, TIMEOUT);
//			else if (huart->Instance == USART2)
//				osTimerStart(Uart1TimeoutHandle, TIMEOUT);
//			else if (huart->Instance == USART4)
//				osTimerStart(Uart2TimeoutHandle, TIMEOUT);
		}
//		if ((uint16_t) READ_REG(huart->Instance->RDR) == 0)
//			UARTStr_RxCpltCallback(huart);
	} else {
		/* Clear RXNE interrupt flag */
		__HAL_UART_SEND_REQ(huart, UART_RXDATA_FLUSH_REQUEST);
	}
}
static void uartTimeoutCallback(void const * argument) {
//	uint8_t size = huart1->RxXferCount;
//	(UART_HandleTypeDef *)argument;
	if (argument == UartTimeoutHandle) {
		huart1.pRxBuffPtr -= huart1.RxXferCount;
		UARTStr_RxCpltCallback(huart1.RxXferCount);
		huart1.RxXferCount = 0;
		memset(huart1.pRxBuffPtr, 0x00, huart1.RxXferSize);
	}
}
__weak void UARTStr_RxCpltCallback(uint8_t size) {
	UNUSED(size);
}
