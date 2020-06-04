#ifndef __I2C_H__
#define __I2C_H__

void i2c_slave_init(void);
void i2c_slave_destroy(void);
void req_init(void);
void req_factory_reset(void);
void req_reboot(void);
void req_power_off(void);
void board_ctrl(void);

#endif
