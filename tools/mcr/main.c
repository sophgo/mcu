#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <console.h>
#include <log.h>


struct comp {
	const char *file;
	unsigned long size;
	unsigned char *buf;
};

static void unload_file(struct comp *comp);
static int load_file(struct comp *comp, const char *file);

char *useage =
"Mcu Console Reference\n"
"useage: mcr device firmware\n"
"example: mcr /dev/ttyUSB0 sc5pro-mcu-v2-2021-02-11-06-18.bin\n";

int main(int argc, char *argv[])
{
	int err, i;
	struct console_ctx console;
	char sn[64];

	log_init();
	if (argc != 3) {
		error("invalid arguments\n");
		error("%s", useage);
		return 1;
	}

	info("SC5PRO uart operations sample code\n");

	err = console_open(&console, argv[1]);

	if (err)
		return abs(err);

	/* get sn */
	err = console_cmd_get_sn(&console, sn);

	if (err < 0) {
		err = abs(err);
		goto err_console_close;
	}


	info("current sn: %s, sn length %d\n", sn, err);

	/* set sn */
	/* generate random sn */
	srand(time(NULL));
	for (i = 0; i < 24; ++i)
		sn[i] = 'A' + rand() % 26;
	sn[i] = 0;

	info("set sn: %s, sn length %ld\n", sn, strlen(sn));

	err = console_cmd_set_sn(&console, sn);

	if (err) {
		err = abs(err);
		goto err_console_close;
	}

	/* get sn again */
	memset(sn, 0, sizeof(sn));
	err = console_cmd_get_sn(&console, sn);

	if (err < 0) {
		err = abs(err);
		goto err_console_close;
	}

	info("current sn: %s, sn length %d\n", sn, err);

	/* upgrade test */
	struct comp firmware;

	info("upgrade firmware\n");
	err = load_file(&firmware, argv[2]);
	if (err) {
		err = abs(err);
		goto err_console_close;
	}

	err = console_cmd_download(&console, 0, firmware.buf, firmware.size);
	if (err) {
		err = abs(err);
		goto err_unload_firmware;
	}

	err = 0;

	err = console_cmd_download(&console, firmware.size / 2,
				   firmware.buf, firmware.size / 2);
	if (err) {
		err = abs(err);
		goto err_unload_firmware;
	}

	err = 0;


err_unload_firmware:
	unload_file(&firmware);
err_console_close:
	console_close(&console);
	return err;
}

static void unload_file(struct comp *comp)
{
	if (comp->buf)
		free(comp->buf);
}

static int load_file(struct comp *comp, const char *file)
{
	int fd;
	int err = -1;
	struct stat stat;

	memset(comp, 0x00, sizeof(*comp));

	comp->file = file;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "cannot load file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		fprintf(stderr, "cannot stat file %s\n", file);
		goto close_file;
	}
	comp->size = stat.st_size;
	comp->buf = malloc(comp->size);
	if (comp->buf == NULL) {
		fprintf(stderr, "cannot malloc buffer for file %s\n", file);
		err = -1;
		goto close_file;
	}
	if (read(fd, comp->buf, comp->size) != comp->size) {
		fprintf(stderr, "cannot load hole file %s\n", file);
		err = -1;
		goto close_file;
	}

	err = 0;
close_file:
	close(fd);
	return err;
}
