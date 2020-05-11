#include <assert.h>
#include "stm32l0xx_hal.h"
#include "i2c.h"
#include "i2c_slave.h"
static struct gpioex_ctx {
	int set_idx;
	int idx;
} gpioex_ctx;
#define GPIOEX_REG_INPUT0	0X00
#define GPIOEX_REG_INPUT1	0X01
#define GPIOEX_REG_OUTPUT0	0X02
#define GPIOEX_REG_OUTPUT1	0X03
#define GPIOEX_REG_POLARITY0 0X04
#define GPIOEX_REG_POLARITY1 0X05
#define GPIOEX_REG_CONFIG0	0x06
#define GPIOEX_REG_CONFIG1 0x07


#define TCA6416A_ADDR	0x40
#define TCA_REG_INPUT0	0X00
#define TCA_REG_INPUT1	0X01
#define TCA_REG_OUTPUT0 0X02
#define TCA_REG_OUTPUT1 0X03
#define TCA_REG_POLARITY0 0X04
#define TCA_REG_POLARITY1 0X05
#define TCA_REG_CONFIG0	0x06
#define TCA_REG_CONFIG1 0x07

#define GPIOEX0_12V_ON		0x01
#define GPIOEX0_RELAY1		0x02
#define GPIOEX0_RELAY2		0x04
#define GPIOEX0_LED1		0x08
#define GPIOEX0_LED2		0x10
#define GPIOEX0_PWR_OFF		0x20	//INPUT
#define GPIOEX0_PCIESEL		0x40
#define GPIOEX0_LTE_RST		0x80

#define GPIOEX1_LTE_PWR_EN	0x01
#define GPIOEX1_PCIE_INT	0x02	//RESERVE	//INPUT
#define GPIOEX1_SATA_EN		0x04
#define GPIOEX1_CLK_EN		0x08	//RESERVE
#define GPIOEX1_5G_PD		0x10	//RESERVE
#define GPIOEX1_WWAN_OFF	0x20	//RESERVE
#define GPIOEX1_WWAN_RST	0x40	//RESERVE
#define GPIOEX1_ASM3142_POR	0x80	//RESERVE

#define GPIOEX0_DEF_INPUT	GPIOEX0_PWR_OFF
#define GPIOEX1_DEF_INPUT	GPIOEX1_PCIE_INT

#define GPIOEX0_DEF_HIGH	0
#define GPIOEX1_DEF_HIGH	(GPIOEX1_LTE_PWR_EN | GPIOEX1_SATA_EN)

#define GPIOEX0_RESERVE	(GPIOEX0_12V_ON|GPIOEX0_LED1|GPIOEX0_PWR_OFF)
#define GPIOEX1_RESERVE (GPIOEX1_PCIE_INT)

#define GPIOEX0_MASK		(~GPIOEX0_RESERVE)
#define GPIOEX1_MASK		(~GPIOEX1_RESERVE)

int sm5_gpioex_led1_on()
{
	int ret;
	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val |= GPIOEX0_LED1;
	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}
int sm5_gpioex_led1_off()
{
	int ret;
	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val &= ~GPIOEX0_LED1;
	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}
int sm5_gpioex_led2_on()
{
	int ret;

	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val |= GPIOEX0_LED2;
	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}
int sm5_gpioex_led2_off()
{
	int ret;

	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val &= ~GPIOEX0_LED2;
	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}

int sm5_gpioex_12v_on()
{
	int ret;

	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val |= GPIOEX0_12V_ON;

	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}
int sm5_gpioex_12v_off()
{
	int ret;

	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	val &= ~GPIOEX0_12V_ON;
	ret = HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -2;
	}
	return 0;
}

int sm5_gpioex_getpoweroff()
{
	int ret;
	uint8_t val;
	ret = HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, TCA_REG_INPUT0,1, &val, 1, 1000);
	if (ret != 0)
	{
		return -1;
	}
	if (val & GPIOEX0_PWR_OFF)
	{
		// HIGH
		return 1;
	}
	else
	{
		// LOW
		return 0;
	}
}

static void sm5_gpioex_match(void *priv, int dir)
{
	if (dir == I2C_SLAVE_WRITE) {
		gpioex_ctx.set_idx = 1;
	} else {

	}
}

static void sm5_gpioex_write(void *priv, uint8_t data)
{
	uint8_t tmp;
	if (gpioex_ctx.set_idx) {
		gpioex_ctx.idx = data % 8;
		gpioex_ctx.set_idx = 0;
		return;
	}

	HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, gpioex_ctx.idx,1, &tmp, 1, 1000);
	switch (gpioex_ctx.idx)
	{
		case GPIOEX_REG_INPUT0:
		case GPIOEX_REG_OUTPUT0:
		case GPIOEX_REG_POLARITY0:
		case GPIOEX_REG_CONFIG0:
			tmp &= GPIOEX0_RESERVE;
			data &= GPIOEX0_MASK;
			break;
		case GPIOEX_REG_INPUT1:
		case GPIOEX_REG_OUTPUT1:
		case GPIOEX_REG_POLARITY1:
		case GPIOEX_REG_CONFIG1:
			tmp &= GPIOEX1_RESERVE;
			data &= GPIOEX1_MASK;
			break;
	}
	data |= tmp;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, gpioex_ctx.idx,1, &data, 1, 1000);
	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
}

static uint8_t sm5_gpioex_read(void *priv)
{
	uint8_t tmp;

	HAL_I2C_Mem_Read(&hi2c1,TCA6416A_ADDR, gpioex_ctx.idx,1, &tmp, 1, 1000);
	gpioex_ctx.idx = (gpioex_ctx.idx + 1) % 8;
	return tmp;
}

static void sm5_gpioex_stop(void *priv)
{

}

static struct i2c_slave_op slave = {
	.addr = 0x6C,
	.match	= sm5_gpioex_match,
	.write	= sm5_gpioex_write,
	.read	= sm5_gpioex_read,
	.stop	= sm5_gpioex_stop,
};
int sm5_gpioex_init()
{
	uint8_t val;

	//set output high
	val = GPIOEX0_DEF_HIGH;
	if (HAL_OK != HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT0,1, &val, 1, 1))
        return -1;

	HAL_Delay(1);
	val = GPIOEX1_DEF_HIGH;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_OUTPUT1,1, &val, 1, 1000);
	HAL_Delay(1);

	// set input pin
	val = GPIOEX0_DEF_INPUT;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_CONFIG0,1, &val, 1, 1000);
	HAL_Delay(1);

	val = GPIOEX1_DEF_INPUT;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_CONFIG1,1, &val, 1, 1000);
	HAL_Delay(1);

	// set polarity
	val = 0;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_POLARITY0,1, &val, 1, 1000);
	HAL_Delay(1);
	val = 0;
	HAL_I2C_Mem_Write(&hi2c1,TCA6416A_ADDR, TCA_REG_POLARITY1,1, &val, 1, 1000);
	HAL_Delay(1);

	i2c_slave_register(&i2c_ctx3,&slave);
    return 0;
}
