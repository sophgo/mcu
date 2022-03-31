#ifndef __AT24C08D_E2PROM_H__
#define __AT24C08D_E2PROM_H__

#define AT24C128C_EEPROM_SIZE   (256)
#define AT24C128C_PAGE_SIZE	(64)

bool is_se6ctrl_board(void);
void at24c128c_init(struct i2c_slave_ctx *i2c);

#endif
