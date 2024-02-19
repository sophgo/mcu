#ifndef __COMMON_H__
#define __COMMON_H__

#include <pin.h>
#include <gd32e50x.h>

#define MCU_SW_VER	0
#define SOC_NUM 6

#define false 0
#define true 1

#define PCIE_RESET_PORT	PCIE_RST_1V8_PORT
#define PCIE_RESET_PIN	PCIE_RST_1V8_PIN
#define PCIE_RESET_EXTI	PCIE_RST_1V8_EXTI
#define PCIE_RESET_NVIC	EXTI2_IRQn

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define ROUND_UP(x, n)		(((x) + ((n) - 1)) / n)

#define MCU_SLAVE_ADDR		0x17

#define I2C1_OA1		0x17
#define I2C1_OA2		0x68
#define I2C1_OA2_MASK		0x03

#define FLASH_SIZE	(256 * 1024)
#define FLASH_PAGE_SIZE	(8 * 1024)
#define FLASH_PAGE_MASK	(FLASH_PAGE_SIZE - 1)
#define EEPROM_BASE	(FLASH_BASE + (FLASH_SIZE - FLASH_PAGE_SIZE))
#define EEPROM_SIZE	FLASH_PAGE_SIZE

#define EEPROM_CELL_SIZE	32
#define EEPROM_CELL_OFFSET(n)	(EEPROM_CELL_SIZE * n)
#define EEPROM_CELL_ADDR(n)	(EEPROM_BASE + EEPROM_CELL_OFFSET(n))

#define EEPROM_MAC0_OFFSET		EEPROM_CELL_OFFSET(0)
#define EEPROM_MAC1_OFFSET		EEPROM_CELL_OFFSET(1)
#define EEPROM_SN_OFFSET		EEPROM_CELL_OFFSET(2)
#define EEPROM_BOARD_TYPE_OFFSET	EEPROM_CELL_OFFSET(3)
#define EEPROM_POWER_OFF_REASON_OFFSET	EEPROM_CELL_OFFSET(5)

#define EEPROM_MAC0_ADDR		EEPROM_CELL_ADDR(0)
#define EEPROM_MAC1_ADDR		EEPROM_CELL_ADDR(1)
#define EEPROM_SN_ADDR			EEPROM_CELL_ADDR(2)
#define EEPROM_BOARD_TYPE_ADDR		EEPROM_CELL_ADDR(3)
#define EEPROM_POWER_OFF_REASON_ADDR	EEPROM_CELL_ADDR(5)

#define LED_FREQ_ALWAYS_ON	1000
#define LED_FREQ_ALWAYS_OFF	0

enum {
	WORK_MODE_SOC = 1,
	WORK_MODE_PCIE = 2,
};

enum {
	DDR_TYPE_LPDDR4X = 0,
	DDR_TYPE_LPDDR4 = 1,
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
void led_on(void);
void led_off(void);
void led_set_frequency(unsigned long freq);

int get_board_temp(void);
int get_soc_temp(void);
void set_board_temp(int temp);
void set_soc_temp(int temp);

/* remap libopencm3 to libgd */
#define gpio_clear	gpio_bit_reset
#define gpio_set	gpio_bit_set
#define gpio_get	gpio_input_bit_get

#define MCU_CPLD_I2C_IRQ	I2C0_EV_IRQn
#define DEBUG_I2C_IRQ		I2C2_EV_IRQn
#define nvic_disable_irq(irq)	nvic_irq_disable(irq)
#define nvic_enable_irq(irq)	nvic_irq_enable(irq, 0, 0)

#define exti_reset_request	exti_interrupt_flag_clear

#endif
