/** @addtogroup flash_file
 *
 */

/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Thomas Otto <tommi@viadmin.org>
 * Copyright (C) 2010 Mark Butler <mbutler@physics.otago.ac.nz>
 * Copyright (C) 2012-13 Karl Palsson <karlp@tweak.net.au>
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

/* Flash routines shared by L0/L1.  L4 has a different flash controller */

/**@{*/

#include <libopencm3/stm32/flash.h>


/**
 * Unlock primary access to the flash control/erase block
 * You must call this before using any of the low level routines
 * yourself.
 * @sa flash_unlock
 */
void flash_unlock_pecr(void)
{
	FLASH_PEKEYR = FLASH_PEKEYR_PEKEY1;
	FLASH_PEKEYR = FLASH_PEKEYR_PEKEY2;
}

void flash_lock_pecr(void)
{
	FLASH_PECR |= FLASH_PECR_PELOCK;
}

/**
 * Unlock program memory itself.
 * Writes the magic sequence to unlock the program memory
 * you must have already unlocked access to this register!
 * @sa flash_unlock_pecr
 */
void flash_unlock_progmem(void)
{
	FLASH_PRGKEYR = FLASH_PRGKEYR_PRGKEY1;
	FLASH_PRGKEYR = FLASH_PRGKEYR_PRGKEY2;
}

void flash_lock_progmem(void)
{
	FLASH_PECR |= FLASH_PECR_PRGLOCK;
}

void flash_lock_option_bytes(void)
{
	FLASH_PECR |= FLASH_PECR_OPTLOCK;
}

/** @brief Unlock all segments of flash
 *
 */
void flash_unlock(void)
{
	flash_unlock_pecr();
	flash_unlock_progmem();
	flash_unlock_option_bytes();
}

/** @brief Lock all segments of flash
 *
 */
void flash_lock(void)
{
	flash_lock_option_bytes();
	flash_lock_progmem();
	flash_lock_pecr();
}

/** @brief Unlock RUN_PD bit from FLASH_ACR register.
 */
void flash_unlock_acr(void)
{
	FLASH_PDKEYR = FLASH_PDKEYR_PDKEY1;
	FLASH_PDKEYR = FLASH_PDKEYR_PDKEY2;
}

/** @brief Write a word to eeprom
 *
 * @param address assumed to be in the eeprom space, no checking
 * @param data word to write
 */
void eeprom_program_word(uint32_t address, uint32_t data)
{
	flash_unlock_pecr();
	/* erase only if needed */
	FLASH_PECR &= ~FLASH_PECR_FTDW;
	MMIO32(address) = data;
	/* wait eeprom program done */
	while (FLASH_SR & FLASH_SR_BSY);
	flash_lock_pecr();
}

/** @brief Write a block of words to eeprom
 *
 * Writes a block of words to EEPROM at the requested address, erasing if
 * necessary, and locking afterwards.  Only wordwise writing is safe for
 * writing any value
 *
 * @param[in] address must point to EEPROM space, no checking!
 * @param[in] data pointer to data to write
 * @param[in] length_in_words size of of data in WORDS!
 */
void eeprom_program_words(uint32_t address, uint32_t *data, int length_in_words)
{
	int i;
	flash_unlock_pecr();
	while (FLASH_SR & FLASH_SR_BSY);
	/* erase only if needed */
	FLASH_PECR &= ~FLASH_PECR_FTDW;
	for (i = 0; i < length_in_words; i++) {
		MMIO32(address + (i * sizeof(uint32_t))) = *(data+i);
		while (FLASH_SR & FLASH_SR_BSY);
	}
	flash_lock_pecr();
}

/** @brief Write a byte to eeprom
 *
 * @param address assumed to be in the eeprom space, no checking
 * @param data byte to write
 */
void eeprom_program_byte(uint32_t address, uint8_t data)
{
	flash_unlock_pecr();
	/* erase only if needed */
	FLASH_PECR &= ~FLASH_PECR_FTDW;
	MMIO8(address) = data;
	/* wait eeprom program done */
	while (FLASH_SR & FLASH_SR_BSY);
	flash_lock_pecr();
}

/** @brief Write a block of byte to eeprom
 *
 * Writes a block of byte to EEPROM at the requested address, erasing if
 * necessary, and locking afterwards.  Only wordwise writing is safe for
 * writing any value
 *
 * @param[in] address must point to EEPROM space, no checking!
 * @param[in] data pointer to data to write
 * @param[in] length size of of data in BYTES!
 */
void eeprom_program_bytes(uint32_t address, uint32_t *data, int length)
{
	int i;
	flash_unlock_pecr();
	while (FLASH_SR & FLASH_SR_BSY);
	/* erase only if needed */
	FLASH_PECR &= ~FLASH_PECR_FTDW;
	for (i = 0; i < length; i++) {
		MMIO8(address + (i * sizeof(uint32_t))) = *(data+i);
		while (FLASH_SR & FLASH_SR_BSY);
	}
	flash_lock_pecr();
}

static inline int flash_is_busy(void)
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
	FLASH_PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG;
	MMIO32(addr & ~(FLASH_PAGE_SIZE - 1)) = 0x00000000;
	flash_wait_idle();
	FLASH_PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_ERASE);
}

void flash_program_page(uint32_t addr, void *data)
{
	uint8_t i;
	uint8_t *tmp;
	register uint32_t pc;
	int run_in_ram;

	flash_unlock();
	flash_erase_page(addr);
	flash_wait_idle();

	/* check if we need re-program this page */
	for (i = 0; i < FLASH_PAGE_SIZE; ++i) {
		if (((uint8_t *)data)[i])
			break;
	}
	if (i == FLASH_PAGE_SIZE) {
		/* all data is zero, just erase, do not program */
		flash_lock();
		return;
	}

	__asm__ volatile ("mov %0, pc" : "=r" (pc));
	run_in_ram = (pc >> 28) ? 1 : 0;

	/* half page program seems not working if we are running in flash */
	if (run_in_ram)
		FLASH_PECR |= FLASH_PECR_FPRG | FLASH_PECR_PROG;
	for (i = 0; i < FLASH_PAGE_SIZE; i += 4) {
		tmp = ((uint8_t *)data) + i;
		MMIO32(addr + i) = tmp[0] | (tmp[1] << 8) |
				(tmp[2] << 16) | (tmp[3] << 24);
	}

	if (run_in_ram)
		FLASH_PECR &= ~(FLASH_PECR_FPRG | FLASH_PECR_PROG);
	flash_wait_idle();
	flash_lock();
}

void flash_clear_error(void)
{
	/* clear all previous flash error */
	uint32_t error_mask =
		FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZEERR |
		FLASH_SR_OPTVERR | FLASH_SR_RDERR | FLASH_SR_FWWERR |
		FLASH_SR_NOTZEROERR;
	FLASH_SR |= error_mask;
}

/**@}*/
