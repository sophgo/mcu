#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG

#define debug(...)	printf(__VA_ARGS__)

#else

#define debug(...)	do {} while (0)

#endif

#endif
