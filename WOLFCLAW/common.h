#ifndef __COMMON_H__
#define __COMMON_H__

#include <pin.h>
#include <gd32e50x.h>

#define MCU_SW_VER	1

#define SLAVE_I2C	I2C1
#define SLAVE_DEFAULT_ADDR	0x50

#define false 0
#define true 1

#define FLASH_SIZE	(256 * 1024)
#define FLASH_PAGE_SIZE	(8 * 1024)
#define FLASH_PAGE_MASK	(FLASH_PAGE_SIZE - 1)

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define ROUND_UP(x, n)		(((x) + ((n) - 1)) / n)

#define LED_FREQ_ALWAYS_ON	1000
#define LED_FREQ_ALWAYS_OFF	0

enum {
	ISL68224 = 0,
	ISL68127,
};

void set_default_device(uint8_t type);
uint8_t get_default_device(void);
void set_default_addr(uint8_t addr);
uint8_t get_default_addr(void);
void set_default_file_name(char* name);
char *get_default_filename();
FlagStatus is_key_pressed();
FlagStatus is_sdcard_inserted();

void led_init(void);
void led_1_0_on(void);
void led_1_1_on(void);
void led_2_0_on(void);
void led_2_1_on(void);
void led_3_0_on(void);
void led_3_1_on(void);
void led_4_0_on(void);
void led_4_1_on(void);
void led_flash(int times);
void led_set_frequency(unsigned long freq);

/* remap libopencm3 to libgd */
#define gpio_clear	gpio_bit_reset
#define gpio_set	gpio_bit_set
#define gpio_get	gpio_input_bit_get

#define nvic_disable_irq(irq)	nvic_irq_disable(irq)
#define nvic_enable_irq(irq)	nvic_irq_enable(irq, 0, 0)

#define exti_reset_request	exti_interrupt_flag_clear

#endif
