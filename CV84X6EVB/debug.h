/*
 * debug.h:
 *
 * (C) Copyright 2023 Sophgo Technology
 * Author: Donghao Zhao <donghao.zhao@sophgo.com>
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG

#include <stdio.h>

#define debug(...)	printf(__VA_ARGS__)

#else

#include <stdio.h>

#define debug(...)	printf(__VA_ARGS__)

#endif

#endif
