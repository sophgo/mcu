#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG

#include <stdio.h>

#define debug(fmt, ...)	printf(fmt, __VA_ARGS__)

#else

#define debug(fmt, ...)	do {} while (0)

#endif

#endif
