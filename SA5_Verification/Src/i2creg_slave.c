/*****************************************************************************
 * 
 * @filepath 	/SA5_Verification/Src/i2creg_slave.c
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
/** Includes **/
#include "i2creg_slave.h"

/** Declarations **/
static HAL_StatusTypeDef I2CReg_SlaveISR(I2C_HandleTypeDef *hi2c,
		uint32_t ITFlags, uint32_t ITSources);
static void I2CReg_AddrMatch(I2C_HandleTypeDef *hi2c);
static void I2CReg_SlaveProcessDone(I2C_HandleTypeDef *hi2c, uint8_t addr);
static void I2CReg_EnableIRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest);
static void I2CReg_DisableIRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest);
static void I2CReg_FlushTXDR(I2C_HandleTypeDef *hi2c);

#define I2C_XFER_TX_IT          (0x00000001U)
#define I2C_XFER_RX_IT          (0x00000002U)
#define I2C_XFER_LISTEN_IT      (0x00000004U)
#define I2C_NO_OPTION_FRAME     (0xFFFF0000U)
#define I2C_XFER_CPLT_IT        (0x00000012U)

/** Source Code **/
//register slave initial
HAL_StatusTypeDef I2CReg_SlaveInit(I2C_HandleTypeDef *hi2c, uint8_t *pData,
		uint8_t size) {
	/* Process Locked */
	__HAL_LOCK(hi2c);
	if ((uint32_t) hi2c->State == HAL_I2C_STATE_READY) {

	hi2c->State = HAL_I2C_STATE_LISTEN;
	hi2c->Mode = HAL_I2C_MODE_SLAVE;
	hi2c->ErrorCode = HAL_I2C_ERROR_NONE;

	/* Enable Address Acknowledge */
	hi2c->Instance->CR2 &= ~I2C_CR2_NACK;

	/* Prepare transfer parameters */
	hi2c->pBuffPtr = pData;
	hi2c->XferCount = 0xFFFF;
	hi2c->XferSize = size;
	hi2c->XferOptions = I2C_NO_OPTION_FRAME;
	hi2c->XferISR = I2CReg_SlaveISR;
	} else {
		return HAL_BUSY;
	}
	/* Process Unlocked */
	__HAL_UNLOCK(hi2c);

	/* Note : The I2C interrupts must be enabled after unlocking current process
	 to avoid the risk of I2C interrupt handle execution before current
	 process unlock */
	/* REnable ADDR interrupt */
	I2CReg_EnableIRQ(hi2c, I2C_XFER_RX_IT | I2C_XFER_TX_IT | I2C_XFER_LISTEN_IT);

	return HAL_OK;
}

HAL_StatusTypeDef I2CReg_SlaveAbort(I2C_HandleTypeDef *hi2c) {
	/* Disable all interrupts */
	I2CReg_DisableIRQ(hi2c, I2C_XFER_LISTEN_IT | I2C_XFER_TX_IT | I2C_XFER_RX_IT);
	/* Disable Address Acknowledge */
	hi2c->Instance->CR2 |= I2C_CR2_NACK;
	/* Clear Configuration Register 2 */
	I2C_RESET_CR2(hi2c);
	/* Flush TX register */
	I2CReg_FlushTXDR(hi2c);
	/* Reset I2C handle */
	hi2c->State = HAL_I2C_STATE_READY;
	hi2c->Mode = HAL_I2C_MODE_NONE;
	hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
	hi2c->pBuffPtr = NULL;
	hi2c->XferCount = 0;
	hi2c->XferSize = 0;
	hi2c->XferOptions = I2C_NO_OPTION_FRAME;
	hi2c->XferISR = NULL;

	__HAL_UNLOCK(hi2c);
	return HAL_OK;
}

static HAL_StatusTypeDef I2CReg_SlaveISR(I2C_HandleTypeDef *hi2c,
		uint32_t ITFlags, uint32_t ITSources) {

	/* Process locked */
	__HAL_LOCK(hi2c);
	static uint8_t addr;

	if ((I2C_CHECK_FLAG(ITFlags, I2C_FLAG_AF) != RESET)
			&& (I2C_CHECK_IT_SOURCE(ITSources, I2C_IT_NACKI) != RESET)) {
		/* Check that I2C transfer finished */
		/* if yes, normal use case, a NACK is sent by the MASTER when Transfer is finished */
		__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

	} else if ((I2C_CHECK_FLAG(ITFlags, I2C_FLAG_ADDR) != RESET)
				&& (I2C_CHECK_IT_SOURCE(ITSources, I2C_IT_ADDRI) != RESET)) {
			I2CReg_AddrMatch(hi2c);

	} else if ((I2C_CHECK_FLAG(ITFlags, I2C_FLAG_RXNE) != RESET)
			&& (I2C_CHECK_IT_SOURCE(ITSources, I2C_IT_RXI) != RESET)) {
		/* incoming bytes */
		ITFlags &= ~I2C_FLAG_RXNE;
		/* Read data from RXDR */
		uint8_t buf = (uint8_t) hi2c->Instance->RXDR;
		if (hi2c->XferCount == 0xFFFF && buf <= hi2c->XferSize) { // first byte, reg addr
		/* Move the buffer ptr */
			addr = buf;
			hi2c->pBuffPtr += addr;
			hi2c->XferCount = addr;
		} else if (hi2c->XferCount != 0xFFFF
				/*&& hi2c->XferCount < hi2c->XferSize*/) { // write reg
				/* Increment Buffer pointer */
			*hi2c->pBuffPtr = buf;
			hi2c->pBuffPtr++;
			hi2c->XferCount++;
			if (hi2c->State != HAL_I2C_STATE_BUSY_TX_LISTEN)
				hi2c->State = HAL_I2C_STATE_BUSY_RX_LISTEN;
		} else { // wrong reg addr or out of bound
			__HAL_I2C_GENERATE_NACK(hi2c);
		}

	} else if ((I2C_CHECK_FLAG(ITFlags, I2C_FLAG_TXIS) != RESET)
			&& (I2C_CHECK_IT_SOURCE(ITSources, I2C_IT_TXI) != RESET)) {
		/* incoming bytes */
		ITFlags &= ~I2C_FLAG_TXIS;
		if (hi2c->XferCount == 0xFFFF)
			hi2c->XferCount = 0; // start from beginning
		if (1/*hi2c->XferCount < hi2c->XferSize*/) { // or a specific addr
			hi2c->Instance->TXDR = *hi2c->pBuffPtr;
			/* Increment Buffer pointer */
			hi2c->pBuffPtr++;
			hi2c->XferCount++;
			hi2c->State = HAL_I2C_STATE_BUSY_TX_LISTEN;
		} else { // out of bound or no reg selected
			__HAL_I2C_GENERATE_NACK(hi2c);
		}
	}

	/* Check if STOPF is set */
	if ((I2C_CHECK_FLAG(ITFlags, I2C_FLAG_STOPF) != RESET)
			&& (I2C_CHECK_IT_SOURCE(ITSources, I2C_IT_STOPI) != RESET)) {
		/* Call I2C Slave complete process */
		I2CReg_SlaveProcessDone(hi2c, addr);
	}

	/* Process Unlocked */
	__HAL_UNLOCK(hi2c);

	return HAL_OK;
}

static void I2CReg_AddrMatch(I2C_HandleTypeDef *hi2c) {
	__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ADDR);
	__HAL_UNLOCK(hi2c);
	I2CReg_AddrCallback(hi2c);

}

static void I2CReg_SlaveProcessDone(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	/* Clear STOP Flag */
	__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);
	/* Reset handle */
	hi2c->pBuffPtr -= hi2c->XferCount;
	hi2c->XferCount = 0xFFFF;
	I2CReg_FlushTXDR(hi2c);
	__HAL_UNLOCK(hi2c);

	if (hi2c->State == HAL_I2C_STATE_BUSY_TX_LISTEN) {
		I2CReg_SlaveTxCallback(hi2c, addr);
	} else if (hi2c->State == HAL_I2C_STATE_BUSY_RX_LISTEN) {
		I2CReg_SlaveRxCallback(hi2c, addr);
	}
	hi2c->State = HAL_I2C_STATE_LISTEN;


}

static void I2CReg_EnableIRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest) {
	uint32_t tmpisr = 0U;
	{
		if ((InterruptRequest & I2C_XFER_LISTEN_IT) == I2C_XFER_LISTEN_IT) {
			/* Enable ERR, STOP, NACK, and ADDR interrupts */
			tmpisr |= I2C_IT_ADDRI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
		}

		if ((InterruptRequest & I2C_XFER_TX_IT) == I2C_XFER_TX_IT) {
			/* Enable ERR, TC, STOP, NACK and RXI interrupts */
			tmpisr |= I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI
					| I2C_IT_TXI;
		}

		if ((InterruptRequest & I2C_XFER_RX_IT) == I2C_XFER_RX_IT) {
			/* Enable ERR, TC, STOP, NACK and TXI interrupts */
			tmpisr |= I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI
					| I2C_IT_RXI;
		}

		if ((InterruptRequest & I2C_XFER_CPLT_IT) == I2C_XFER_CPLT_IT) {
			/* Enable STOP interrupts */
			tmpisr |= I2C_IT_STOPI;
		}
	}

	/* Enable interrupts only at the end */
	/* to avoid the risk of I2C interrupt handle execution before */
	/* all interrupts requested done */
	__HAL_I2C_ENABLE_IT(hi2c, tmpisr);
}

static void I2CReg_DisableIRQ(I2C_HandleTypeDef *hi2c, uint16_t InterruptRequest) {
	uint32_t tmpisr = 0U;

	if ((InterruptRequest & I2C_XFER_TX_IT) == I2C_XFER_TX_IT) {
		/* Disable TC and TXI interrupts */
		tmpisr |= I2C_IT_TCI | I2C_IT_TXI;

		if (((uint32_t) hi2c->State & (uint32_t) HAL_I2C_STATE_LISTEN)
				!= (uint32_t) HAL_I2C_STATE_LISTEN) {
			/* Disable NACK and STOP interrupts */
			tmpisr |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
		}
	}

	if ((InterruptRequest & I2C_XFER_RX_IT) == I2C_XFER_RX_IT) {
		/* Disable TC and RXI interrupts */
		tmpisr |= I2C_IT_TCI | I2C_IT_RXI;

		if (((uint32_t) hi2c->State & (uint32_t) HAL_I2C_STATE_LISTEN)
				!= (uint32_t) HAL_I2C_STATE_LISTEN) {
			/* Disable NACK and STOP interrupts */
			tmpisr |= I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
		}
	}

	if ((InterruptRequest & I2C_XFER_LISTEN_IT) == I2C_XFER_LISTEN_IT) {
		/* Disable ADDR, NACK and STOP interrupts */
		tmpisr |= I2C_IT_ADDRI | I2C_IT_STOPI | I2C_IT_NACKI | I2C_IT_ERRI;
	}

	if ((InterruptRequest & I2C_XFER_CPLT_IT) == I2C_XFER_CPLT_IT) {
		/* Enable STOP interrupts */
		tmpisr |= I2C_IT_STOPI;
	}

	/* Disable interrupts only at the end */
	/* to avoid a breaking situation like at "t" time */
	/* all disable interrupts request are not done */
	__HAL_I2C_DISABLE_IT(hi2c, tmpisr);
}

static void I2CReg_FlushTXDR(I2C_HandleTypeDef *hi2c) {
	/* If a pending TXIS flag is set */
	/* Write a dummy data in TXDR to clear it */
	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXIS) != RESET) {
		hi2c->Instance->TXDR = 0x00U;
	}

	/* Flush TX register if not empty */
	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET) {
		__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_TXE);
	}
}

__weak void I2CReg_AddrCallback(I2C_HandleTypeDef *hi2c) {
	UNUSED(hi2c);
}
__weak void I2CReg_SlaveTxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	UNUSED(hi2c);
}
__weak void I2CReg_SlaveRxCallback(I2C_HandleTypeDef *hi2c, uint8_t addr) {
	UNUSED(hi2c);
}
