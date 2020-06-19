#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <libopencm3/stm32/usart.h>

#define USART1_BAUDRATE	115200
#define USART2_BAUDRATE	115200
#define USART4_BAUDRATE	115200

#define STD_UART	USART1
#define MON_UART	USART2
#define UPG_UART	USART4

void system_init(void);
void clock_init(void);
int std_read(void);

#endif
