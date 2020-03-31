#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include <opt.h>

enum{
    LEVEL_ERR,
    LEVEL_INFO,
    LEVEL_DBG,
};

extern unsigned log_output_level;

#define error(fmt,...)					\
	do {						\
		fprintf(stderr, fmt, ##__VA_ARGS__);	\
	} while(0)

#define debug(fmt,...)						\
	do {							\
		if (log_output_level >= LEVEL_DBG) {		\
			fprintf(stdout, fmt, ##__VA_ARGS__);	\
			fflush(stdout);				\
		}						\
	} while(0)

#define warn(fmt,...)						\
	do {							\
		if (log_output_level >= LEVEL_DBG) {		\
			fprintf(stdout, fmt, ##__VA_ARGS__);	\
			fflush(stdout);				\
		}						\
	} while(0)

#define info(fmt,...)						\
	do {							\
		if (log_output_level >= LEVEL_INFO) {		\
			fprintf(stdout, fmt, ##__VA_ARGS__);	\
			fflush(stdout);				\
		}						\
	} while(0)

void set_log_level(int level);

#endif
