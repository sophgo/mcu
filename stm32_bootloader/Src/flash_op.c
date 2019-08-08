/*
 * flash_op.c
 *
 *  Created on: 2019Äê8ÔÂ1ÈÕ
 *      Author: taiqiang.cao
 */
#include "stm32l0xx_hal.h"
#include "flash_op.h"

//extern FLASH_ProcessTypeDef pFlash;
extern void    FLASH_PageErase(uint32_t PageAddress);

//HAL_StatusTypeDef HAL_FLASH_ProgramHalfWord(uint32_t Address, uint8_t Data)
//{
//  HAL_StatusTypeDef status = HAL_ERROR;
//
//  /* Process Locked */
//  __HAL_LOCK(&pFlash);
//
//  /* Check the parameters */
////  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));
//  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));
//
//  /* Wait for last operation to be completed */
//  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
//
//  if(status == HAL_OK)
//  {
//    /* Clean the error context */
//    pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;
//
//    /*Program word (32-bit) at a specified address.*/
//    *(__IO uint32_t *)Address = Data;
//
//    /* Wait for last operation to be completed */
//    status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
//  }
//
//  /* Process Unlocked */
//  __HAL_UNLOCK(&pFlash);
//
//  return status;
//}
//
//void FLASH_WriteByte(uint32_t addr, uint8_t *p, uint16_t Byte_Num)
//{
//	uint32_t HalfWord;
//	Byte_Num = Byte_Num/2;
//
//	HAL_FLASH_Unlock();
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
//	FLASH_PageErase(addr);
//	while(Byte_Num--)
//	{
//		HalfWord = *(p++);
//		HalfWord |= *(p++)<<8;
//		HAL_FLASH_ProgramHalfWord(addr, HalfWord);
//		addr += 2;
//	}
//	HAL_FLASH_Lock();
//}

void FLASH_WriteWord(uint32_t addr, uint8_t *p, uint16_t Byte_Num)
{
	uint32_t Word;
	Byte_Num = Byte_Num/4;
	uint8_t ret;

	ret = HAL_FLASH_Unlock();
	if (ret == HAL_ERROR)
		while(1);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR);
	FLASH_PageErase(addr);
	while(Byte_Num--)
	{
		Word = *(p++);
		Word |= *(p++)<<8;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr, Word);
		addr += 4;
	}
	HAL_FLASH_Lock();
}

void FLASH_ReadByte(uint32_t addr, uint8_t *p, uint16_t Byte_Num)
{
	while(Byte_Num--)
	{
		*(p++) = *((uint8_t*)addr++);
	}
}
