/** @defgroup flash_file FLASH peripheral API
 *
 * @ingroup peripheral_apis
 *
 * @brief <b>libopencm3 STM32G0xx FLASH</b>
 *
 * @version 1.0.0
 *
 * LGPL License Terms @ref lgpl_license
 */

/*
 * This file is part of the libopencm3 project.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/**@{*/

#include <libopencm3/stm32/flash.h>

void flash_icache_enable(void)
{
	FLASH_ACR |= FLASH_ACR_ICEN;
}

void flash_icache_disable(void)
{
	FLASH_ACR &= ~FLASH_ACR_ICEN;
}

void flash_icache_reset(void)
{
	FLASH_ACR |= FLASH_ACR_ICRST;
}

void flash_unlock_progmem(void)
{
	FLASH_KEYR = FLASH_KEYR_KEY1;
	FLASH_KEYR = FLASH_KEYR_KEY2;
}

void flash_lock_progmem(void)
{
	FLASH_CR |= FLASH_CR_LOCK;
}

void flash_lock_option_bytes(void)
{
	FLASH_CR |= FLASH_CR_OPTLOCK;
}

void flash_unlock(void)
{
	flash_unlock_progmem();
	flash_unlock_option_bytes();
}

void flash_lock(void)
{
	flash_lock_option_bytes();
	flash_lock_progmem();
}
void flash_clear_error(void)
{
	/* clear all previous flash error */
	uint32_t error_mask =
		FLASH_SR_WRPERR | FLASH_SR_PGAERR |
		FLASH_SR_OPTVERR | FLASH_SR_RDERR;
	FLASH_SR |= error_mask;
}
static inline int flash_is_busy(void)
{
	uint32_t error_mask =
		FLASH_SR_WRPERR | FLASH_SR_PGAERR |
		FLASH_SR_OPTVERR | FLASH_SR_RDERR;
	uint32_t sr = FLASH_SR;

	/* clear all previous flash error */
	if (sr & error_mask)
		FLASH_SR |= error_mask;

	if (FLASH_SR & FLASH_SR_EOP)
		FLASH_SR = FLASH_SR_EOP;

	return sr & FLASH_SR_BSY;
}

static inline int flash_wait_idle(void)
{
	unsigned int time = 0x3fff;
	do {
		if (!flash_is_busy())
			return 0;
		time--;
	} while (time);
	return -1;
}

static inline void flash_erase_page(uint32_t addr)
{
	int ret = 0;
	int png = addr >> FLASH_PAGE_SIZE_SHIFT;

	ret = flash_wait_idle();
	if (ret == -1)
		return;

	FLASH_CR |= png<<FLASH_CR_PNB_SHIFT | 1<<FLASH_CR_PER_SHIFT;
	FLASH_CR |= 1<<FLASH_CR_STRT_SHIFT;

	ret = flash_wait_idle();
	if (ret == -1)
		return;
}
static inline int flash_clear_eop(void)
{
	if (FLASH_SR & FLASH_SR_EOP)
		FLASH_SR |= FLASH_SR_EOP;

	return 0;
}
void flash_program_page(uint32_t addr, void *data)
{
	uint64_t *p_data = (uint64_t *)data;
	int page_len = FLASH_PAGE_SIZE / 8;
	int i = 0;

	flash_unlock();
	flash_erase_page(addr);

	FLASH_CR |= FLASH_CR_PG;

	for (i = 0; i < page_len; i++){
		if (p_data[i])
			break;
	}
	if (i == page_len){
		flash_unlock();
		return;
	}

	for (i = 0; i < page_len; i++){
		MMIO64(addr+i*8) = p_data[i];
	}

	flash_wait_idle();
	flash_clear_eop();

	FLASH_CR |= ~FLASH_CR_PG;
}

/**@}*/