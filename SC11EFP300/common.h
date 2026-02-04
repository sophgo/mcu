#ifndef __COMMON_H__
#define __COMMON_H__

#include <gd32f4xx.h>
#include <pin.h>

#define MCU_SW_VER      0
#define SOC_NUM         4
#define RAA228234_CHANNEL	2

#define false	0
#define true	1

#define PCIE_RESET_PORT PCIE_RST_X_PORT
#define PCIE_RESET_PIN  PCIE_RST_X_PIN
#define PCIE_RESET_EXTI PCIE_RST_X_EXTI
#define PCIE_RESET_NVIC EXTI2_IRQn

#define ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))
#define ROUND_UP(x, n)          (((x) + ((n) - 1)) / n)

#define MCU_SLAVE_ADDR          0x17

#define I2C1_OA1                0x17
#define I2C1_OA2                0x68
#define I2C1_OA2_MASK           0x03

/*
+-----------------------+ 0x08000000
|                       |
|  Flash Memory (768K)  |
|                       |
+-----------------------+ 0x080C0000 (FLASH_BASE + EEPROM_BASE_OFFSET)
| EEPROM Page (128K)      |
| 0: DVFS Mode          | 0x080C0000 (EEPROM_BASE + EEPROM_DVFS_MODE_OFFSET)
| 1: Power Off Reason   | 0x080C0020 (EEPROM_BASE + EEPROM_POWER_OFF_REASON_OFFSET)
| 2: MAC0               | 0x080C0040 (EEPROM_BASE + EEPROM_MAC0_OFFSET)
| 3: MAC1               | 0x080C0060 (EEPROM_BASE + EEPROM_MAC1_OFFSET)
+-----------------------+ 0x080E0000 (FLASH_BASE + SN_BASE)
| SN Page (4K)          |
| SN (32 bytes)  	| 0x080E0020 (FLASH_BASE + SN_BASE)
+-----------------------+ 0x080E1000
| Unused                |
+-----------------------+ 0x08100000 (FLASH_BASE + FLASH_SIZE)

*/

/*
 * Flash block layout
+-----------------------+-----------------------+ 0x08000000
|	BLOCK 0		|         16KB		|
|	BLOCK 1		|         16KB          |
|	BLOCK 2		|         16KB          |
|	BLOCK 3		|         16KB          |
|	BLOCK 4		|         64KB          |
|	BLOCK 5		|         128KB         |
|	BLOCK 6		|         128KB         |
|	BLOCK 7		|         128KB         |
|	BLOCK 8		|         128KB         |
|	BLOCK 9		|         128KB         |
|	BLOCK 10	|         128KB         |
|	BLOCK 11	|         128KB         |
+-----------------------+-----------------------+ 0x08100000

 */

#define FLASH_SIZE	(1024 * 1024)
#define FLASH_PAGE_SIZE (4 * 1024)
#define FLASH_BLOCK_MIN_SIZE	(16 * 1024)
#define FLASH_BLOCK_MID_SIZE	(64 * 1024)
#define FLASH_BLOCK_MAX_SIZE	(128 * 1024)
#define FLASH_PAGE_MASK	(FLASH_PAGE_SIZE - 1)

#define EEPROM_BASE	(FLASH_BASE + (FLASH_SIZE - FLASH_PAGE_SIZE * 2) )
#define EEPROM_SIZE	FLASH_BLOCK_MAX_SIZE

#define EEPROM_CELL_SIZE	32
#define EEPROM_CELL_OFFSET(n)	(EEPROM_CELL_SIZE * n)
#define EEPROM_CELL_ADDR(n)	(EEPROM_BASE + EEPROM_CELL_OFFSET(n))

#define EEPROM_DVFS_MODE_OFFSET		EEPROM_CELL_OFFSET(0)
#define EEPROM_POWER_OFF_REASON_OFFSET	EEPROM_CELL_OFFSET(1)
#define EEPROM_MAC0_OFFSET		EEPROM_CELL_OFFSET(2)
#define EEPROM_MAC1_OFFSET		EEPROM_CELL_OFFSET(3)

#define SN_BASE			(FLASH_BASE + FLASH_SIZE - FLASH_BLOCK_MAX_SIZE)
#define SN_BASE_OFFSET		(FLASH_SIZE - FLASH_BLOCK_MAX_SIZE)
#define SN_CELL_SIZE		32
#define SN_CELL_OFFSET(n)	(SN_CELL_SIZE * n)

#define POWER_OFF_REASON_POWER_OFF	0x80
#define POWER_OFF_REASON_RESET		0x81
#define POWER_OFF_REASON_REBOOT		0x82
#define POWER_OFF_REASON_OVER_HEAT	0x83

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

void led_init(void);
void led_on(void);
void led_off(void);
void led_set_frequency(unsigned long freq);

/* remap libopencm3 to libgd */
#define gpio_clear	gpio_bit_reset
#define gpio_set	gpio_bit_set
#define gpio_get	gpio_input_bit_get

#define BM1690_I2C0_IRQ		I2C0_EV_IRQn
#define BM1690_I2C_IRQ		I2C1_EV_IRQn
#define DEBUG_I2C_IRQ		I2C2_EV_IRQn
#define nvic_disable_irq(irq)	nvic_irq_disable(irq)
#define nvic_enable_irq(irq)	nvic_irq_enable(irq, 0, 0)

#define exti_reset_request	exti_interrupt_flag_clear

#define INPUT_MODE	0
#define OUTPUT_MODE	1

uint8_t get_firmware_version(void);
void check_gpio_pin(char* name, uint32_t port, uint16_t pin, int mode);

#endif
