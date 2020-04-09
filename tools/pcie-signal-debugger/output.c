#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
#include <opt.h>
#include <log.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int output(struct opt *opt)
{
	int err = -1;
	/* check write right */
	if (access(opt->device, R_OK)) {
		error("cannot read from device %s\n", opt->device);
		return -1;
	}
	/* generate control block */
	struct ctrl_block ctrl;

	int ifd, ofd;

	ifd = open(opt->device, O_RDONLY);
	if (ifd < 0) {
		error("cannot open device file\n");
		return -1;
	}

	ssize_t size;
	size = read(ifd, &ctrl, sizeof(ctrl));
	if (size != sizeof(ctrl)) {
		error("read control block failed\n");
		goto close_input_file;
	}

	if (memcmp(ctrl.magic, MAGIC, sizeof(ctrl.magic))) {
		warn("not a valid control block\n");
		warn("take it easy, maybe user donnot init it, use device default config\n");
	} else {
		info("version: %u\n"
		     "acquire time: %ds\n"
		     , ctrl.version, ctrl.time);
	}

	struct info_block info;
	size = read(ifd, &info, sizeof(info));
	if (size != sizeof(info)) {
		error("read information block failed\n");
		goto close_input_file;
	}

	info("channel number: %d\n"
	     "sample rate: %d\n"
	     "bits per sample: %d\n"
	     "acquired samples: %ld\n",
	     info.channels, info.sample_rate,
	     info.bits_per_sample, info.samples);
	if (opt->verbose)
		info("signal data offset: %d bytes\n", info.data_offset);

	if (info.bits_per_sample != 8) {
		error("no support for sample depth which is not 8\n");
		goto close_input_file;
	}

	int i = 0;
	int ch;
	char channel_labels[16][64];
	double channel_scale[16];

	for (ch = 0; ch < info.channels; ++ch) {
		while (info.channel_labels[i] == 0) {
			++i;
			if (i > sizeof(info.channel_labels)) {
				error("channel label error");
				goto close_input_file;
			}
		}
		strcpy(channel_labels[ch], info.channel_labels + i);
		info("channel %d: %s\n", ch, channel_labels[ch]);
		while (info.channel_labels[i]) {
			++i;
			if (i > sizeof(info.channel_labels)) {
				error("channel label error");
				goto close_input_file;
			}
		}
	}

	for (ch = 0; ch < info.channels; ++ch) {
		char *s;
		s = strstr(channel_labels[ch], "scale:");
		if (s == NULL) {
			/* no scale key word */
			/* set to default 3.3v */
			channel_scale[ch] = 3.3;
			continue;
		}
		/* find first digit */
		while (!isdigit(*s) && *s)
			++s;
		if (*s == 0) {
			warn("cannot find any digit\n");
			channel_scale[ch] = 3.3;
			continue;
		}

		char tmp[32];
		for (i = 0; isdigit(*s) || *s == '.'; ++i, ++s)
			tmp[i] = *s;
		tmp[i] = 0;
		sscanf(tmp, "%lf", channel_scale + ch);
		debug("channel %d scale %s (%lf)\n",
		      ch, tmp, channel_scale[ch]);
	}

	for (ch = 0; ch < info.channels; ++ch) {
		char *s;
		for (s = channel_labels[ch]; *s && *s != ','; ++s)
			;
		*s = 0;
	}

	if (opt->output[0] == 0) {
		debug("no output file, just show information\n");
		err = 0;
		goto close_input_file;
	}

	info("output file %s\n", opt->output);
	if (opt->time)
		info("%u seconds waveform will be exported\n", opt->time);
	ofd = open(opt->output, O_RDWR | O_TRUNC | O_CREAT,
		   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (ofd < 0) {
		error("cannot create output file\n");
		err = 1;
		goto close_input_file;
	}

	const char *fmt_header =
		"* CAZM-format output from SM5-PCIE-BASE-BOARD *\n"
		"\n"
		"TRANSIENT ANALYSIS\n";

	size = write(ofd, fmt_header, strlen(fmt_header));
	if (size != strlen(fmt_header)) {
		error("write file format header failed\n");
		goto close_output_file;
	}
	if (dprintf(ofd, "Time ") < 0) {
		error("write channel label failed\n");
		goto close_output_file;
	}

	for (ch = 0; ch < info.channels; ++ch) {
		dprintf(ofd, "%s", channel_labels[ch]);
		if (ch != info.channels - 1)
			dprintf(ofd, " ");
		else
			dprintf(ofd, "\n");
	}

	if (lseek(ifd, info.data_offset, SEEK_SET) < 0) {
		error("seek to data offset failed\n");
		goto close_output_file;
	}

	unsigned long long p;
	double t = 0;
	double d = (double)1 / info.sample_rate;
	double total_time = opt->time ?
		opt->time : (double)info.samples / info.sample_rate;
	for (p = 0; p < info.samples; ++p) {
		char tmp[512];
		unsigned char data[16];
		if (read(ifd, data, info.channels) != info.channels) {
			error("cannot get acquired data\n");
			goto close_output_file;
		}
		int x;
		x = sprintf(tmp, "%e ", t);
		for (i = 0; i < info.channels; ++i) {
			double scale = channel_scale[i];

			if (i != info.channels - 1)
				x += sprintf(tmp + x, "%e ",
					     (double)data[i] * scale / 255);
			else
				x += sprintf(tmp + x, "%e\n",
					     (double)data[i] * scale / 255);
		}

		if (write(ofd, tmp, strlen(tmp)) != strlen(tmp)) {
			error("cannot save acquired data\n");
			goto close_output_file;
		}
		t += d;
		info("%.2lf%%\r", (t / total_time) * 100);
		if (opt->time && t > opt->time)
			break;
	}

	info("save captured signal success\n");

close_output_file:
	close(ofd);
close_input_file:
	close(ifd);

	return err;
}
