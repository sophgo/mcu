/**
 * @file main.c
 * @author chao.wei
 * @brief application logical start
 */
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>

void system_init(void)
{
	/* we may load this to a place other than default reset address */
	/* so relocated vtor -- vector table offset register */
	SCB_VTOR = (uint32_t)&vector_table;
}
