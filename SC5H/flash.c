#include <libopencm3/stm32/flash.h>
#include <stdint.h>
#include <flash.h>

#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEFU)
#define FLASH_PEKEY2               ((uint32_t)0x02030405U)

#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBFU)
#define FLASH_PRGKEY2              ((uint32_t)0x13141516U)

void flash_unlock(void)
{
	if (FLASH_PECR & FLASH_PECR_PRGLOCK) {
		if(FLASH_PECR & FLASH_PECR_PELOCK) {
			FLASH_PEKEYR = FLASH_PEKEY1;
			FLASH_PEKEYR = FLASH_PEKEY2;
		}
		FLASH_PRGKEYR = FLASH_PRGKEY1;
		FLASH_PRGKEYR = FLASH_PRGKEY2;
	}
}

void flash_lock(void)
{
	FLASH_PECR |= FLASH_PECR_PRGLOCK;
}

int flash_is_busy(void)
{
	uint32_t error_mask =
		FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZEERR |
		FLASH_SR_OPTVERR | FLASH_SR_RDERR | FLASH_SR_FWWERR |
		FLASH_SR_NOTZEROERR;
	uint32_t sr = FLASH_SR;

	/* clear all previous flash error */
	if (sr & error_mask)
		FLASH_SR |= error_mask;

	if (FLASH_SR & FLASH_SR_EOP)
		FLASH_SR = FLASH_SR_EOP;

	return sr & FLASH_SR_BSY;
}

int flash_wait_idle(void)
{
	unsigned int time = 0x3fff;
	do {
		if (!flash_is_busy())
			return 0;
		time--;
	} while (time);
	return -1;
}

void flash_erase_page(uint32_t addr)
{
	FLASH_PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG;
	*(volatile uint32_t *)(addr & ~(FLASH_PAGE_SIZE - 1)) = 0x00000000;
	flash_wait_idle();
	FLASH_PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_ERASE);
}

void flash_write32(uint32_t addr,uint32_t data)
{
	flash_unlock();
	flash_erase_page(addr);
	flash_wait_idle();
	*(volatile uint32_t *)addr = data;
	flash_wait_idle();
	flash_lock();
}

void flash_write_page(uint32_t addr, uint8_t *p, uint8_t len)
{
	uint32_t data;
	uint8_t cnt = len/4;
	uint8_t i = 0;

	flash_unlock();
	flash_erase_page(addr);
	flash_wait_idle();
	/* half page program seems not working if we are running in flash */
	/* FLASH_PECR |= FLASH_PECR_FPRG | FLASH_PECR_PROG; */
	while (cnt--) {
		data = *(p++);
		data |= *(p++)<< 8;
		data |= *(p++)<< 16;
		data |= *(p++)<< 24;

		*(volatile uint32_t *)(addr + (4 * i)) = data;
		++i;
	}
	/* FLASH_PECR &= ~(FLASH_PECR_FPRG | FLASH_PECR_PROG); */
	flash_wait_idle();
	flash_lock();
}

void flash_program(unsigned long flash_addr, void *data, int len)
{
#if 0
	if (offset >= FLASH_PROGRAM_END)
		return;
	uint32_t flash_addr = (uint32_t)FLASH_PROGRAM_START + offset;
#endif
	flash_write_page(flash_addr & ~FLASH_PAGE_MASK, data, len);
}

void flash_init(void)
{
	/* clear all previous flash error */
	uint32_t error_mask =
		FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZEERR |
		FLASH_SR_OPTVERR | FLASH_SR_RDERR | FLASH_SR_FWWERR |
		FLASH_SR_NOTZEROERR;
	FLASH_SR |= error_mask;
}

