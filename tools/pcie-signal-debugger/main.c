#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <opt.h>
#include <log.h>
#include <common.h>
#include <unistd.h>
#include <setup.h>
#include <output.h>

#define ROUND_UP(x, align) (((x) + ((align) - 1)) / (align) * (align))
#define ROUND_DOWN(x, align) ((x) / (align) * (align))

int main(int argc, char *argv[])
{
	int err;

	assert(sizeof(struct ctrl_block) == 512);
	assert(sizeof(struct info_block) == 512);

	struct opt *opt;
	opt = opt_parse(argc, argv);
	if (opt == NULL) {
		error("invalid argument\n");
		return 1;
	}

	if (opt->verbose) {
		opt_info(opt);
		set_log_level(LEVEL_DBG);
	}

	if (opt->help || argc == 1) {
		opt_help();
		return 0;
	}

	if (opt->version) {
		info("\npcie-signal-debugger " VERSION "\n\n");
		return 0;
	}

	if (opt->device[0] == 0) {
		error("no sd card device specified\n");
		opt_help();
		return 1;
	}

	if (opt->setup)
		err = setup(opt);
	else
		err = output(opt);

	return err;
}

