#include <ecdc/ecdc.h>
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


static const char * const cmd_upgrade_usage =
"upgrade\n"
"    enter uart upgrade mode\n";

static void cmd_upgrade(void *hint, int argc, char const *argv[])
{
	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	printf("entering uart upgrade mode\n");
	uart_upgrade_start();
}

static const char * const cmd_show_usage =
"show\n"
"    show collected information, voltage, current, power etc.\n";

static void cmd_show(void *hint, int argc, char const *argv[])
{
	if (argc != 1) {
		printf("invalid usage\n");
		return;
	}

	mon_put_text();
}

static const char * const cmd_verbose_usage =
"verbose [on|off]\n"
"    switch on or off verbose mode when collect information\n"
"    if on or off ommited, this command shows current verbose mode\n";

static void cmd_verbose(void *hint, int argc, char const *argv[])
{
	if (argc > 2)
		printf("invalid usage\n");
	else if (argc == 1)
		printf("current verbose mode %s\n",
		       mon_get_verbose() ? "on" : "off");
	else if (strcmp("on", argv[1]) == 0)
		mon_set_verbose(true);
	else if (strcmp("off", argv[1]) == 0)
		mon_set_verbose(false);
	else
		printf("invalid verbose mode \'%s\'", argv[1]);
}

static const char * const cmd_sn_usage =
"sn [sn]\n"
"    if sn is ommited, this command shows current sn\n"
"    otherwise, sn will write to eeprom\n";

static void cmd_sn(void *hint, int argc, char const *argv[])
{
	int i;
	uint8_t tmp;

	if (argc > 2)
		printf("invalid usage\n");
	else if (argc == 1) {
		/* read sn out */
		for (i = 0; i < EEPROM_CELL_SIZE; ++i) {
			tmp = eeprom_read_byte(EEPROM_SN_OFFSET + i);
			if (tmp) {
				if (isprint(tmp))
					printf("%c", tmp);
				else
					printf(".");
			} else
				break;
		}
		printf("\n");
	}
	else {
		/* write sn to eeprom */
		for (i = 0; i < EEPROM_CELL_SIZE - 1; ++i) {
			tmp = argv[1][i];
			if (tmp)
				eeprom_write_byte(EEPROM_SN_OFFSET + i, tmp);
			else
				break;
		}
		/* zero terminated */
		eeprom_write_byte(EEPROM_SN_OFFSET + i, 0);
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
	{"upgrade", NULL, cmd_upgrade_usage, cmd_upgrade},
	{"show", NULL, cmd_show_usage, cmd_show},
	{"verbose", NULL, cmd_verbose_usage, cmd_verbose},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
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

void console_test(void)
{
	while (1)
		console_poll();
}
