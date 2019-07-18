/*
 * eeprom.c
 *
 *  Created on: 2019��4��10��
 *      Author: taiqiang.cao
 */

#include "eeprom.h"
#include "main.h"

/*
 * EEPROM read data by Bytes
 */
void EEPROM_ReadBytes(uint16_t Addr, uint8_t *Buffer, uint16_t Length)
{
	uint8_t *wAddr;
	wAddr = (uint8_t *)(EEPROM_BASE_ADDR + Addr);
	while(Length--){
		*Buffer++ = *wAddr++;
	}
}


/*
 * EEPROM read data by Words
 */
void EEPROM_ReadWords(uint16_t Addr, uint16_t *Buffer, uint16_t Length)
{
	uint32_t *wAddr;
	wAddr = (uint32_t *)(EEPROM_BASE_ADDR + Addr);
	while(Length--){
		*Buffer++ = *wAddr++;
	}
}

/*
 * EEPROM write data by Bytes
 */
void EEPROM_WriteBytes(uint16_t Addr, uint8_t *Buffer, uint16_t Length)
{
	uint8_t *wAddr;
	wAddr = (uint8_t *)(EEPROM_BASE_ADDR + Addr);
	DIS_INT
	FLASH->PEKEYR = PEKEY1;//unlock
	FLASH->PEKEYR = PEKEY2;
	while(FLASH->PECR&FLASH_PECR_PELOCK);
	FLASH->PECR |= FLASH_PECR_FIX;	//Fixed Time Data write for Word/Half Word/Byte programming
	while(Length--){
		*wAddr++ = *Buffer++;
		while(FLASH->SR&FLASH_SR_BSY);
	}
	FLASH->PECR|=FLASH_PECR_PELOCK;
	EN_INT
}


/*
 * EEPROM write data by Words
 */
void EEPROM_WriteWords(uint16_t Addr, uint16_t *Buffer, uint16_t Length)
{
	uint32_t *wAddr;
	wAddr = (uint32_t *)(EEPROM_BASE_ADDR + Addr);
	DIS_INT
	FLASH->PEKEYR = PEKEY1;//unlock
	FLASH->PEKEYR = PEKEY2;
	while(FLASH->PECR&FLASH_PECR_PELOCK);
	FLASH->PECR |= FLASH_PECR_FIX;	// Fixed Time Data write for Word/Half Word/Byte programming
	while(Length){
		*wAddr++=*Buffer++;
		while(FLASH->SR&FLASH_SR_BSY);
	}
	FLASH->PECR|=FLASH_PECR_PELOCK;
	EN_INT
}

void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = Addr;
	f.NbPages = 1;

	uint32_t PageError = 0;

	HAL_FLASHEx_Erase(&f, &PageError);

	HAL_FLASH_Program(TYPEPROGRAM_WORD, Addr, writeFlashData);

	HAL_FLASH_Lock();
}
