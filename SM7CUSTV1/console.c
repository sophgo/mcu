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
#include <chip.h>
#include <mon_print.h>
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

static const char cmd_poweron_usage[] =
"poweron\n"
"    power on the bm1684x\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
	power_on();
	chip_enable();

	printf("BM1684XOK\n");
}

static const char cmd_poweroff_usage[] =
"poweroff\n"
"    power off the bm1684x\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
	slt_reset();

	printf("BM1684XOK\n");
}

static const char cmd_getmcutype_usage[] =
"getmcutype\n";

static void cmd_getmcutype(void *hint, int argc, char const *argv[])
{
	printf("SM7CUSTV1\n");
}

static const char cmd_query_usage[] =
"query\n"
"	query the result from bm1684x\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
	printf("0x%x\n", get_slt_result());
}

static const char  cmd_power_usage[] =
"power [on|off power_name]\n"
"	on: enable power\n"
"	off: disable power\n"
"	if on or off ommited, get the status of power enable pin\n";

static void cmd_power(void *hint, int argc, char const *argv[])
{
	int ret = 0;

	if (argc == 1){
		cmd_get_node_status();
	}else if (argc == 3){
		if (strcmp(argv[1], "on") == 0){
			ret = power_node_on(argv[2]);
			printf(argv[2]);

			if (ret == -1)
				printf(" enable failed\n");
			else
				printf(" enable success\n");
		}else if (strcmp(argv[1], "off") == 0){
			power_node_off(argv[2]);
		}
		else
			printf("invalid command\n");
	}else{
		printf(cmd_power_usage);
	}
}

static const char  cmd_enprint_usage[] =
"enprint [second]\n"
"	second: set time between two pinrt\n"
"	default value is 1s\n";

static void cmd_enprint(void *hint, int argc, char const *argv[])
{
	unsigned long time = 0;

	if (argc == 1){
		mon_print_fun();
		enable_mon_print_task(0);
	}else if (argc == 2){
		mon_print_fun();
		time = strtol(argv[1], NULL, 10);
		enable_mon_print_task(time*1000);
	}else{
		printf(cmd_enprint_usage);
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
	{"poweron", NULL , cmd_poweron_usage, cmd_poweron},
	{"poweroff", NULL , cmd_poweroff_usage, cmd_poweroff},
	{"getmcutype", NULL, cmd_getmcutype_usage, cmd_getmcutype},
	{"query", NULL, cmd_query_usage, cmd_query},
	{"power", NULL, cmd_power_usage, cmd_power},
	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
};

void print_usage(struct command *cmd)
{
	if (cmd->usage)
		printf(cmd->usage);
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
