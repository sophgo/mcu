#ifndef __COMMON_H__

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define MCU_SLAVE_ADDR		0x17

#define I2C1_OA1		0x17
#define I2C1_OA2		0x68
#define I2C1_OA2_MASK		0x07

#endif
