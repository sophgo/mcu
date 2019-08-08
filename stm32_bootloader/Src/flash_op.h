/*
 * flash_op.h
 *
 *  Created on: 2019Äê8ÔÂ1ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef FLASH_OP_H_
#define FLASH_OP_H_

//void FLASH_WriteByte(uint32_t addr, uint8_t *p, uint16_t Byte_Num);
void FLASH_WriteWord(uint32_t addr, uint8_t *p, uint16_t Byte_Num);
void FLASH_ReadByte(uint32_t addr, uint8_t *p, uint16_t Byte_Num);

#endif /* FLASH_OP_H_ */
