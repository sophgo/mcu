#include <adc.h>
#include <system.h>
#include <stdio.h>
#include <tick.h>
#include <project.h>
#include <mp5475.h>
#include <isl68224.h>
#include <base64.h>
#include <stdarg.h>

#define OUTPUT_BASE64
// #define OUTPUT_TEXT

// #define FILTER_DISABLE

#define MCU_SW_VER	0
#define MCU_HW_VER	0

#define STX	0x02	/* start of text */
#define ETX	0x03	/* end of text */

#define BROADCAST_BUF_SIZE	1024
#define BROADCAST_INTERVAL	200

#define FILTER_DEPTH_SHIFT	6
#define FILTER_DEPTH		(1 << FILTER_DEPTH_SHIFT)
#define FILTER_DEPTH_MASK	(FILTER_DEPTH - 1)

struct filter {
	unsigned short data[FILTER_DEPTH];
	unsigned long total;
	unsigned long value;
	int p;
};

static unsigned long filter_init(struct filter *f, unsigned long d)
{
	int i;

	f->p = 0;
	f->total = 0;
	for (i = 0; i < FILTER_DEPTH; ++i) {
		f->data[i] = d;
		f->total += d;
	}
	return d;
}

static unsigned long filter_in(struct filter *f, unsigned long d)
{
	f->total -= f->data[f->p];
	f->total += d;
	f->data[f->p] = d;
	f->p = (f->p + 1) & FILTER_DEPTH_MASK;
	f->value = f->total >> FILTER_DEPTH_SHIFT;

#ifdef FILTER_DISABLE
	return d;
#else
	return f->value;
#endif
}

static unsigned long last_time;
static struct filter i12v;
static struct filter mp5475_current[4][4];
static struct filter mp5475_voltage[4][4];
static struct filter isl68224_current[4][3];
static struct filter isl68224_voltage[4][3];
static struct filter isl68224_power[4][3];

static struct {
	uint8_t id;
	uint8_t type;
	uint8_t sw_ver;
	uint8_t hw_ver;
	uint32_t i12v;

	struct {
		struct {
			uint32_t voltage;
			uint32_t current;
		} buck[4];
	} mp5475[4];

	struct {
		struct {
			uint32_t voltage;
			uint32_t current;
			uint32_t power;
		} rail[3];
	} isl68224[4];

	uint32_t checksum;
} __attribute__((packed)) __attribute__((aligned(4))) pkg;

static unsigned long pkg_base64_size;

void mon_init(void)
{
	int i, j;

	filter_init(&i12v, 0);

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			filter_init(&(mp5475_current[i][j]), 0);
			filter_init(&(mp5475_voltage[i][j]), 0);
		}
	}

	pkg.type = SC5PRO;
	pkg.sw_ver = MCU_SW_VER;
	pkg.hw_ver = MCU_HW_VER;

	pkg_base64_size = (sizeof(pkg) + 2) / 3 * 4;

	last_time = tick_get();
}

static void collect_mp5475(void)
{
	int i, j;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {

			pkg.mp5475[i].buck[j].current =
				filter_in(&(mp5475_current[i][j]),
					  mp5475_output_current(i, j));

			pkg.mp5475[i].buck[j].voltage =
				filter_in(&(mp5475_voltage[i][j]),
					  mp5475_output_voltage(i, j));
		}
	}
}

static void __attribute__((unused))
collect_isl68224(void)
{
	int i, j;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 3; ++j) {

			pkg.isl68224[i].rail[j].current =
				filter_in(&(isl68224_current[i][j]),
					  isl68224_output_current(i, j));

			pkg.isl68224[i].rail[j].voltage =
				filter_in(&(isl68224_voltage[i][j]),
					  isl68224_output_voltage(i, j));

			pkg.isl68224[i].rail[j].power =
				filter_in(&(isl68224_power[i][j]),
					  isl68224_output_power(i, j));
		}
	}
}

static void collect_checksum(void)
{
	uint32_t *p, *s, *e;
	uint32_t sum = 0;

	s = (uint32_t *)&pkg;
	e = (uint32_t *)((unsigned long)s + sizeof(pkg) - 4);

	for (p = s; p != e; ++p)
		sum += *p;

	pkg.checksum = sum;
}

static void collect(void)
{
	/* get 12v current from adc */
	pkg.i12v = filter_in(&i12v, adc_read());
	collect_mp5475();
	collect_isl68224();
	collect_checksum();

	++pkg.id;
}

static inline void mon_puts(char *s)
{
	char *p;

	for (p = s; *p; ++p)
		usart_send_blocking(MON_UART, *p);
}

static inline void mon_putc(int c)
{
	usart_send_blocking(MON_UART, c);
}

int mon_printf(const char *fmt, ...)
{
	va_list ap;
	char p[128];
	int len;

	va_start(ap, fmt);
	len = vsnprintf(p, sizeof(p), fmt, ap);
	va_end(ap);

	char *q;

	for (q = p; *q; ++q)
		usart_send_blocking(MON_UART, *q);

	return len;
}

static void __attribute__((unused))
mon_put_base64(void *str, unsigned long size)
{
	mon_putc(STX);
	int col_mask = 64 - 1;
	int i;
	for (i = 0; i < size; ++i) {
		if ((i & col_mask) == 0 && i != 0)
			mon_printf("\r\n");
		mon_printf("%c", ((char *)str)[i]);
	}
	mon_printf("\r\n");
	mon_putc(ETX);
}

static void __attribute__((unused))
mon_put_text(void)
{
	mon_putc(STX);
	int i, j;

	mon_printf("id: %u\r\n", pkg.id);
	mon_printf("type: %u\r\n", pkg.type);
	mon_printf("sw_ver: %u\r\n", pkg.sw_ver);
	mon_printf("hw_ver: %u\r\n", pkg.hw_ver);

	mon_printf("12v-current: %lu\r\n", pkg.i12v);

	mon_printf("{\r\n");
	for (i = 0; i < 4; ++i) {
		mon_printf("\tmp5475%d:\r\n", i);
		for (j = 0; j < 4; ++j) {
			mon_printf("\t\tbuck%d ", j);
			mon_printf("%lu %lu\r\n",
			       pkg.mp5475[i].buck[j].voltage,
			       pkg.mp5475[i].buck[j].current);
		}
	}
	mon_printf("}\r\n");

	mon_printf("{\r\n");
	for (i = 0; i < 4; ++i) {
		mon_printf("\tisl68224%d:\r\n", i);
		for (j = 0; j < 3; ++j) {
			mon_printf("\t\trail%d ", j);
			mon_printf("%lu %lu %lu\r\n",
			       pkg.isl68224[i].rail[j].voltage,
			       pkg.isl68224[i].rail[j].current,
			       pkg.isl68224[i].rail[j].power);
		}
	}
	mon_printf("}\r\n");


	mon_printf("checksum: 0x%08lx\r\n", pkg.checksum);
	mon_putc(ETX);
}

static void broadcast(void)
{
	char text[BROADCAST_BUF_SIZE];

	base64_encode(text, &pkg, sizeof(pkg));

#ifdef OUTPUT_BASE64
	mon_put_base64(text, pkg_base64_size);
#endif
#ifdef OUTPUT_TEXT
	mon_put_text();
#endif
}

void mon_process(void)
{
	unsigned long current_time;

	collect();

	current_time = tick_get();
	if (current_time - last_time > BROADCAST_INTERVAL) {
		broadcast();
		last_time = current_time;
	}
}
