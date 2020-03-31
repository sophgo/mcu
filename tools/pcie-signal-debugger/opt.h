#ifndef __OPT_H__
#define __OPT_H__

#include <stdlib.h>

#ifdef PATH_CHAR_MAX
#error PATH_CHAR_MAX has been defined
#endif
#define PATH_CHAR_MAX    (256)

#ifdef COMM_MAX
#error COMM_MAX has been defined
#undef COMM_MAX
#endif
#define COMM_MAX    (32)

struct opt{
	int verbose;
	int version;
	int help;

	int setup; /* working at setup mode */
	unsigned int time;

	char output[PATH_CHAR_MAX];
	char device[PATH_CHAR_MAX];
};


int opt_destroy(struct opt *opt);
void opt_info(struct opt *opt);
void opt_help(void);
struct opt *opt_parse(int argc,char *argv[]);

#endif
