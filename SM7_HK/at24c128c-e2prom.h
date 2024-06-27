#ifndef __AT24C08D_E2PROM_H__
#define __AT24C08D_E2PROM_H__

/*
 *
 *offset:0x00 --se6ctr sn	32 bytes
 *offset:0x20 --se6 sn		32 bytes
 *offset:0x40 --product flag	2 bytes
 *offset:0x42 --power off timer	2 bytes
 *
 */
#define AT24_EEPROM_SIZE   (256)
#define AT24C128C_PAGE_SIZE	(64)
#define AT24C01D_PAGE_SIZE  (8)

#define AT24C128C 0
#define AT24C01D 1

#define PWROFF_TIMER_OFFSET_H	(0x42)
#define PWROFF_TIMER_OFFSET_L	(0x43)

uint8_t get_eeprom_type(void);
bool is_se6ctrl_board(void);
void at24c128c_init(struct i2c_slave_ctx *i2c);
void at24c01d_init(struct i2c_slave_ctx *i2c);
uint16_t at24_get_pwroff_timer(void);

#endif
