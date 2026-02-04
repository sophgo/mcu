#include <gd32f4xx.h>
#include <ecdc/ecdc.h>
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
#include <raa228234.h>
#include <adc.h>
#include <project.h>
#include <aw95124.h>
#include <power.h>

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

//static void cmd_hello(void *hint, int argc, char const *argv[])
//{
//	dbg_printf("Hello SOPHON SC11E-FP300\n");
//}

static const char * const cmd_volt_usage =
"volt\n"
" get voltage of 1690e from raa228234\n";

static void cmd_volt(void *hint, int argc, char const *argv[])
{
	int i;
	int raa228234_index;
	int page;
	unsigned long val;

	if(argc != 1){
		dbg_printf("invalid usage\n");
		return;
	}

	/* raa228234 read voltage*/
	for(i = 0; i < 4; i++){
		raa228234_index = i / 2;
		page = i % 2;
		//dbg_printf("index = %d, page = %d\n", raa228234_index, page);
		val = raa228234_output_voltage(raa228234_index, page);
		dbg_printf("chip-%d volt val = %ld\n", i, val);
	}
}

static const char * const cmd_volt_set_usage =
"volt_set [chip_num] [val]\n"
" set voltage for 1690e chip\n"
" chip num range 0~3\n";

static void cmd_volt_set(void *hint, int argc, char const *argv[])
{
	int raa228234_index;
	int page;
	int val;

	if(argc != 3){
		dbg_printf("invalid usage\n");
		return;
	}

	/* raa228234 set voltage*/
	raa228234_index = atoi(argv[1]) / 2;
	page = atoi(argv[1]) % 2;
	val = atoi(argv[2]);
	dbg_printf("index = %d, page = %d, val = %d\n", raa228234_index, page, val);
	raa228234_set_out_voltage(raa228234_index, page, val);
}

static const char * const cmd_rdroop_usage =
"rdroop\n"
" get rdroop from raa228234\n";

static void cmd_rdroop(void *hint, int argc, char const *argv[])
{
	int i;
	int raa228234_index;
	int page;
	unsigned long val;

	if(argc != 1){
		dbg_printf("invalid usage\n");
		return;
	}

	/* raa228234 read rdroop*/
	for(i = 0; i < 4; i++){
		raa228234_index = i / 2;
		page = i % 2;
		val = raa228234_out_droop(raa228234_index, page);
		dbg_printf("chip-%d rdroop val = %ld\n", i, val);
	}
}

static const char * const cmd_rdroop_set_usage =
"rdroop_set [chip_num] [val]\n"
" set rdroop for 1690e chip\n"
" chip num range 0~3\n";

static void cmd_rdroop_set(void *hint, int argc, char const *argv[])
{
	int raa228234_index;
	int page;
	int val;

	if(argc != 3){
		dbg_printf("invalid usage\n");
		return;
	}

	/* raa228234 set rdroop*/
	raa228234_index = atoi(argv[1]) / 2;
	page = atoi(argv[1]) % 2;
	val = atoi(argv[2]);
	raa228234_set_out_droop(raa228234_index, page, val);
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
		dbg_printf("invalid usage\n");
	else if (argc == 1) {
		/* read sn out */
		for (i = 0; i < EEPROM_CELL_SIZE; ++i) {
			tmp = p_sn[i];
			if (tmp) {
				if (isprint(tmp))
					dbg_printf("%c", tmp);
				else
					dbg_printf(".");
			} else
				break;
		}
		dbg_printf("\n");
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

uint32_t sys_rst_pin_list[4][2] = {
	{SYS_RST_X_H_BM0_PORT, SYS_RST_X_H_BM0_PIN},
	{SYS_RST_X_H_BM1_PORT, SYS_RST_X_H_BM1_PIN},
	{SYS_RST_X_H_BM2_PORT, SYS_RST_X_H_BM2_PIN},
	{SYS_RST_X_H_BM3_PORT, SYS_RST_X_H_BM3_PIN},
};
static void cmd_low(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		dbg_printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_clear(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	dbg_printf("set chip%lu low\n", pin);

}

static void cmd_high(void *hint, int argc, char const *argv[])
{
	uint32_t pin = 0;
	if (argc != 2) {
		dbg_printf("error cmd\n");
		return;
	}

	pin = strtol(argv[1], NULL, 0);
	gpio_set(sys_rst_pin_list[pin][0], sys_rst_pin_list[pin][1]);

	dbg_printf("set chip%lu high\n", pin);
}

//static const char * const cmd_pg_usage =
//"pg_check\n"
//	"check power good from aw95124 by i2c switch\n";
//
//static void cmd_pg(void *hint, int argc, char const *argv[])
//{
//	if(argc != 1){
//		dbg_printf("invalid usage\n");
//	}
//
//	check_gpio_power_good();
//	check_pg_node();
//}

static const char * const cmd_power_usage =
"power\n"
	"get power of atx 12v, pcie 12v & 3v3\n";

static void cmd_power(void *hint, int argc, char const *argv[])
{
	if(argc != 1){
		dbg_printf("invalid usage\n");
	}

	unsigned long atx_12v_power, pcie_12v_power, pcie_3v3_power;

	pcie_12v_power = adc_read_pcie_i12v() * 12;
	pcie_3v3_power = adc_read_pcie_i3v3() * 33 / 10;
	atx_12v_power = adc_read_sys_i12v() * 12;

	dbg_printf("pcie 3v3 power: %lu mW, pcie 12v power: %lu mW, atx 12v power: %lu mW\n",pcie_3v3_power, pcie_12v_power, atx_12v_power);
}

#if 0
static void cmd_devmem(void *hint, int argc, char const *argv[])
{
	uint32_t reg_val;
	uint32_t addr;

	addr = (uint32_t)strtol(argv[1], NULL, 16);
	reg_val = REG32(addr);
	dbg_printf("0x%x",reg_val);
}
#endif

static const char * const cmd_info_usage =
"info\n"
"    get information about board and mcu\n";

static void cmd_info(void *hint, int argc, char const *argv[])
{
	dbg_printf("Chip type: BM1690E\n");
	dbg_printf("PCB Version: %d\n", get_pcb_ver());
	dbg_printf("Board type: %s\n", "SC11EFP300");
	dbg_printf("MCU_SW_VER: %d\n", MCU_SW_VER);
}

static const char * const cmd_temp_usage =
"temp\n"
"    temp soc&board\n";
static void cmd_temp(void *hint, int argc, char const *argv[])
{
	int boardtemp0, soctemp0, boardtemp1, soctemp1, boardtemp2, soctemp2, boardtemp3, soctemp3;


	boardtemp0 = get_board_temp(0);
	soctemp0 = get_soc_temp(0);
	boardtemp1 = get_board_temp(1);
	soctemp1 = get_soc_temp(1);
	boardtemp2 = get_board_temp(2);
	soctemp2 = get_soc_temp(2);
	boardtemp3 = get_board_temp(3);
	soctemp3 = get_soc_temp(3);
	if (argc == 1){
		dbg_printf("CHIP0: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp0, boardtemp0);
		dbg_printf("CHIP1: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp1, boardtemp1);
		dbg_printf("CHIP2: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp2, boardtemp2);
		dbg_printf("CHIP3: soc temp = %d(C)\tboard temp = %d(C)\n", soctemp3, boardtemp3);
	}else {
		dbg_printf(cmd_temp_usage);
	}
}

struct command {
	const char *name, *alias, *usage;
	ecdc_callback_fn fn;
};

static void cmd_help(void *hint, int argc, char const *argv[]);

static struct command command_list[] = {
	{"help", NULL , NULL, cmd_help},
//	{"hello", NULL , NULL, cmd_hello},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
	{"volt", NULL, cmd_volt_usage, cmd_volt},
	{"volt_set", NULL, cmd_volt_set_usage, cmd_volt_set},
	{"rdroop", NULL, cmd_rdroop_usage, cmd_rdroop},
	{"rdroop_set", NULL, cmd_rdroop_set_usage, cmd_rdroop_set},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
//	{"pg_check", NULL, cmd_pg_usage, cmd_pg},
	{"power", NULL, cmd_power_usage, cmd_power},
	{"low", NULL, NULL, cmd_low},
	{"high", NULL, NULL, cmd_high},
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
			dbg_printf("\'%s\' not found\n", argv[1]);
	} else {
		dbg_printf("invalid usage\n");
		dbg_printf("help [command]");
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
