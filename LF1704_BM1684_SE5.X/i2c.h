#ifndef __I2C_H__
#define __I2C_H__

#define CMD_REBOOT          7
#define CMD_POWER_OFF       2

void i2c_slave_init(void);
void i2c_slave_destroy(void);
void req_init(void);
void req_factory_reset(void);
void req_reboot(void);
void req_power_off(void);
void board_ctrl(void);
void power_acquire(void);
void power_init(void);
void flash_init(void);

#endif
