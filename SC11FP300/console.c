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
#include <flash.h>
#include <timer.h>
#include <pin.h>
#include <isl68224.h>
#include <gd32e50x.h>
#include <adc.h>
#include <mon.h>
#include <project.h>

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
	printf("Hello SOPHON SC11-FP300\n");
}

static const char * const cmd_sn_usage =
"sn [sn]\n"
"    if sn is ommited, this command shows current sn\n"
"    otherwise, sn will write to eeprom\n";

static void cmd_sn(void *hint, int argc, char const *argv[])
{
	int i;
	uint8_t tmp;
	uint8_t flash_data[FLASH_PAGE_SIZE] = {0};
	volatile uint8_t *p_sn = (uint8_t *) SN_BASE;

	if (argc > 2)
		printf("invalid usage\n");
	else if (argc == 1) {
		/* read sn out */
		for (i = 0; i < EEPROM_CELL_SIZE; ++i) {
			tmp = p_sn[i];
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
			for (i = 0; i < FLASH_PAGE_SIZE; i++){
				flash_data[i] = p_sn[i];
			}

			/* write sn to eeprom */
			for (i = 0; i < SN_CELL_SIZE - 1; ++i) {
				tmp = argv[1][i];

				if (tmp){
					flash_data[SN_CELL_OFFSET(0) + i] = tmp;
				}
				else
					break;
			}
			/* zero terminated */
			flash_data[SN_CELL_OFFSET(0) + i] = 0;
			flash_program_page(SN_BASE, flash_data, FLASH_PAGE_SIZE);
	}
}
#if 0
uint32_t sys_rst_pin_list[2][2] = {
	{SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN},
	{SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN},
};
static void cmd_low(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_clear(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	printf("set chip%lu low\n", pin);

}

static void cmd_high(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_set(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	printf("set chip%lu high\n", pin);
}

static void cmd_devmem(void *hint, int argc, char const *argv[])
{
	uint32_t reg_val;
	uint32_t addr;

	addr = (uint32_t)strtol(argv[1], NULL, 16);
	reg_val = REG32(addr);
	printf("0x%x",reg_val);
}

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	printf("Chip type: BM1690\n");
	printf("PCB Version: %d\n", get_pcb_version());
	printf("Board type: %s\n", SC11FP300);
	printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

#endif

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";
static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp0, soctemp0, boardtemp1, soctemp1;


	boardtemp0 = get_board_temp(0);
	soctemp0 = get_soc_temp(0);
	boardtemp1 = get_board_temp(1);
	soctemp1 = get_soc_temp(1);
	if (argc == 1){
		printf("CHIP0: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp0, boardtemp0);
		printf("CHIP1: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp1, boardtemp1);
	}else if (argc == 2){
		if (strcmp(argv[1], "soc") == 0){
			printf("chip0:soc temp = %d(C)\n", soctemp0);
			printf("chip1:soc temp = %d(C)\n", soctemp1);
		}else if (strcmp(argv[1], "board") == 0){
			printf("chip0:board temp = %d(C)\n", boardtemp0);
			printf("chip1 board temp = %d(C)\n", boardtemp1);
		}
		else
			printf("get %s temp failed\n", argv[1]);
	}else {
		printf(cmd_temp_usage);
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
	//{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	// {"show", NULL, cmd_show_usage, cmd_show},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
	// {"low", NULL, NULL, cmd_low},
	// {"high", NULL, NULL, cmd_high},
	// {"devmem", NULL, NULL, cmd_devmem},
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
