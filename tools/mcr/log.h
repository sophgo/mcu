#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define debug(fmt, ...)	fprintf(stdout, fmt, ## __VA_ARGS__)
#define info(fmt, ...)	fprintf(stdout, fmt, ## __VA_ARGS__)
#define error(fmt, ...)	fprintf(stderr, fmt, ## __VA_ARGS__)
#define log_init()	do { setvbuf(stdout, NULL, _IONBF, 0); } while (0)

#endif
