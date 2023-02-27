/*
 * eeprom.h
 *
 *  Created on: 2019Äê4ÔÂ10ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include <sys/_stdint.h>
#include "stm32l0xx_hal.h"

/*
 * EEPROM read data by Bytes & Words
 */
void EEPROM_ReadBytes(uint16_t Addr, uint8_t *Buffer, uint16_t Length);
void EEPROM_ReadWords(uint16_t Addr, uint16_t *Buffer, uint16_t Length);

#define EN_INT __enable_irq();
#define DIS_INT __disable_irq();

#define PEKEY1 0x89ABCDEF //FLASH_PEKEYR
#define PEKEY2 0x02030405  //FLASH_PEKEYR

/*
 * EEPROM write data by Bytes & Words
 */
void EEPROM_WriteBytes(uint16_t Addr, uint8_t *Buffer, uint16_t Length);
void EEPROM_WriteWords(uint16_t Addr, uint16_t *Buffer, uint16_t Length);
void EEPROM_Write(uint32_t Addr, uint32_t writeFlashData);


#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H_ */
