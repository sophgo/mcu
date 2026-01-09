#include <ecdc/ecdc.h>
#include <ctype.h>
#include <system.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <common.h>
#include <upgrade.h>
#include <timer.h>
#include <adc.h>
#include <pin.h>
#include <chip.h>
#include <power.h>
#include <i2c-slaves/mcu.h>
#include <i2c-slaves/slt.h>
#include <loop.h>
#include <board_power_impl.h>
#include <project.h>

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

static const char * const cmd_power_usage =
"power\n"
"    Reading pcie 12V & 3v3 and atx 12V power.\n";

static void cmd_power(void *hint, int argc, char const *argv[])
{
	unsigned long atx_12v_power, pcie_12v_power, pcie_3v3_power;

	pcie_12v_power = adc_read_pcie_12v_current() * 12 ;
	pcie_3v3_power = adc_read_pcie_3v3_current() * 33 / 10 ;
	atx_12v_power = adc_read_atx_12v_current() * 12 ;

	dbg_printf("pcie 3v3 power: %lu mW, pcie 12v power: %lu mW, atx 12v power: %lu mW\n",pcie_3v3_power, pcie_12v_power, atx_12v_power);
}


/*
static const char * const cmd_reboot_usage =
"reboot\n"
"    reboot the bm1690evb\n";

static void cmd_reboot(void *hint, int argc, char const *argv[])
{
	power_off();
	timer_mdelay(500);
	power_on();
	chip_enable();
	if (get_board_type() == BM1690EVB) {
		power_is_on = false;
		printf("BM1690EVB REBOOT\n");
	}
}
*/

static const char* const cmd_reset_usage =
"reset\n"
" reset everything on board\n";

static void cmd_reset(void *hint, int argc, char const *argv[])
{
	gpio_bit_reset(PCIE4_L0_RESET_X_H_PORT,PCIE4_L0_RESET_X_H_PIN);

	if(gpio_output_bit_get(PCIE4_L0_RESET_X_H_PORT,PCIE4_L0_RESET_X_H_PIN)){
		dbg_printf("PCIE4_L0_RESET_X_H pull down failed\n");
	}else{
		dbg_printf("PCIE4_L0_RESET_X_H pull down success\n");
	}
}

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	dbg_printf("Chip type: BM1690E\n");
	dbg_printf("PCB Version: %d\n", get_pcb_version());
	dbg_printf("Board type: %s\n", get_board_type_name());
	dbg_printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";

static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp, soctemp, soctemp_x;

	boardtemp = get_board_temp();
	soctemp = get_soc_temp();
	soctemp_x = get_soc_temp_x();
	if (argc == 1){
		dbg_printf("soc temp = %d(C)\tboard temp = %d(C)\torigin = %d(C)\n", soctemp, boardtemp, soctemp_x);
	}else {
		dbg_printf(cmd_temp_usage);
	}
}

/*
static const char * const cmd_query_usage =
"query\n"
"    query slt reg result\n"
"    query (reg_num)\n";

static void cmd_query(void *hint, int argc, char const *argv[])
{
	int reg = 0;
	if (argc == 1){
		for (reg = 0; reg < 6; reg += 2){
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}else if (argc == 2){
		reg = atoi(argv[1]);
		if (reg >= 64 || reg < 0){
			printf("reg%d inexist\n",reg);
		}else {
			uint16_t result = get_slt_result(reg);
			printf("reg%d&reg%d = 0x%04x\n", (reg + 1), reg, result);
		}
	}
	else
		printf(cmd_query_usage);
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

*/
static const char * const cmd_upgrade_usage =
"upgrade\n"
"    enter uart upgrade mode\n";
static void cmd_upgrade(void *hint, int argc, char const *argv[])
{
	if (argc != 1) {
		dbg_printf("invalid usage\n");
		return;
	}

	dbg_printf("entering uart upgrade mode\n");
	uart_upgrade_start();
}

static const char * const cmd_pg_usage =
"pg_check\n"
	"check power good from aw95124 by i2c switch\n";

static void cmd_pg(void *hint, int argc, char const *argv[])
{
	if(argc != 1){
		dbg_printf("invalid usage\n");
	}

	check_gpio_power_good();
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
	{"power", NULL, cmd_power_usage, cmd_power},
//	{"reboot", NULL, cmd_reboot_usage, cmd_reboot},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
//	{"query", NULL, cmd_query_usage, cmd_query},
//	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
//	{"current", NULL, cmd_current_usage, cmd_current},
	{"upgrade", NULL, cmd_upgrade_usage, cmd_upgrade},
	{"reset", NULL, cmd_reset_usage, cmd_reset},
	{"pg_check", NULL, cmd_pg_usage, cmd_pg},
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
			dbg_printf("\'%s\' not found\n", argv[1]);
	} else {
		dbg_printf("invalid usage\n");
		dbg_printf("help [command]\n");
	}
}

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
	if (console == NULL) {
		dbg_printf("create console failed\n");
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
