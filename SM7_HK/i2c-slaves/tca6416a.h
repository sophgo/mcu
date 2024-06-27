#ifndef __TCA6416A_H__
#define __TCA6416A_H__

#include <i2c_slave.h>

#define TCA6416A_P0_IN		0
#define TCA6416A_P1_IN		1
#define TCA6416A_P0_OUT		2
#define TCA6416A_P1_OUT		3
#define TCA6416A_P0_POL		4
#define TCA6416A_P1_POL		5
#define TCA6416A_P0_CFG		6
#define TCA6416A_P1_CFG		7
#define TCA6416A_REG_MAX	8
#define TCA6416A_REG_MASK	(TCA6416A_REG_MAX - 1)

int tca6416a_probe(void);
int tca6416a_write(uint8_t reg, uint8_t val);
int tca6416a_read(uint8_t reg);
int tca6416a_get(uint8_t port, uint8_t pin);
void tca6416a_set(uint8_t port, uint8_t pin);
void tca6416a_clr(uint8_t port, uint8_t pin);
int tca6416a_available(void);
int tca6416a_init(struct i2c_slave_ctx *i2c);

#endif
