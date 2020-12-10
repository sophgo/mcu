#include "mcc_generated_files/mcc.h"
#include "flash.h"

/*
 * PIC F1704 program memory operations.
 * program memory unit is word, but this word is not 2bytes long, it is 14bits
 * long, erase min unit is row, 1row = 16words.
 * endurance memory is just a special program memory region,
 * it can be erased more time than normal region
 */

/* program only */

int flash_program_row(uint16_t offset, void *data)
{
    int i = 0;

    offset += FLASH_ENDURANCE_BASE;

    if (offset & FLASH_ROW_MASK)
        return -1;

    if (offset + FLASH_ROW_SIZE > FLASH_ENDURANCE_END)
        return -1;

    di();

    PMCON1bits.CFGS = 0;
    PMCON1bits.FREE = 0;
    PMCON1bits.LWLO = 1;
    PMCON1bits.WREN = 1;

    /* latch a row */
    /* drop MSB 6bits */
    PMADRH = offset >> 8;
    for (; i < FLASH_ROW_SIZE; ++i) {
        PMADRL = (offset + i) & 0xff;

        PMDATL = ((uint8_t *)data)[i];
        PMDATH = 0;

        /* magic code, unlock */
        PMCON2 = 0x55;
        PMCON2 = 0xaa;
        PMCON1bits.WR = 1;
        NOP();
        NOP();
    }
    /* flush data lached */
    PMCON1bits.LWLO = 0;
    PMCON2 = 0x55;
    PMCON2 = 0xaa;
    PMCON1bits.WR = 1;
    NOP();
    NOP();

    PMCON1bits.WREN = 0;

    ei();

    return 0;
}

int flash_erase_row(uint16_t offset)
{
    offset += FLASH_ENDURANCE_BASE;

    if (offset & FLASH_ROW_MASK)
        return -1;

    if (offset + FLASH_ROW_SIZE > FLASH_ENDURANCE_END)
        return -1;

    di();

    PMADRL = offset & 0xff;
    PMADRH = offset >> 8;
    PMCON1bits.CFGS = 0;
    PMCON1bits.FREE = 1;
    PMCON1bits.WREN = 1;
    PMCON2 = 0x55;
    PMCON2 = 0xaa;
    PMCON1bits.WR = 1;
    NOP();
    NOP();
    PMCON1bits.WREN = 0;

    ei();

    return 0;
}

int flash_read_byte(uint16_t offset)
{
    offset += FLASH_ENDURANCE_BASE;

    if (offset + 1 > FLASH_ENDURANCE_END)
        return -1;

    PMADRL = offset & 0xff;
    PMADRH = offset >> 8;
    PMCON1bits.CFGS = 0;
    PMCON1bits.RD = 1;
    NOP();
    NOP();

    /* ignore MSB 6bits */
    return PMDATL;
}

int flash_read_row(uint16_t offset, void *data)
{
    int i;

    if (offset & FLASH_ROW_MASK)
        return -1;

    if (offset + FLASH_ROW_SIZE > FLASH_ENDURANCE_SIZE)
        return -1;

    for (i = 0; i < FLASH_ROW_SIZE; ++i)
        ((uint8_t *)data)[i] = flash_read_byte(offset + i);

    return 0;
}

int flash_write_row(uint16_t offset, void *data)
{
    int err;

    err = flash_erase_row(offset);
    if (err)
        return err;

    return flash_program_row(offset, data);
}
