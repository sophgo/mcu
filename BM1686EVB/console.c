#include <ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <mon.h>
#include <upgrade.h>
#include <eeprom.h>
#include <timer.h>
#include <power.h>
#include <slt.h>
#include <loop.h>
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
	printf("Hello BITMAIN SOPHON\n");
}

static const char * const cmd_poweron_usage =
"poweron\n"
"    power on the bm1684x\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
	power_on();
}

static const char * const cmd_poweroff_usage =
"poweroff\n"
"    power off the bm1684x\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
	slt_reset();
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL , NULL, cmd_help},
	{"hello", NULL , NULL, cmd_hello},
	{"poweron", NULL , cmd_poweron_usage, cmd_poweron},
	{"poweroff", NULL , cmd_poweroff_usage, cmd_poweroff},
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

	console_putc(NULL, '\r');
	console_putc(NULL, '\n');

	return 0;
}

void console_poll(void)
{
	ecdc_pump_console(console);
}

void console_add(void)
{
	console_init();
	loop_add(console_poll);
}

void console_test(void)
{
	while (1)
		console_poll();
}
