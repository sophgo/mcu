#include <stdio.h>
#include <stdarg.h>

#define MSG_BUF_LEN	2048
static volatile char msg[MSG_BUF_LEN];
static int idx;
static int dbg_en = 1;

static int mputc(int ch)
{
	msg[idx] = ch;
	idx = (idx + 1) % sizeof(msg);
	return ch;
}

int mprintf(const char *fmt, ...)
{
	char tmp[256];
	int n;
	va_list arg;

	if (dbg_en == 0)
		return 0;

	va_start(arg, fmt);
	n = vsnprintf(tmp, sizeof(tmp), fmt, arg);
	va_end(arg);
	int i;
	for (i = 0; i < n; ++i) {
		mputc(tmp[i]);
	}
	return n;
}

int dbg_enable(void)
{
	int tmp = dbg_en;
	dbg_en = 1;
	return tmp;
}

int dbg_disable(void)
{
	int tmp = dbg_en;
	dbg_en = 0;
	return tmp;
}

int dbg_restore(int enable)
{
	int tmp = dbg_en;
	dbg_en = enable;
	return tmp;
}


