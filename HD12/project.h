#ifndef __PROJECT_H__
#define __PROJECT_H__

#define HD12		0xb6
#define RHS12		0xb7
// #define BOARD_TYPE	HD12

/* V2.0 threshold: PCB version >= 2 means V2.0 hardware */
#define PCB_VER_V2_0	2

/* MCU Family */
enum {
	MCU_FAMILY_STM32L0	= 0,
	MCU_FAMILY_GD32E50,
	MCU_FAMILY_GD32F470,
};

#endif
