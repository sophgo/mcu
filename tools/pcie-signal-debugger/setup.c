#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
#include <opt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>

int setup(struct opt *opt)
{
	int err = -1;
	/* check write right */
	if (access(opt->device, W_OK)) {
		error("cannot write to device %s\n", opt->device);
		return -1;
	}
	/* generate control block */
	struct ctrl_block ctrl;
	memset(&ctrl, 0x00, sizeof(ctrl));
	memcpy(ctrl.magic, MAGIC, sizeof(ctrl.magic));
	ctrl.version = FMT_VER;
	ctrl.time = opt->time;
	snprintf(ctrl.padding,
		 sizeof(ctrl.padding),
		 "version: %d\n"
		"acquire time: %ds\n",
		ctrl.version,
		ctrl.time);
	if (opt->verbose)
		info("%s\n", ctrl.padding);

	int fd;

	fd = open(opt->device, O_RDWR);
	if (fd < 0) {
		error("cannot open device file\n");
		return -1;
	}

	ssize_t size;

	size = write(fd, &ctrl, sizeof(ctrl));
	if (size != sizeof(ctrl)) {
		error("write control block to device failed\n");
		err = -1;
		goto close_file;
	}

close_file:
	close(fd);

	return err;
}
