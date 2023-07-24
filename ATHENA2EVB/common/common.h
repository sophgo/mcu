/*
 * common.h :
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <pin.h>
#include <gd32e50x.h>
#include <stdint.h>

#define MCU_SW_VER		0
#define false			0
#define true			1
#define OFF			0
#define ON			1
#define FLASH_SIZE		(256 * 1024)
#define FLASH_PAGE_SIZE		(8 * 1024)
#define FLASH_PAGE_MASK		(FLASH_PAGE_SIZE - 1)
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define ROUND_UP(x, n)		(((x) + ((n) - 1)) / n)
#define MCU_SLAVE_ADDR		0x17

enum {
    DDR_TYPE_LPDDR4X,
    DDR_TYPE_LPDDR4,
    UNDEFINED_DDR_TYPE,
};

struct gpio_nodes {
	int port, pin;
};

uint8_t get_pcb_version(void);
void set_pcb_version(uint8_t version);
uint8_t get_bom_version(void);
uint8_t get_hardware_version(void);
uint8_t get_ddr_type(void);
uint8_t get_firmware_version(void);
char *get_board_type_name(void);
uint8_t get_board_type(void);
void set_board_type(uint8_t type);

/* remap libopencm3 to libgd */
#define gpio_clear		gpio_bit_reset
#define gpio_set		gpio_bit_set
#define gpio_get		gpio_input_bit_get
#define nvic_disable_irq(irq)	nvic_irq_disable(irq)
#define nvic_enable_irq(irq)	nvic_irq_enable(irq, 0, 0)
#define exti_reset_request	exti_interrupt_flag_clear

#endif /* __COMMON_H__ */
