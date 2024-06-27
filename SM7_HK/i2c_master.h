#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#include <libopencm3/stm32/i2c.h>
/* use I2C1 I2C2 ... as function first parameter */

#include "pin.h"

#define IIC_SCL   GPIO0
#define IIC_SDA   GPIO1
#define IIC_SCL_PORT  GPIOA
#define IIC_SDA_PORT GPIOA


#define IIC_SCL_1    gpio_set(IIC_SCL_PORT, IIC_SCL)            /* SCL = 1 */
#define IIC_SCL_0    gpio_clear(IIC_SCL_PORT, IIC_SCL)        /* SCL = 0 */

#define IIC_SDA_1    gpio_set(IIC_SDA_PORT, IIC_SDA)            /* SDA = 1 */
#define IIC_SDA_0    gpio_clear(IIC_SDA_PORT, IIC_SDA)        /* SDA = 0 */

#define IIC_READ_SDA()    gpio_get(IIC_SDA_PORT, IIC_SDA)    /* ?SDA???? */

void IIC_Start(void);

int i2c_master_init(int i2c);
int i2c2_master_init(int i2c);
int i2c_master_destroy(int i2c);
int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn, void *r, unsigned int rn);
int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data);
int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data);
int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data);
int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data);

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data);

#endif
