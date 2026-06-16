/*
 * project.h:
 *
 * (C) Copyright 2024 Sophgo Technology
 */
#ifndef __PROJECT_H__
#define __PROJECT_H__

/* 与 mcu-util / gd32e50_util 族编号一致 (0=STM32L0, 1=GD32E50) */
enum {
	MCU_FAMILY_STM32L0,
	MCU_FAMILY_GD32E50,
};

#define BOARD_TYPE_UNKNOWN		0
#define BOARD_TYPE_CV84X6EVB		0x51

#endif /* __PROJECT_H__ */
