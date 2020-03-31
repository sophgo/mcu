#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG

#include <stdio.h>

#define debug(...)	printf(__VA_ARGS__)

#else

#define debug(fmt, ...)	do {} while (0)

#endif

#define error(...)	printf(__VA_ARGS__)
#define warn(...)	printf(__VA_ARGS__)
#define info(...)	printf(__VA_ARGS__)

void dump(void *data, unsigned long len);

#endif
