#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG

#include <stdio.h>
#include <stdint.h>

#define debug(...)	printf(__VA_ARGS__)
#define debug_reg(addr)	printf("[Register Memory] 0x%08lx:0x%08lx\n",\
						 (uint32_t)addr, (*(volatile uint32_t *)(uint32_t)(addr)))

#else

#define debug(...)	do {} while (0)
#define debug_reg(addr) do{} while (0)
#endif

#endif
