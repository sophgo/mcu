#include <ecdc/ecdc.h>
#include <stdio.h>
#include <stdstub.h>
#include <config.h>

static struct ecdc_console *console;

extern unsigned long current;
extern unsigned long voltage;

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
	printf("Hello BITMAIN SM5 PCIE adapter\n\r");
}

static void cmd_current(void *hint, int argc, char const *argv[])
{
	printf("%lumA\n\r",
	       (unsigned long)(current * CURRENT_SCALE / 255));
}

static void cmd_voltage(void *hint, int argc, char const *argv[])
{
	printf("%lumV\n\r",
	       (unsigned long)(voltage * VOLTAGE_SCALE / 255));
}

static void cmd_power(void *hint, int argc, char const *argv[])
{
	unsigned long c = current * CURRENT_SCALE / 255;
	unsigned long v = voltage * VOLTAGE_SCALE / 255;
	printf("%lumW\n\r", c * v / 1000);
}

struct command {
	char *name, *alias;
	ecdc_callback_fn fn;
};

static struct command command_list[] = {
	{"hello", NULL , cmd_hello},
	{"current", "c", cmd_current},
	{"voltage", "v", cmd_voltage},
	{"power", "p", cmd_power},
};

int console_init(void)
{
	int i;

	console = ecdc_alloc_console(NULL, console_getc, console_putc, 128, 4);
	if (console == NULL) {
		printf("create console failed\r\n");
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

	ecdc_alloc_list_command(console, "help");

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
