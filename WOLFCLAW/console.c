#include <ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <timer.h>
#include <pin.h>

static struct ecdc_console *console;
extern int power_is_on;
extern int is_print_enabled;

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
	printf("Hello! Wolfclaw User!\n");
}

static const char * const cmd_getmcutype_usage =
"getmcutype\n"
"    get mcu's type\n";

static void cmd_getmcutype(void *hint, int argc, char const *argv[])
{
	printf("Wolfclaw\n");
}

static const char * const cmd_led_usage =
"led\n"
"    led [led_num] [led_status]\n"
"    control led on/off\n";

static void cmd_led(void *hint, int argc, char const *argv[])
{
	int led_num;

	if (argc != 3){
		printf("%s", cmd_led_usage);
		return;
	}

	led_num = strtol(argv[1], NULL, 0);
	switch (led_num)
	{
	case 1:
		if(strcmp("on", argv[2]) == 0){
			led_1_0_on();
			printf("turn on led 1\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_1_1_on();
			printf("turn off led 1\n");
		}else
			printf("error input status\n");
		break;
	case 2:
		if(strcmp("on", argv[2]) == 0){
			led_2_0_on();
			printf("turn on led 2\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_2_1_on();
			printf("turn off led 2\n");
		}else
			printf("error input status\n");
		break;
	case 3:
		if(strcmp("on", argv[2]) == 0){
			led_3_0_on();
			printf("turn on led 3\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_3_1_on();
			printf("turn off led 3\n");
		}else
			printf("error input status\n");
		break;

	case 4:
		if(strcmp("on", argv[2]) == 0){
			led_4_0_on();
			printf("turn on led 4\n");
		}else if(strcmp("off", argv[2]) == 0){
			led_4_1_on();
			printf("turn off led 4\n");
		}else
			printf("error input status\n");
		break;
	default:
		break;
	}
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL, NULL, cmd_help},
	{"hello", NULL, NULL, cmd_hello},
	{"getmcutype", NULL, cmd_getmcutype_usage, cmd_getmcutype},
	{"led", NULL, cmd_led_usage, cmd_led},
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
		printf("help [command]\n");
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

	for (i = 0; i < ARRAY_SIZE(command_list); ++i) {
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
