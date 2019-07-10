/*****************************************************************************
 * @filename 	uart_streaming.h
 * 
 * @brief 		UART receive unknown length message
 * 
 * @date 		Jun 11, 2019
 * 
 * @author		ziyi.xiong
 *
 ****************************************************************************/
#ifndef UART_STREAMING_H_
#define UART_STREAMING_H_

/** Includes **/
#include "main.h"
/** Declarations **/
HAL_StatusTypeDef UARTStr_RxInit(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size);
HAL_StatusTypeDef UARTStr_RxAbort(UART_HandleTypeDef *huart);
void UARTStr_RxCpltCallback(uint8_t size);
/** Prototypes **/


#endif /* UART_STREAMING_H_ */
