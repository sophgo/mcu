#include <ctype.h>
#include <ecdc/ecdc.h>
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
#include <isl68224.h>
#include <flash.h>
#include <freq.h>

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
"power\n";

static void cmd_power(void *hint, int argc, char const *argv[])
{
	unsigned long atx_12v_power, pcie_12v_power, pcie_3v3_power;
	unsigned long atx_12v_current, pcie_12v_current, pcie_3v3_current;

	pcie_12v_current = adc_read_pcie_12v_current();
	pcie_3v3_current = adc_read_pcie_3v3_current();
	atx_12v_current = adc_read_atx_12v_current();

	pcie_12v_power = adc_read_pcie_12v_current() * 12 ;
	pcie_3v3_power = adc_read_pcie_3v3_current() * 33 / 10 ;
	atx_12v_power = adc_read_atx_12v_current() * 12 ;

	dbg_printf("pcie 3v3 power: %lu mW, pcie 12v power: %lu mW, atx 12v power: %lu mW\n",pcie_3v3_power, pcie_12v_power, atx_12v_power);
	dbg_printf("pcie 3v3 current: %lu mA, pcie 12v current: %lu mA, atx 12v current: %lu mA\n",pcie_3v3_current, pcie_12v_current, atx_12v_current);
}

#if 0
static  const char * const cmd_freq_usage =
"freq\n";

static void cmd_freq(void *hint, int argc, char const *argv[])
{
	int rate, mpll_id;
	if (argc != 3){
		mpll_id = atoi(argv[1]);
		rate = atoi(argv[2]);
		sg2044_clk_pll_set_rate(mpll_id, rate, PARENT_FREQ);
	}
}
#endif

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
*/

static const char * const cmd_info_usage =
"info\n";

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
"pg_check\n";

static void cmd_pg(void *hint, int argc, char const *argv[])
{
	if(argc != 1){
		dbg_printf("invalid usage\n");
	}

	check_gpio_power_good();
}
*/

static const char * const cmd_volt_usage =
"volt [val]\n";

static void cmd_volt(void *hint, int argc, char const *argv[])
{
	int page = 0;
	unsigned long val;

	if(argc == 1){
		/* raa228234 read voltage*/
		val = isl68224_output_voltage(page);
		dbg_printf("volt val = %ld\n", val);
	} else if (argc == 2) {
		/* raa228234 set voltage*/
		val = atoi(argv[1]);
		isl68224_set_out_voltage(page, val);
	} else {
		dbg_printf("invalid usage\n");
		return;
	}

}

static const char * const cmd_rdroop_usage =
"rdroop [val]\n";

static void cmd_rdroop(void *hint, int argc, char const *argv[])
{
	int page = 0;
	unsigned long val;

	if(argc == 1){
		/* raa228234 read rdroop*/
		val = isl68224_out_droop(page);
		dbg_printf("rdroop val = %ld\n", val);
	} else if (argc == 2){
		/* raa228234 set rdroop*/
		val = atoi(argv[1]);
		isl68224_set_out_droop(page,val);
	} else {
		dbg_printf("invalid usage\n");
		return;
	}

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
//	{"freq", NULL, cmd_freq_usage, cmd_freq},
	{"sn", NULL, cmd_sn_usage, cmd_sn},
//	{"reboot", NULL, cmd_reboot_usage, cmd_reboot},
	{"info", NULL, cmd_info_usage, cmd_info},
	{"temp", NULL, cmd_temp_usage, cmd_temp},
//	{"query", NULL, cmd_query_usage, cmd_query},
//	{"enprint", NULL, cmd_enprint_usage, cmd_enprint},
//	{"current", NULL, cmd_current_usage, cmd_current},
//	{"upgrade", NULL, cmd_upgrade_usage, cmd_upgrade},
//	{"reset", NULL, cmd_reset_usage, cmd_reset},
//	{"pg_check", NULL, cmd_pg_usage, cmd_pg},
	{"volt", NULL, cmd_volt_usage, cmd_volt},
	{"rdroop", NULL, cmd_rdroop_usage, cmd_rdroop},
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
