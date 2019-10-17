/*
 * i2c_slave.h
 *
 *  Created on: 2019Äê7ÔÂ27ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

/** Includes **/
#include "main.h"
/** Declarations **/

/** Prototypes **/
HAL_StatusTypeDef I2C_SlaveInit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint8_t size);
HAL_StatusTypeDef I2C_SlaveAbort(I2C_HandleTypeDef *hi2c);
void I2C_AddrCallback(I2C_HandleTypeDef *hi2c);
void I2C_SlaveTxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr);
void I2C_SlaveRxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr);


#endif /* I2C_SLAVE_H_ */
