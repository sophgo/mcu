/*
 * board_power_porting.c:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

/* AUTO GENERATED CODE */

#ifndef __weak
#define __weak __attribute__((weak))
#endif

int __weak sys_rst_assert_on(void)
{
	/* Add customer code here */
	return 0;
}

void __weak sys_rst_assert_off(void)
{
	/* Add customer code here */
}

int __weak sys_rst_deassert_on(void)
{
	/* Add customer code here */
	return 0;
}

void __weak sys_rst_deassert_off(void)
{
	/* Add customer code here */
}

/* AUTO GENERATED CODE END */
