#ifndef __PIC_H__
#define __PIC_H__

#include <stm32l0xx_hal.h>

/* smbus timeout, count in ms */
#define PIC_SMBTO		2

/* PIC function definition */
#define PIC_ADDR		(0x24 << 1)
#define PIC_REG_BOARD_TYPE	0
#define PIC_REG_SW_VERSION	1
#define PIC_REG_HW_VERSION	2
#define PIC_REG_CTRL		3
#define PIC_REG_SOC_TEMP	4
#define PIC_REG_HEATER_CTRL	5
#define PIC_REG_REQUEST		6

#define PIC_BOARD_TYPE_SE5	3
#define PIC_CMD_REBOOT		7
#define PIC_CMD_POWER_OFF	2

#define PIC_REQ_NONE		0
#define PIC_REQ_POWER_OFF	1
#define PIC_REQ_REBOOT		2
#define PIC_REQ_FACTORY_RESET	3

#define ERROR_RETRY		10

extern volatile int is_pic_available;

static inline int pic_probe(void)
{
	uint8_t tmp;
	int err;

	err = HAL_I2C_Mem_Read(&hi2c1, PIC_ADDR, PIC_REG_BOARD_TYPE, 1,
			       &tmp, 1, PIC_SMBTO);

	return is_pic_available = (err == HAL_OK);
}

static inline int pic_write(uint8_t reg, uint8_t val)
{
	int err;
	int i = 0;

	HAL_NVIC_DisableIRQ(I2C3_IRQn);

	do {
		err = HAL_I2C_Mem_Write(&hi2c1, PIC_ADDR,
					reg, 1, &val, 1, PIC_SMBTO);
		++i;
	} while (err < 0 && i <= ERROR_RETRY);


	HAL_NVIC_EnableIRQ(I2C3_IRQn);

	return err == HAL_OK ? 0 : -1;
}


static inline int pic_read(uint8_t reg)
{
	uint8_t tmp;
	int err;
	int i = 0;

	HAL_NVIC_DisableIRQ(I2C3_IRQn);

	do {
		err = HAL_I2C_Mem_Read(&hi2c1, PIC_ADDR,
				       reg, 1, &tmp, 1, PIC_SMBTO);
		++i;
	} while (err < 0 && i <= ERROR_RETRY);

	HAL_NVIC_EnableIRQ(I2C3_IRQn);

	if (err == HAL_OK)
		return tmp;
	else
		return -1;
}

void pic_i2c_slave_init(void);

#endif
