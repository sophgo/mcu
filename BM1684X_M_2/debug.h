#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG

#ifdef DEBUG

#include <stdio.h>

#define debug(...)	printf(__VA_ARGS__)

#else

#define debug(...)	do {} while (0)

#endif

#endif
