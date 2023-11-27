#include <ecdc/ecdc.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <loop.h>
#include <common.h>
#include <upgrade.h>
#include <timer.h>
#include <adc.h>
#include <pin.h>
#include <chip.h>
#include <power.h>
#include <mcu.h>
#include <wdt.h>

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

static const char * const cmd_poweron_usage =
"poweron\n"
"    power on the sg2042x4\n";

static void cmd_poweron(void *hint, int argc, char const *argv[])
{
	power_on();
	if (gpio_get(GPIOE, GPIO_PIN_14) == 1)
		power_is_on = false;

	printf("SG2042X4 POWER ON\n");
}

static const char * const cmd_poweroff_usage =
"poweroff\n"
"    power off the sg2042x4\n";

static void cmd_poweroff(void *hint, int argc, char const *argv[])
{
	power_off();
	if (gpio_get(GPIOE, GPIO_PIN_14) == 0)
		power_is_on = true;

	wdt_reset();
	timer_mdelay(500);

	printf("SG2042X4 POWER OFF\n");
}

static const char * const cmd_reboot_usage =
"reboot\n"
"    reboot the sg2042x4\n";

static void cmd_reboot(void *hint, int argc, char const *argv[])
{
	power_off();
	wdt_reset();
	timer_mdelay(500);
	power_on();
	chip_enable();
	if (gpio_get(GPIOE, GPIO_PIN_14) == 1)
		power_is_on = false;

	printf("SG2042X4 REBOOT\n");
}

static const char * const cmd_reset_usage =
"reset\n"
"    reset the sg2042x4\n";

static void cmd_reset(void *hint, int argc, char const *argv[])
{
	chip_reset();
	wdt_reset();
	printf("SG2042X4 RESET\n");
}

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	printf("Board type: SG2042X4\n");
	printf("PCB Version: %d\n", get_pcb_version());
	printf("PCB type: SG2042_X4_V0_0\n");
	printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";

static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp, soctemp;

	boardtemp = get_board_temp();
	soctemp = get_soc_temp();
	if (argc == 1){
		printf("soc temp = %d(C)\tboard temp = %d(C)\n", soctemp, boardtemp);
	}else if (argc == 2){
		if (strcmp(argv[1], "soc") == 0){
			printf("soc temp = %d(C)\n", soctemp);
		}else if (strcmp(argv[1], "board") == 0){
			printf("board temp = %d(C)\n", boardtemp);
		}
		else
			printf("get %s temp failed\n", argv[1]);
	}else {
		printf(cmd_temp_usage);
	}
}

static const char * const cmd_current_usage =
"current\n"
"    output current one time\n";

static void cmd_current(void *hint, int argc, char const *argv[])
{
	current_print_func();
}

static const char * const cmd_enprint_usage =
"enprint\n"
"    enprint 0/1; 1:output current every second\n";

static void cmd_enprint(void *hint, int argc, char const *argv[])
{
	if (argc == 1){
		is_print_enabled = 1;
	}else if (argc == 2){
		if (strcmp(argv[1], "1") == 0){
			is_print_enabled = 1;
		}else if (strcmp(argv[1], "0") == 0){
			is_print_enabled = 0;
		}
		else
			printf("set enprint 0&1\n");
	}else {
		printf(cmd_enprint_usage);
	}
}

static const char * const cmd_wdt_usage =
"wdt\n"
"    show wdt information\n";

static void cmd_wdt(void *hint, int argc, char const *argv[])
{
	wdt_info_print();
}

uint32_t sys_rst_pin_list[1][2] = {
	{SYS_RST_X_H_PORT, SYS_RST_X_H_PIN},
};

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL, NULL, cmd_help},
	{"poweron", NULL, cmd_poweron_usage, cmd_poweron},
	{"poweroff", NULL, cmd_poweroff_usage, cmd_poweroff},
	{"reboot", NULL, cmd_reboot_usage, cmd_reboot},
	{"reset", NULL, cmd_reset_usage, cmd_reset},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
	{"current", NULL, cmd_current_usage, cmd_current},
	{"wdt", NULL, cmd_wdt_usage, cmd_wdt},
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
