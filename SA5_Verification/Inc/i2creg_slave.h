/*****************************************************************************
 * 
 * @filepath 	/SA5_Verification/Inc/i2creg_slave.h
 * 
 * @project 	SA5_Verification
 * 
 * @brief 		I2C register-like Slave
 * 
 * @date 		Jun 11, 2019
 * 
 * @author		ziyi.xiong
 *
 ****************************************************************************/
#ifndef I2CREG_SLAVE_H_
#define I2CREG_SLAVE_H_

/** Includes **/
#include "main.h"
/** Declarations **/

/** Prototypes **/
HAL_StatusTypeDef I2CReg_SlaveInit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint8_t size);
HAL_StatusTypeDef I2CReg_SlaveAbort(I2C_HandleTypeDef *hi2c);
void I2CReg_AddrCallback(I2C_HandleTypeDef *hi2c);
void I2CReg_SlaveTxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr);
void I2CReg_SlaveRxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr);

#endif /* I2CREG_SLAVE_H_ */
