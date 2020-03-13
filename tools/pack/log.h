#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define debug(...)	printf("[DEBUG]: " __VA_ARGS__)
#define error(...)	fprintf(stderr, "[ERROR]: " __VA_ARGS__)
#define warn(...)	fprintf(stderr, "[WARN]: " __VA_ARGS__)



#endif
