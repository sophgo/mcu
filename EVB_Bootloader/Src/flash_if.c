/*
 * flash_if.c
 *
 *  Created on: 2019年7月29日
 *      Author: taiqiang.cao
 */
/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"

#include "main.h"



//解锁STM32的FLASH
void STMFLASH_Unlock(void)
{
    if(FLASH->PECR&FLASH_PECR_PRGLOCK)
    {
        if(FLASH->PECR&FLASH_PECR_PELOCK)
        {
            FLASH->PEKEYR=FLASH_PEKEY1;//写入解锁序列.
            FLASH->PEKEYR=FLASH_PEKEY2;
        }
        FLASH->PRGKEYR=FLASH_PRGKEY1;//写入解锁序列.
        FLASH->PRGKEYR=FLASH_PRGKEY2;
    }
}
void STMFLASH_lock(void)
{
    FLASH->PECR|=FLASH_PECR_PRGLOCK;
}
//FLASH->SR & FLASH_FLAG_BSY
unsigned char STMFLASH_Status(void)
{
    unsigned char res = 0;
    if((FLASH->SR & FLASH_FLAG_BSY)==FLASH_FLAG_BSY )
    {
        res = 1;
    }
    if( ((FLASH->SR&FLASH_FLAG_WRPERR) ==FLASH_FLAG_WRPERR)  ||
        ((FLASH->SR&FLASH_FLAG_PGAERR) ==FLASH_FLAG_PGAERR)  ||
        ((FLASH->SR&FLASH_FLAG_SIZERR) ==FLASH_FLAG_SIZERR)  ||
        ((FLASH->SR&FLASH_FLAG_OPTVERR) ==FLASH_FLAG_OPTVERR)||
        ((FLASH->SR&FLASH_FLAG_RDERR) ==FLASH_FLAG_RDERR)    ||
        ((FLASH->SR&FLASH_FLAG_FWWERR) ==FLASH_FLAG_FWWERR)  ||
        ((FLASH->SR&FLASH_FLAG_NOTZEROERR) ==FLASH_FLAG_NOTZEROERR) )
    {
        res = 2;
    }

    return res;
}

unsigned char STMFlash_WaitDone(unsigned short time)
{
    unsigned char res;
    do
    {
        res = STMFLASH_Status();
        if(res != 1)
            break;
        time--;
    }while(time);
    if(0 == time)
        res = 0xff;

    return res;
}

unsigned char STMFlash_ErasePage(uint32_t WriteAddr)
{
    unsigned char res = 0;
//    uint32_t erase_addr = FLASH_USER_START_ADDR + (padd * FLASH_PAGE_SIZE);
    FLASH->PECR |= FLASH_PECR_ERASE;
    FLASH->PECR |= FLASH_PECR_PROG;
    *(__IO uint32_t *)(uint32_t)( WriteAddr & ~(FLASH_PAGE_SIZE - 1)) = 0x00000000;
    STMFlash_WaitDone(0x3fff);
    FLASH->PECR&=~FLASH_PECR_PROG;
    FLASH->PECR&=~FLASH_PECR_ERASE;
    return res;
}
void STMFLASH_Write(uint32_t WriteAddr,uint32_t Data)
{
    //(FLASH->ACR)&=~FLASH_ACR_DISAB_BUF;
    STMFLASH_Unlock();
    STMFlash_ErasePage(WriteAddr);
    STMFlash_WaitDone(0x3fff);
    *(__IO uint32_t *)WriteAddr = Data;
    STMFlash_WaitDone(0x3fff);
    STMFLASH_lock();
}
uint32_t STMFLASH_Read(uint32_t ReadAddr)
{
    uint32_t data32;
    data32 = *(__IO uint32_t *)ReadAddr;

    return data32;
}


void STMFLASH_Write_Page(uint32_t WriteAddr, uint8_t *p, uint8_t len)
{
	uint32_t data;
	uint8_t cnt = len/4;
	uint8_t i = 0;

    STMFLASH_Unlock();
    STMFlash_ErasePage(WriteAddr);
    STMFlash_WaitDone(0x3fff);
    while (cnt--) {
    	data = *(p++);
    	data |= *(p++)<< 8;
    	data |= *(p++)<< 16;
    	data |= *(p++)<< 24;

    	*(__IO uint32_t *)(WriteAddr + (4*i)) = data;
    	i++;
    }
    STMFlash_WaitDone(0x3fff);
    STMFLASH_lock();
}
