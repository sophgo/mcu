#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

/* PIC 16F1704 program memory unit is word, but word is 14bits long,
 * drop MSB 6bits, just use LSB 8bits as a byte
 */

/* write data to high-endurance flash, count in words */
#define FLASH_ENDURANCE_BASE    0x0f80
#define FLASH_ENDURANCE_SIZE    128
#define FLASH_ENDURANCE_END     (FLASH_ENDURANCE_BASE + FLASH_ENDURANCE_SIZE)

/* 1row = 16words */
#define FLASH_ROW_SIZE          32
#define FLASH_ROW_MASK          (FLASH_ROW_SIZE - 1)

/* program only */
int flash_program_row(uint16_t offset, void *data);
int flash_erase_row(uint16_t offset);
int flash_read_byte(uint16_t offset);
int flash_read_row(uint16_t offset, void *data);
int flash_write_row(uint16_t offset, void *data);

#endif
