#include <opt.h>
#include <string.h>
#include <stdlib.h>
#include <log.h>
#include <getopt.h>

#define mfree(ptr)  do{if(ptr) {free(ptr);ptr = NULL;}}while(0)

struct optdes{
	char *s;    //short option
	char *l;    //long option
	char *d;    //description
};

const struct optdes help[] = {
	{"V", "verbose", "verbose information"},
	{"v", "version", "version information"},
	{"h", "help", "display this message"},

	{"s", "setup", "setup sd card control block, donnot export signal data"},
	{"t", "time",
		"on setup mode, set acquire time, count in seconds, set to 0 uses default time\n"
		"        on output mode, how long to be saved, count in seconds"
	},
	{"o", "output", "output file"},
};

void opt_help(void)
{
	info("\npcie-signal-debugger [OPTIONS] SD-CARD-DEVICE\n");

	int i;

	for(i = 0;i<sizeof(help)/sizeof(help[0]);++i) {
		info("-%s, --%s\n", help[i].s, help[i].l);
		info("        %s\n", help[i].d);
	}
	info("\n");
	return;
}

int opt_destroy(struct opt *opt)
{
	if(opt)
		free(opt);
	return 0;
}

void opt_info(struct opt *opt)
{
	if(NULL == opt) {
		info("NULL option\n");
		return;
	}
	/* info("[%p]\n", opt); */

	info("%-10s %s\n", "verbose", opt->verbose ? "TRUE" : "FALSE");
	info("%-10s %s\n", "help", opt->help ? "TRUE" : "FALSE");
	info("%-10s %s\n", "version", opt->version ? "TRUE" : "FALSE");

	info("%-10s %s\n", "setup", opt->setup ? "TRUE" : "FALSE");
	info("%-10s %u\n", "time", opt->time);
	info("%-10s %s\n", "output", opt->output);
	info("%-10s %s\n", "sd card device", opt->device);
}

struct opt *opt_parse(int argc, char *argv[])
{
	struct opt *opt = malloc(sizeof(struct opt));
	if(NULL == opt) {
		error("malloc failed\n");
		return NULL;
	}
	memset(opt, 0x00, sizeof(*opt));
	while (1) {
		static struct option long_options[] = {
			/* These options set a flag. */
			{"verbose", no_argument,       0, 'V'},
			{"version", no_argument,       0, 'v'},
			{"help",    no_argument,       0, 'h'},

			{"setup",  no_argument, 0, 's'},
			{"time",  required_argument, 0, 't'},
			{"output",  required_argument, 0, 'o'},

			{0, 0, 0, 0},
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		int c = getopt_long (argc, argv, "t:o:sVvh",
				     long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 'v':
			opt->version = 1;
			break;
		case 'V':
			opt->verbose = 1;
			break;
		case 'h':
			opt->help = 1;
			break;

		case 's':
			opt->setup = 1;
			break;
		case 'o':
			strcpy(opt->output, optarg);
			break;
		case 't':
			opt->time = strtol(optarg, NULL, 0);
			break;

		case '?':
			/* getopt_long already printed an error message. */
			goto err0;
			break;
		default:
			abort ();
		}
	}
	if(argc > optind)
		strcpy(opt->device, argv[optind]);

	return opt;
err0:
	opt_destroy(opt);
	return NULL;
}

