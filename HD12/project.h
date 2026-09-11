#ifndef __PROJECT_H__
#define __PROJECT_H__

#define HD12		0xb6
#define RHS12		0xb7
// #define BOARD_TYPE	HD12

/* Board hardware version = get_hardware_version() = (pcb_ver<<4)|bom_ver.
 * Notation V<pcb>.<bom>: pcb is major, bom is minor. */
#define HW_VER(pcb, bom)	(((pcb) << 4) | (bom))
#define HW_VER_V1_0		HW_VER(1, 0)	/* 0x10 */
#define HW_VER_V1_1		HW_VER(1, 1)	/* 0x11 */
#define HW_VER_V2_0		HW_VER(2, 0)	/* 0x20 */

/* MCU Family */
enum {
	MCU_FAMILY_STM32L0	= 0,
	MCU_FAMILY_GD32E50,
	MCU_FAMILY_GD32F470,
};

#endif
