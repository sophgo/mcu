#include <ecdc/ecdc.h>
#include <stdio.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <flash.h>

static struct ecdc_console *console;

static int console_getc(void *console_hint)
{
	int err;

	err = uart_getc();
	return err >= 0 ? err : ECDC_GETC_EOF;
}

static void console_putc(void *console_hint, char c)
{
	uart_putc(c);
}

static void cmd_hello(void *hint, int argc, char const *argv[])
{
	printf("Hello BITMAIN SOPHON, version %s\n", VERSION);
}

static const char * const cmd_download_usage =
"download [offset]\n"
"    download file from host to program flash through xmodem\n"
"    if offset ommitted, program downloaded file from the beginning of program flash\n";

static void cmd_download(void *hint, int argc, char const *argv[])
{
	long int offset = 0;

	if (argc > 2) {
		printf("invalid usage\n");
		return;
	}

	if (argc == 2) {
		offset = strtol(argv[1], NULL, 0);
		if (offset == LONG_MIN || offset == LONG_MAX) {
			printf("invalid argument\n");
			return;
		}
	}

	printf("download to 0x%lx though xmodem\n", offset);

	if (download_to_flash(offset) < 0)
		printf("download failed\n");
	else
		printf("download success\n");
}

static const char * const cmd_burn_usage =
"burn\n"
"    burn firmware to program flash through xmodem\n";

static void cmd_burn(void *hint, int argc, char const *argv[])
{
	long int firmware_size;
	int err;

	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	printf("download firmware though xmodem\n");

	firmware_size = download_to_flash(0);
	if (firmware_size < 0) {
		printf("download failed\n");
		return;
	}

	err = check_firmware(firmware_size);

	while (uart_getc() != '\r') {
		printf("firmware upgrade %s, press enter return to console\n",
		       err ? "failed" : "success");
		mdelay(200);
	}
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL , NULL, cmd_help},
	{"hello", NULL , NULL, cmd_hello},
	{"download", NULL, cmd_download_usage, cmd_download},
	{"burn", NULL, cmd_burn_usage, cmd_burn},
};

void print_usage(struct command *cmd)
{
	if (cmd->usage)
		uart_puts(cmd->usage);
}

struct command *find_command(const char *name)
{
	int i;
	struct command *cmd;

	for (i = 0; i < ARRAY_SIZE(command_list); ++i) {
		cmd = &command_list[i];
		if (strcmp(cmd->name, name) == 0)
			return cmd;
	}

	return NULL;
}

static void cmd_help(void *hint, int argc, char const *argv[])
{
	int i;
	struct command *cmd;

	if (argc == 1) {
		for (i = 0; i < ARRAY_SIZE(command_list); ++i)
			print_usage(&command_list[i]);
	} else if (argc == 2) {
		cmd = find_command(argv[1]);
		if (cmd)
			print_usage(cmd);
		else
			printf("\'%s\' not found\n", argv[1]);
	} else {
		printf("invalid usage\n");
		printf("help [command]");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
	if (console == NULL) {
		printf("create console failed\n");
		return -1;
	}
	ecdc_configure_console(console, ECDC_MODE_ANSI, ECDC_SET_LOCAL_ECHO);

	for (i = 0; i < sizeof(command_list) / sizeof(command_list[0]); ++i) {
		if (command_list[i].name)
			ecdc_alloc_command(NULL, console,
					   command_list[i].name,
					   command_list[i].fn);
		if (command_list[i].alias)
			ecdc_alloc_command(NULL, console,
					   command_list[i].alias,
					   command_list[i].fn);
	}

	ecdc_alloc_list_command(console, "list");

	return 0;
}

void console_poll(void)
{
	ecdc_pump_console(console);
}

void console_test(void)
{
	while (1)
		console_poll();
}
