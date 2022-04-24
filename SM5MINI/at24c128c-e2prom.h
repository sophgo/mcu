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
#define AT24C128C_EEPROM_SIZE   (256)
#define AT24C128C_PAGE_SIZE	(64)

#define PWROFF_TIMER_OFFSET_H	(0x42)
#define PWROFF_TIMER_OFFSET_L	(0x43)

bool is_se6ctrl_board(void);
void at24c128c_init(struct i2c_slave_ctx *i2c);
uint16_t at24c128c_get_pwroff_timer(void);
int at24c128c_read_byte(void *priv, unsigned int offset);

#endif
