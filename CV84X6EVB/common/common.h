/*
 * common.h:
 *
 * (C) Copyright 2024 Sophgo Technology
 * CV84X6EVB 板级公共定义
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <gd32e50x.h>
#include <pin.h>
#include <project.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define FLASH_PAGE_SIZE		(8 * 1024)
#define FLASH_PAGE_MASK		(FLASH_PAGE_SIZE - 1)
#define MCU_SLAVE_ADDR		0x17
#define MCU_SW_VER		0x01
#define PCB_VERSION		0x01
#define BOARD_TYPE		BOARD_TYPE_CV84X6EVB
#define BOM_VERSION		0x01

static inline uint8_t get_board_type(void)
{
	return BOARD_TYPE;
}

static inline uint8_t get_firmware_version(void)
{
	return MCU_SW_VER;
}

static inline uint8_t get_pcb_version(void)
{
	return PCB_VERSION;
}

static inline uint8_t get_bom_version(void)
{
	return BOM_VERSION;
}

static inline uint8_t get_ddr_type(void)
{
	return 0xff;
}

#endif /* __COMMON_H__ */
