/* setup c runtime */

#include <stdint.h>

extern uint32_t __ld_data_load_start[0], __ld_data_load_end[0];
extern uint32_t __ld_data_start[0], __ld_data_end[0];
extern uint32_t __ld_bss_start[0], __ld_bss_end[0];
extern void SystemInit(void);
extern void main(void);

void __attribute__((weak)) crt_init(void)
{
	/* stack is ready after chip init */
	/* copy data */
	uint32_t *src, *dst;

	src = __ld_data_load_start;
	dst = __ld_data_start;

	while (dst != __ld_data_end) {
		*dst = *src;
		++dst;
		++src;
	}

	/* zero bss */
	for (dst = __ld_bss_start; dst != __ld_bss_end; ++dst)
		*dst = 0;
}

void __attribute__((weak)) Reset_Handler(void)
{
	SystemInit();
	crt_init();
	main();
	while (1)
		;
}
