#ifndef __TCA6416A_H__
#define __TCA6416A_H__

#include <stm32l0xx_hal.h>
#include <i2c.h>

#define TCA6416A_ADDR	(0x20 << 1)

#define TCA6416A_SMBTO	(1)

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

extern int is_tca6416a_available;
extern uint32_t dummy_tca6416a_regmap[TCA6416A_REG_MAX];

/* true if 6416 is active */
static inline int tca6416a_probe(void)
{
	int err;
	uint8_t tmp;

	err = HAL_I2C_Mem_Read(&hi2c1, TCA6416A_ADDR, TCA6416A_P0_CFG, 1,
			       &tmp, 1, 1);
	return is_tca6416a_available = (err == HAL_OK);
}

static inline int tca6416a_write(uint8_t reg, uint8_t val)
{
	int err;

	if (is_tca6416a_available) {
		HAL_NVIC_DisableIRQ(I2C3_IRQn);
		err = HAL_I2C_Mem_Write(&hi2c1, TCA6416A_ADDR, reg, 1,
					&val, 1, TCA6416A_SMBTO);
		HAL_NVIC_EnableIRQ(I2C3_IRQn);
	} else {
		dummy_tca6416a_regmap[reg & TCA6416A_REG_MASK] = val;
		err = HAL_OK;
	}

	return err;
}

static inline int tca6416a_read(uint8_t reg)
{
	uint8_t tmp;
	int err;

	if (is_tca6416a_available) {
		HAL_NVIC_DisableIRQ(I2C3_IRQn);
		err = HAL_I2C_Mem_Read(&hi2c1, TCA6416A_ADDR, reg, 1,
			       &tmp, 1, TCA6416A_SMBTO);
		HAL_NVIC_EnableIRQ(I2C3_IRQn);
	} else {
		tmp = dummy_tca6416a_regmap[reg & TCA6416A_REG_MASK];
		err = HAL_OK;
	}

	if (err == HAL_OK)
		return tmp;
	else
		return -1;
}

static inline void tca6416a_set(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val |= (1 << pin);
	tca6416a_write(reg, val);
}

static inline void tca6416a_clr(uint8_t port, uint8_t pin)
{
	uint8_t reg;
	uint8_t val;

	reg = TCA6416A_P0_OUT + (port ? 1 : 0);
	val = tca6416a_read(reg);
	val &= ~(1 << pin);
	tca6416a_write(reg, val);
}

#endif
