/*
 * flash_if.h
 *
 *  Created on: 2019Äê7ÔÂ29ÈÕ
 *      Author: taiqiang.cao
 */

#ifndef FLASH_IF_H_
#define FLASH_IF_H_

#include "main.h"


#define FLASH_USER_START_ADDR   (FLASH_BASE + FLASH_PAGE_SIZE * 256)       /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     FLASH_USER_START_ADDR + FLASH_PAGE_SIZE * 256   /* End @ of user Flash area */


#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEFU)
#define FLASH_PEKEY2               ((uint32_t)0x02030405U)


void STMFLASH_Unlock(void);
void STMFLASH_lock(void);
void STMFLASH_Write(uint32_t WriteAddr,uint32_t Data);
uint32_t STMFLASH_Read(uint32_t ReadAddr);
void STMFLASH_Write_Page(uint32_t WriteAddr, uint8_t *p, uint8_t len);

#endif /* FLASH_IF_H_ */
