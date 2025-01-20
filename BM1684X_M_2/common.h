#ifndef __COMMON_H__
#define __COMMON_H__

#include <pin.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define MCU_SLAVE_ADDR		0x17

#define I2C1_OA1		0x17
#define I2C1_OA2		0x68
#define I2C1_OA2_MASK		0x03

#define I2C2_OA1		0x38
#define I2C2_OA2		0x00
#define I2C2_OA2_MASK		0x00

#define PCIE_RESET_PORT	PCIEE_RST_X_MCU_PORT
#define PCIE_RESET_PIN	PCIEE_RST_X_MCU_PIN
#define PCIE_RESET_EXTI	PCIEE_RST_X_MCU_EXTI
#define PCIE_RESET_NVIC	NVIC_EXTI4_15_IRQ

#define SYS_RST_PORT	SYS_RST_X_PORT
#define SYS_RST_PIN	SYS_RST_X_PIN

#define EEPROM_BASE	0x08080000
#define EEPROM_SIZE	0x00000800

#define EEPROM_CELL_SIZE	32
#define EEPROM_CELL_OFFSET(n)	(EEPROM_CELL_SIZE * n)
#define EEPROM_CELL_ADDR(n)	(EEPROM_BASE + EEPROM_CELL_OFFSET(n))

#define EEPROM_MAC0_OFFSET		EEPROM_CELL_OFFSET(0)
#define EEPROM_MAC1_OFFSET		EEPROM_CELL_OFFSET(1)
#define EEPROM_SN_OFFSET		EEPROM_CELL_OFFSET(2)
#define EEPROM_BOARD_TYPE_OFFSET	EEPROM_CELL_OFFSET(3)
#define EEPROM_POWER_OFF_REASON_OFFSET	EEPROM_CELL_OFFSET(5)
#define EEPROM_MIXED_MODE_OFFSET	(EEPROM_CELL_OFFSET(5) + 1)
#define EEPROM_TEST_MODE_OFFSET		(EEPROM_CELL_OFFSET(5) + 2)

#define EEPROM_MAC0_ADDR		EEPROM_CELL_ADDR(0)
#define EEPROM_MAC1_ADDR		EEPROM_CELL_ADDR(1)
#define EEPROM_SN_ADDR			EEPROM_CELL_ADDR(2)
#define EEPROM_BOARD_TYPE_ADDR		EEPROM_CELL_ADDR(3)
#define EEPROM_POWER_OFF_REASON_ADDR	EEPROM_CELL_ADDR(5)
#define EEPROM_MIXED_MODE_ADDR	(EEPROM_BASE + EEPROM_MIXED_MODE_OFFSET)

#define BIT(n)		(1 << n)

#define LED_FREQ_ALWAYS_ON	1000
#define LED_FREQ_ALWAYS_OFF	0

enum {
	WORK_MODE_SOC = 1,
	WORK_MODE_PCIE = 2,
	WORK_MODE_MIXED = 3,
};

enum {
	DDR_TYPE_LPDDR4X = 0,
	DDR_TYPE_LPDDR4 = 1,
};

enum {
	TEST_MODE_HALT = 1,
	TEST_MODE_RUN = 2,
};

void set_board_type(uint8_t type);
uint8_t get_board_type(void);
uint8_t get_firmware_version(void);
int get_work_mode(void);
void board_init(void);
uint8_t get_declared_board_type(void);
uint8_t get_ddr_type(void);
char *get_board_type_name();
void led_init(void);
void led_set_frequency(unsigned long freq);
int detect_test_mode(void);
void set_soc_temp(int temp);
void set_board_temp(int temp);
int get_board_temp(void);
int get_soc_temp(void);
void set_soc_forever(void);
#endif
