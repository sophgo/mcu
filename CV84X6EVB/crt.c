/*
 * crt.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

/* Setup C runtime */

#include <stdint.h>
#include <upgrade/upgrade.h>
#include <common/common.h>
#include <gd32e50x_misc.h>

extern uint32_t __ld_bss_start[0], __ld_bss_end[0];
extern uint32_t __ld_program_start[0], __ld_program_end[0];
extern void SystemInit(void);
extern void main(void);
extern int stage;

void __attribute__((noinline)) crt_init(void)
{
	/* Stack is ready after chip init */

	uint32_t *src;
	uint32_t *dst = __ld_program_start;
	register uint32_t pc;
	register uint32_t st;

	asm volatile ("mov %0, pc" : "=r" (pc));
	if (pc < MEMMAP_LOADER_END)
		st = RUN_STAGE_LOADER;
	else
		st = RUN_STAGE_APP;

	/* Copy program */
	src = (void *)(pc & ~FLASH_PAGE_MASK);

	while (dst != __ld_program_end) {
		*dst = *src;
		++dst;
		++src;
	}
	/* Sync cache */
	asm volatile ("isb");
	asm volatile ("dsb");

	/* Zero bss */
	for (dst = __ld_bss_start; dst != __ld_bss_end; ++dst)
		*dst = 0;

	SCB->VTOR = (unsigned long)__ld_program_start;

	stage = st;
}

void Reset_Handler(void)
{
	/* Main function should not return */
	/* Without volatile, gcc main optimize long call to a simple pc relative
	 * branch
	 */
	void (*volatile ram_entry)(void) = main;

	SystemInit();
	crt_init();

	ram_entry();

	/* Should not reach here */
	while (1)
		;
}
