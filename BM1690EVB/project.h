#ifndef __PROJECT_H__
#define __PROJECT_H__

#define EVB	0
#define SA5	1
#define SC5	2
#define SE5	3
#define SM5P	4
#define SM5S	5
#define SA6	6

#define SC5PLUS	7
#define SC5H	8
#define SC5PRO	9
#define SM5ME	10
#define SM5MP	11
#define SM5MS	12
#define SM5MA	13

#define SG2042EVB	0x80
#define SG2042REVB	0x81
/* 0x82 use for mango Pisces */
#define SG2042X4	0x83
#define MILKV_PIONEER	0x90

/* MCU Family */
enum {
	MCU_FAMILY_STM32L0	= 0,
	MCU_FAMILY_GD32E50,
};

#endif
